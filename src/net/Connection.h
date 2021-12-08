/*************************************************************************
* File Name: Connection.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分14秒
*************************************************************************/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <string>
#include <memory>
#include "Buffer.h"
#include "Channel.h"
#include "InetAddress.h"
#include "../Timestamp.h"
#include "../Noncopyable.h"

namespace ping
{

class EventLoop;
class Connection;

using ConnectionPtr = std::shared_ptr<Connection>;
using CloseCallback = std::function<void (const ConnectionPtr)>;
using ConnectCallback = std::function<void (const ConnectionPtr)>;
using DisconnectCallback = std::function<void (const ConnectionPtr)>;
using WriteCompleteCallback = std::function<void (const ConnectionPtr)>;
using HighWaterMarkCallback = std::function<void (const ConnectionPtr)>;
using MessageCallback = std::function<void (const ConnectionPtr, Buffer &buffer, const Timestamp &recvTime)>;

using std::string;
using std::string_view;

class Connection: Noncopyable, public std::enable_shared_from_this<Connection>
{
public:
    using ChannelPtr = std::unique_ptr<Channel>;

    Connection(EventLoopPtr eventLoop, const string &name, int sockfd, 
            const InetAddress &localAddr, const InetAddress &peerAddr);
    ~Connection();

    void connected();
    void disconnected();

    void shutdown(); // 关闭写端
    void close();
    
    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setConnectCallback(const ConnectCallback &cb) { m_connectCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setDisconnectCallback(const DisconnectCallback &cb) { m_disconnectCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) 
    { 
        m_highWaterMarkCallback = cb; 
        m_highWaterMark = highWaterMark;
    }

    string name() { return m_name; }
    string connectionInfo() const;
    EventLoopPtr eventLoop() { return m_eventLoop; }
    const InetAddress &localAddr() const { return m_localAddr; }
    const InetAddress &peerAddr() const { return m_peerAddr; }

    void send(Buffer &message);
    void send(const string_view &message);
    void send(const void *data, int len);

private:
    void realClose();
    void realShutdown();
    void realSend(const void *data, size_t len);

    void handleRead(const Timestamp &time);
    void handleWrite(const Timestamp &time);
    void handleClose(const Timestamp &time);
    void handleError(const Timestamp &time);

private:
    enum State
    {
        KDisconnected,
        KConnecting,
        KConnected,
        KDisconnecting,
    };

    State m_state;
    const int m_sockfd;
    const string m_name;
    ChannelPtr m_channel;
    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
    size_t m_highWaterMark; // 用于防止发送数据过快
    EventLoopPtr m_eventLoop;
    const InetAddress m_localAddr;
    const InetAddress m_peerAddr;

    CloseCallback m_closeCallback;
    ConnectCallback m_connectCallback;
    MessageCallback m_messageCallback;
    DisconnectCallback m_disconnectCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    HighWaterMarkCallback m_highWaterMarkCallback;
};
}

#endif

