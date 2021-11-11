/*************************************************************************
* File Name: Connection.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分21秒
*************************************************************************/

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "../Log.h"
#include "EventLoop.h"
#include "Connection.h"

namespace ping
{

using std::placeholders::_1;
Connection::Connection(EventLoopPtr eventLoop, const string &name, int sockfd, 
            const InetAddress &localAddr, const InetAddress &peerAddr)
    : m_sockfd(sockfd), m_localAddr(localAddr), m_peerAddr(peerAddr), m_eventLoop(eventLoop)
{
    m_channel = std::make_unique<Channel>(eventLoop.get(), sockfd);
    m_channel->setReadCallback(std::bind(&Connection::handleRead, this, _1));
    m_channel->setWriteCallback(std::bind(&Connection::handleWrite, this, _1));
    m_channel->setCloseCallback(std::bind(&Connection::handleClose, this, _1));
    m_channel->setErrorCallback(std::bind(&Connection::handleError, this, _1));
}

Connection::~Connection()
{

}

void Connection::connected()
{
    m_eventLoop->checkThread();
    m_channel->enableRead();
    m_connectCallback(shared_from_this());
}

void Connection::disconnected()
{
    m_eventLoop->checkThread();
    m_channel->disableAll();
    m_connectCallback(shared_from_this());

}

string Connection::connectionInfo() const
{
    char buf[1024] = {0x00};
    struct tcp_info tcpinfo;
    socklen_t len = sizeof(tcpinfo);
    int ret = getsockopt(m_sockfd, SOL_TCP, TCP_INFO, &tcpinfo, &len);
    if(0 == ret)
    {
        snprintf(buf, len, "unrecovered=%u "
                           "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                           "lost=%u retrans=%u rtt=%u rttvar=%u "
                           "sshthresh=%u cwnd=%u total_retrans=%u",
                tcpinfo.tcpi_retransmits, 
                tcpinfo.tcpi_rto, tcpinfo.tcpi_ato, tcpinfo.tcpi_snd_mss, tcpinfo.tcpi_rcv_mss,
                tcpinfo.tcpi_lost, tcpinfo.tcpi_retrans, tcpinfo.tcpi_rtt, tcpinfo.tcpi_rttvar,
                tcpinfo.tcpi_snd_ssthresh, tcpinfo.tcpi_snd_cwnd, tcpinfo.tcpi_total_retrans);
    }else
    {
        LOG_ERROR << "getTcpInfo::getsockopt TCP_INFO error";
    }
    return buf;
}

void Connection::send(Buffer &buf)
{
    send(buf.peek(), buf.readableBytes());
    buf.hasReadAll();
}

void Connection::send(const string_view &message)
{
    send(message.data(), message.length());
}

void Connection::send(const void *data, int len)
{
    if(true) 
    {
        if(m_eventLoop->inLoopThread())
        {
            sendInLoop(data, len);
        }else
        {
            m_eventLoop->runInLoop(std::bind(&Connection::sendInLoop, this, data, len));
        }
    }
}

void Connection::sendInLoop(const void *data, size_t len)
{
    m_eventLoop->checkThread();
    ssize_t nwrote = 0;
    size_t remain;
    bool faultError = false;
    // 应用层发送缓冲区为空，且没有关注POLLOUT事件 即应用层当前没有发送中的数据
    if(!m_channel->isWriting() && m_outputBuffer.readableBytes() == 0)
    {
        nwrote = Socket::write(m_channel->fd(), data, len);
        if(nwrote >= 0)
        {
            remain = len - nwrote;
            if(remain == 0 && m_writeCompleteCallback)
            {
                m_writeCompleteCallback(shared_from_this());
            }
        }else
        {
            nwrote = 0;
            if(errno != EWOULDBLOCK)
            {
                LOG_ERROR << "Connection::sendInLoop";
                if(errno == EPIPE || errno == ECONNRESET) // 对端关闭
                {
                    faultError = true;
                }
            }
        }
    }

    // OS的网络发送缓冲区满，放到应用层缓冲区，同时关注POLLOUT事件
    if(!faultError && remain > 0)
    {
        size_t oldLen = m_outputBuffer.readableBytes();
        if(oldLen + remain >= m_highWaterMark && oldLen < m_highWaterMark && m_highWaterMarkCallback)
        {
            m_highWaterMarkCallback(shared_from_this());
        }
        m_outputBuffer.write(static_cast<const char *>(data) + nwrote, remain);
        if(!m_channel->isWriting()) m_channel->enableWrite();
    }
}

void Connection::handleRead(const Timestamp &time)
{
    int errorNo = 0;
    size_t n = m_inputBuffer.readFd(m_channel->fd(), errorNo);
    if(n > 0)
    {
        m_messageCallback(shared_from_this(), m_inputBuffer, time);
    }else if(n == 0)
    {
        handleClose(time);
    }else
    {
        LOG_ERROR << "Connection::handleRead";
        handleError(time);
    }
}

void Connection::handleWrite(const Timestamp &time)
{
    if(m_channel->isWriting())
    {
        ssize_t n = Socket::write(m_channel->fd(), m_outputBuffer.peek(), m_outputBuffer.readableBytes());
        if(n > 0)
        {
            m_outputBuffer.hasRead(n);
            if(m_outputBuffer.readableBytes() == 0) // 发送完毕，取消关注EPOLLOUT事件
            {
                m_channel->disableWrite();
                if(m_writeCompleteCallback)
                {
                    m_writeCompleteCallback(shared_from_this());
                }

            }
        }else
        {
            LOG_ERROR << "Connection::handleWrite";
        }
    }else
    {
        LOG_TRACE << "Connecion fd = " << m_channel->fd() << " is down, no more writing";
    }
}

void Connection::handleClose(const Timestamp &time)
{
    m_channel->disableAll();
    m_disconnectCallback(shared_from_this());
    m_closeCallback(shared_from_this());
}

void Connection::handleError(const Timestamp &time)
{
    int errorNo = Socket::socketError(m_channel->fd());
    LOG_ERROR << "Connection::handleError SO_ERROR = " << errorNo << " " << strerror(errorNo);
}

}
