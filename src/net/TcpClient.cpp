/*************************************************************************
* File Name: TcpClient.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分06秒
*************************************************************************/

#include "../Log.h"
#include "TcpClient.h"

namespace ping
{

TcpClient::TcpClient(const InetAddress &serverAddr, const string &name, 
        EventLoopPtr eventLoop, bool reconnect)
    : m_eventLoop(eventLoop ? eventLoop : std::make_shared<EventLoop>()), 
      m_name(name), m_reconnect(reconnect), 
      m_commBaseloop(eventLoop ? true : false)
{
    LOG_TRACE << "TcpClient construct.";
    m_connector = std::make_unique<Connector>(m_eventLoop, serverAddr);
    m_connector->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1, _2));
}

TcpClient::~TcpClient()
{
    if(m_connection)
    {
        m_reconnect = false; // 关闭重连
        m_connection->close();
    }
}

void TcpClient::connect()
{
    if(m_connect)
    {
        LOG_ERROR << "TcpClient has called connect.";
        return;
    }
    LOG_TRACE << "TcpClient connect.";
    m_connect = true;
    m_connector->connect();
    if(!m_commBaseloop) m_eventLoop->start();
}

void TcpClient::disconnect()
{
    LOG_TRACE << "TcpClient disconnect.";
    m_connect = false;
    if(m_connection)
    {
        m_connection->shutdown();
    }
    if(!m_commBaseloop) m_eventLoop->stop();
}

void TcpClient::send(const string &msg)
{
    LOG_TRACE << "TcpClient send " << msg;
    if(m_connection)
    {
        m_connection->send(msg);
    }
}

void TcpClient::newConnection(int sockfd, const InetAddress &serverAddr)
{
    LOG_TRACE << "TcpClient new connection.";
    string connName = m_name + serverAddr.ipPort();
    InetAddress localAddr(Socket::localAddr(sockfd));
    m_connection = std::make_shared<Connection>(m_eventLoop, connName, sockfd, localAddr, serverAddr);
    m_connection->setConnectCallback(m_connectCallback);
    m_connection->setMessageCallback(m_messageCallback);
    m_connection->setWriteCompleteCallback(m_writeCompleteCallback);
    m_connection->setDisconnectCallback(m_disconnectCallback);
    m_connection->setCloseCallback(std::bind(&TcpClient::closeConnection, this, _1));
    m_connection->connected();
}

void TcpClient::closeConnection(const ConnectionPtr &conn)
{
    LOG_TRACE << "TcpClient close connection.";
    m_eventLoop->checkThread();
    m_connection.reset();
    m_eventLoop->runInLoop(std::bind(&Connection::disconnected, conn));
    if(m_reconnect && m_connect)
    {
        m_connector->reconnect();
    }
}

}

