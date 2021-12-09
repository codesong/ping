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

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class TcpClient: Noncopyable
{
public:
    using ConnectorPtr = std::unique_ptr<Connector>;

    TcpClient(const InetAddress &serverAddr, const string &name = "", 
        EventLoopPtr eventLoop = nullptr, bool reconnect = true);
    ~TcpClient();

    void connect();
    void disconnect();

    void send(const string &msg);

    void setConnectCallback(const ConnectCallback &cb) { m_connectCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }
    void setDisconnectCallback(const DisconnectCallback &cb) { m_disconnectCallback = cb; }


private:
    void newConnection(int sockfd, const InetAddress &serverAddr);
    void closeConnection(const ConnectionPtr &conn);

private:
    const string m_name;
    bool m_connect;
    atomic<bool> m_reconnect; // 自动重连
    const bool m_commBaseloop;
    ConnectorPtr m_connector;
    EventLoopPtr m_eventLoop;
    ConnectionPtr m_connection;

    ConnectCallback m_connectCallback;
    MessageCallback m_messageCallback;
    DisconnectCallback m_disconnectCallback;
    WriteCompleteCallback m_writeCompleteCallback;
};

}

#endif

