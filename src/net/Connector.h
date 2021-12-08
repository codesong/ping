/*************************************************************************
* File Name: Connector.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时04分33秒
*************************************************************************/

#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "EventLoop.h"
#include "InetAddress.h"

namespace ping
{
// TcpClient使用，客户端可能需要重复连接
class Connector: Noncopyable
{
public:
    using ChannelPtr = std::unique_ptr<Channel>;
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

    Connector(EventLoopPtr eventLoop, const InetAddress &serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        m_newConnectionCallback = cb;
    }

    void connect(); 
    void reconnect(); 

private:
    void realConnect();
    void realReconnect();
    void connecting(int connectFd);
    void retry(int connectFd);
    int resetChannel();
    void handleWrite(const Timestamp &time);
    void handleError(const Timestamp &time);

private:
    enum State
    {
        KDisconnected,
        KConnecting,
        KConnected,
    };

private:
    bool m_connect;
    State m_state;
    ChannelPtr m_channel; 
    EventLoopPtr m_eventLoop;
    const InetAddress m_serverAddr;
    NewConnectionCallback m_newConnectionCallback;
};
}

#endif

