/*************************************************************************
* File Name: Connector.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时04分47秒
*************************************************************************/

#include "Socket.h"
#include "Connector.h"
#include "../Log.h"

namespace ping
{

using std::placeholders::_1;
Connector::Connector(EventLoopPtr eventLoop, const InetAddress &serverAddr)
    : m_connect(false), m_state(KDisconnected), 
      m_eventLoop(eventLoop), m_serverAddr(serverAddr)
{
}

Connector::~Connector()
{
    m_connect = false;
    if(KConnecting == m_state)
    {
        // 正在连接，则直接停止连接，状态设置为断开
        m_state = KDisconnected;
        resetChannel();
    }
    LOG_TRACE << "~Connector.";
}

void Connector::connect()
{
    m_eventLoop->runInLoop(std::bind(&Connector::realConnect, this));
}

void Connector::reconnect()
{
    m_eventLoop->runInLoop(std::bind(&Connector::realReconnect, this));
}

void Connector::realConnect() 
{ 
    if(m_connect) 
    { 
        LOG_FATAL << "Connector has request connect."; 
    } 
    m_connect = true; 

    int connectFd = Socket::createTcpSocket(m_serverAddr.family()); 
    if(connectFd < 0)
    {
        LOG_FATAL << "Socket::createTcpSocket error.";
    }

    int ret = Socket::connect(connectFd, m_serverAddr);
    int errorNo = (ret == 0) ? 0 : errno;
    switch(errorNo)
    {
    case 0:
    case EINTR:
    case EISCONN:
    case EINPROGRESS:
        connecting(connectFd);
        break;

    case EAGAIN:
    case EADDRINUSE:
    case ENETUNREACH:
    case ECONNREFUSED:
    case EADDRNOTAVAIL:
        retry(connectFd);
        break;

    case EPERM:
    case EBADF:
    case EACCES:
    case EFAULT:
    case ENOTSOCK:
    case EALREADY:
    case EAFNOSUPPORT:
        LOG_ERROR << "connect error in Connector::connect " << errorNo;
        Socket::close(connectFd);
        break;

    default:
        LOG_ERROR << "unexcepted error in Connector::connect " << errorNo;
        Socket::close(connectFd);
        break;
    }
}

void Connector::connecting(int connectFd)
{
    // socket fd 是nonblocking，所以connect之后要检查是否连接成功
    // connect成功，fd可读(发送缓冲区空闲)；connect失败，fd可读可写。
    // 故通过可写去检查连接状态
    m_state = KConnecting;
    m_channel = std::make_unique<Channel>(m_eventLoop.get(), connectFd);
    m_channel->setWriteCallback(std::bind(&Connector::handleWrite, this, _1));
    m_channel->setErrorCallback(std::bind(&Connector::handleError, this, _1));
    LOG_TRACE << "Connector fd " << connectFd << " enableWrite.";
    m_channel->enableWrite();
}

void Connector::realReconnect()
{
    m_connect = true;
    realConnect();
}

void Connector::retry(int connectFd)
{
    Socket::close(connectFd);
    m_state = KDisconnected;
    if(m_connect)
    {
        LOG_INFO << "Connector::retry Retry connect to " << m_serverAddr.ipPort();
        m_eventLoop->runInLoop(std::bind(&Connector::realConnect, this));
    }else
    {
        LOG_DEBUG << "not connect";
    }
}
    
int Connector::resetChannel()
{
    // 检查连接状态后，将Channel重置
    int connectFd = m_channel->fd();
    LOG_TRACE << "Connector fd " << connectFd << " disableAll, resetChannel.";
    m_channel->disableAll();
    m_channel->destory();
    m_channel.reset();
    return connectFd;
}

void Connector::handleWrite(const Timestamp &time)
{
    LOG_TRACE << "Connector::handleWrite " << m_state;
    if(KConnecting == m_state)
    {
        int connectFd = resetChannel();
        int errorNo = Socket::socketError(connectFd);
        if(errorNo)
        {
            LOG_ERROR << "Connector::handleWrite SO_ERROR = " << errorNo << " " << strerror(errorNo);
            retry(connectFd);
        }else if(Socket::isSelfConnect(connectFd))
        {
            LOG_WARN << "Connector::handleWrite - self connect";
            retry(connectFd);
        }else
        {
            m_state = KConnected;
            if(m_connect)
            {
                m_newConnectionCallback(connectFd, m_serverAddr);
            }else
            {
                Socket::close(connectFd);
            }
        }
    }
}

void Connector::handleError(const Timestamp &time)
{
    LOG_ERROR << "Connector::handleError state = " << m_state;
    if(KConnecting == m_state)
    {
        int connectFd = resetChannel(); 
        int errorNo = Socket::socketError(connectFd);
        LOG_ERROR << "Connector::handleError SO_ERROR = " << errorNo << " " << strerror(errorNo);
        retry(connectFd);
    }
}

}

