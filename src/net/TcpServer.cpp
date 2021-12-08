/*************************************************************************
* File Name: TcpServer.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分48秒
*************************************************************************/

#include "../Log.h"
#include "TcpServer.h"

namespace ping
{
TcpServer::TcpServer(const InetAddress &serverAddr, const string &name, 
    EventLoopPtr baseLoop, int threadNum, bool reusePort)
    : m_name(name), m_nextConnId(0), m_localAddr(serverAddr), m_ipPort(serverAddr.ipPort()), 
     m_baseLoop(baseLoop ? baseLoop : std::make_shared<EventLoop>()), 
     m_eventLoopPool("name", threadNum), m_commBaseLoop(baseLoop ? true : false)
{
    m_acceptor = std::make_unique<Acceptor>(m_baseLoop, serverAddr, reusePort);
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    LOG_TRACE << "TcpServer::start";
    m_acceptor->listen();
    if(!m_commBaseLoop) m_baseLoop->start();
}

void TcpServer::stop()
{
    LOG_TRACE << "TcpServer::stop";
    if(!m_commBaseLoop) m_baseLoop->stop();
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    LOG_TRACE << "new connection: " << peerAddr.ipPort();
    EventLoopPtr ioEventLoop = m_eventLoopPool.nextLoop();
    if(!ioEventLoop) ioEventLoop = m_baseLoop;
    string connName = m_name;
    connName += m_ipPort;
    connName += std::to_string(++m_nextConnId);
    ConnectionPtr conn = std::make_shared<Connection>(ioEventLoop, connName, sockfd, m_localAddr, peerAddr);
    m_mapConnection[connName] = conn;
    conn->setConnectCallback(m_connectCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setDisconnectCallback(m_disconnectCallback);
    conn->setCloseCallback(std::bind(&TcpServer::closeConnection, this, _1));
    
    ioEventLoop->runInLoop(std::bind(&Connection::connected, conn));
}

void TcpServer::closeConnection(const ConnectionPtr &conn)
{
    LOG_TRACE << "close connection.";
    m_baseLoop->runInLoop(std::bind(&TcpServer::removeConnection, this, conn));
}

void TcpServer::removeConnection(const ConnectionPtr &conn)
{
    LOG_TRACE << "remove connection.";
    m_mapConnection.erase(conn->name());
    EventLoopPtr ioEventLoop = conn->eventLoop();
    ioEventLoop->runInLoop(std::bind(&Connection::disconnected, conn));
}

}
