/*************************************************************************
* File Name: TcpClient.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分06秒
*************************************************************************/

#include "TcpClient.h"

namespace ping
{

TcpClient::TcpClient(EventLoop *eventLoop, const InetAddress &serverAddr, const string &name)
    : m_reconnect(false), m_eventLoop(eventLoop), m_name(name),
      m_connector(m_eventLoop, serverAddr)
{

}

TcpClient::~TcpClient()
{

}

void TcpClient::connect(bool reconnect)
{
    m_reconnect = reconnect;
    m_connector.connect();
}

void TcpClient::disconnect()
{
    
}

void TcpClient::newConnection(int sockfd, const InetAddress &serverAddr)
{

}

void TcpClient::disconnection(const ConnectionPtr &conn)
{
    
}

}

