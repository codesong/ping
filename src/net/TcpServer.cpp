/*************************************************************************
* File Name: TcpServer.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分48秒
*************************************************************************/

#include "TcpServer.h"

namespace ping
{

TcpServe::TcpServe(const InetAddress &serverAddr, const string &name, bool reusePort)
    : m_eventLoopPool("name", 4), 
{

    m_baseLoop = std::make_shared<EventLoop>(false, "BaseLoop");
    m_acceptor = std::make_shared<Acceptor>(m_baseLoop, serverAddr, reusePort);
}

TcpServe::~TcpServe()
{

}

void TcpServe::start()
{
    m_eventLoopPool.start();
    m_acceptor->listen();
    m_baseLoop->start();
}

void TcpServe::stop()
{
    m_baseLoop->stop();
    m_eventLoopPool.stop();
}

void TcpServer::addConnection(int sockfd, const InetAddress &peerAddr)
{
    EventLoopPtr ioEventLoop = m_eventLoopPool.next();
    
    string connName = m_name;
    connName += m_ipPort;
    connName += std::to_string(getConnId());
    ConnectionPtr conn = std::make_shared<Connection>(ioEventLoop, connName, sockfd, m_localAddr, peerAddr);
    m_mapConnection[connName] = conn;
    conn->setConnectCallback(m_connectCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setDisconnectCallback(m_disconnectCallback);
    conn->setCloseCallback(std::bind(TcpServe::delConnection, this, _1));
}

void TcpServer::delConnection(const ConnectionPtr &conn)
{
    m_baseLoop->executeInLoop(std::bind(TcpServe::removeConnection, this, conn));
}

void TcpServe::removeConnection(const ConnectionPtr &conn)
{
    m_mapConnection.erase(conn->name());
    EventLoopPtr ioEventLoop = conn->getEventLoop();
    ioEventLoop->executeInLoop(std::bind(&Connection::disconnected, conn));
}

}
