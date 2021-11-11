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
    using EventLoopPtr = std::shared_ptr<EventLoop>;

    Connector(EventLoopPtr eventLoop, const InetAddress &serverAddr);
    ~Connector();

    void connect(); 
    void reconnect(); 
    void disconnect();

private:
    void realConnect();
    void realReconnect();
    void realDisconnect();
    void connecting(int connectFd);
    void retry(int connectFd);
    int resetChannel();
    void handleWrite(const Timestamp &time);
    void handleError(const Timestamp &time);

private:
    enum States
    {
        KDisconnected,
        KConnecting,
        KConnected,
    };

private:
    bool m_connect;
    States m_state;
    ChannelPtr m_channel; 
    EventLoopPtr m_eventLoop;
    InetAddress m_serverAddr;
};
}

#endif

