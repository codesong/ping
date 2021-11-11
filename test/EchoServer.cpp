/*************************************************************************
* File Name: EchoServer.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年10月22日 星期五 17时03分06秒
*************************************************************************/

#include "../src/Log.h"
#include "../src/net/TcpServer.h"
#include "../src/net/InetAddress.h"

using namespace ping;

class EchoServer
{
public:
    EchoServer(const InetAddress &serverAddr)
        : m_serverAddr(serverAddr), m_server(serverAddr, "EchoServer")
    {
        m_server.setConnectCallback(std::bind(&EchoServer::onConnected, this, _1));
        m_server.setDisconnectCallback(std::bind(&EchoServer::onDisconnected, this, _1));
        m_server.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
    }

    void start() { m_server.start(); }

private:
    void onConnected(const ConnectionPtr conn)
    {
        LOG_TRACE << conn->peerAddr().ipPort() << " -> "
            << conn->localAddr().ipPort() << " is UP.";
        LOG_INFO << conn->connectionInfo();
        conn->send("hello\n");
    }

    void onDisconnected(const ConnectionPtr conn)
    {
        LOG_TRACE << conn->peerAddr().ipPort() << " -> "
            << conn->localAddr().ipPort() << " is DOWN.";
    }

    void onMessage(const ConnectionPtr conn, Buffer &buf, const Timestamp &recvTime)
    {
        string msg(buf.readAll());
        conn->send(msg);
    }

private:
    TcpServer m_server;
    InetAddress m_serverAddr;
};

int main(int argc, char *argv[])
{
    InetAddress serverAddr(2222);
    EchoServer server(serverAddr);
    server.start();
}


