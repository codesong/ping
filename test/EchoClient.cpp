/*************************************************************************
* File Name: EchoClient.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年11月23日 星期二 09时24分01秒
*************************************************************************/

#include "../src/Log.h"
#include "../src/Noncopyable.h"
#include "../src/net/TcpClient.h"
#include "../src/net/InetAddress.h"

using namespace ping;

class EchoClient: Noncopyable
{
public:
    using EventLoopPtr = std::shared_ptr<EventLoop>;

    EchoClient(const InetAddress &serverAddr, const string &id)
        : m_client(serverAddr, "EchoClient" + id)
    {
        m_client.setConnectCallback(std::bind(&EchoClient::onConnected, this, _1));
        m_client.setDisconnectCallback(std::bind(&EchoClient::onDisconnected, this, _1));
        m_client.setMessageCallback(std::bind(&EchoClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        m_client.connect();
    }

    void disconnect()
    {
        m_client.disconnect();
    }

    void send(const string &msg)
    {
        m_client.send(msg);
    }

private:
    void onConnected(const ConnectionPtr conn)
    {
        LOG_TRACE << conn->localAddr().ipPort() << " connect " 
            << conn->peerAddr().ipPort() << " success.";
        send("hello\n");
        LOG_INFO << "send msg hello.";
    }

    void onDisconnected(const ConnectionPtr conn)
    {
        LOG_TRACE << conn->localAddr().ipPort() << " disconnect " 
            << conn->peerAddr().ipPort() << ".";
    }

    void onMessage(const ConnectionPtr &conn, Buffer &buf, const Timestamp &recvTime)
    {
        string msg(buf.readAll());
        LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << recvTime.toString();
        disconnect();
    }

private:
    TcpClient m_client;
};

void idleLoop(int count)
{
    for(int i = 0; i < count; ++i)
    {
        //LOG_INFO << "idleLoop";
    }
}

int main(int argc, char *argv[])
{
    LOGGER.init("./log", argv[0], TRACE);
    InetAddress serverAddr(2222);
    EchoClient client(serverAddr, "1");
    client.connect();
    
   // idleLoop(1000);

    return 0;
}

