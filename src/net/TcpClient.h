/*************************************************************************
* File Name: TcpClient.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分59秒
*************************************************************************/

#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include "EventLoop.h"
#include "Connector.h"
#include "Connection.h"

namespace ping
{
class TcpClient: Noncopyable
{
public:
    TcpClient(EventLoop *eventLoop, const InetAddress &serverAddr, const string &name);
    ~TcpClient();

    void connect(bool reconnect = true);
    void disconnect();

    void send();

private:
    void newConnection(int sockfd, const InetAddress &serverAddr);
    void disconnection(const ConnectionPtr &conn);

private:
    bool m_reconnect; // 自动重连
    const string m_name;
    Connector m_connector;
    EventLoop *m_eventLoop;
    ConnectionPtr m_connection;

};

}

#endif

