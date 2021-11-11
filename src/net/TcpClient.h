/*************************************************************************
* File Name: TcpClient.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分59秒
*************************************************************************/

#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include "EventLoop.h"

namespace ping
{
class TcpClient: Noncopyable
{
public:
    TcpClient(const InetAddress &serverAddr, const string &name);
    ~TcpClient();

    void connect();
    void disconnect();

    void send();

private:
    EventLoop m_eventLoop;
    ConnectionPtr m_connection;

};

}

#endif

