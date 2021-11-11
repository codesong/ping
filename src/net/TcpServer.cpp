/*************************************************************************
* File Name: TcpServer.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分48秒
*************************************************************************/

#include "TcpServer.h"

namespace ping
{
TcpServer::TcpServer(const InetAddress &serverAddr, const string &name, int threadNum, bool reusePort)
    : m_name(name), m_localAddr(serverAddr), m_ipPort(serverAddr.ipPort()), 
     m_eventLoopPool("name", threadNum), m_nextConnId(0), m_baseLoop(false, "BaseLoop")
{
    m_acceptor = std::make_unique<Acceptor>(&m_baseLoop, serverAddr, reusePort);
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    m_eventLoopPool.start();
    m_acceptor->listen();
    m_baseLoop.start();
}

void TcpServer::stop()
{
    m_baseLoop.stop();
    m_eventLoopPool.stop();
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    EventLoopPtr ioEventLoop = m_eventLoopPool.nextLoop();
    
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
}

void TcpServer::closeConnection(const ConnectionPtr &conn)
{
    m_baseLoop.runInLoop(std::bind(&TcpServer::removeConnection, this, conn));
}

void TcpServer::removeConnection(const ConnectionPtr &conn)
{
    m_mapConnection.erase(conn->name());
    EventLoopPtr ioEventLoop = conn->eventLoop();
    ioEventLoop->runInLoop(std::bind(&Connection::disconnected, conn));
}

}
