/*************************************************************************
* File Name: Connection.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分14秒
*************************************************************************/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <memory>
#include "Buffer.h"
#include "Channel.h"
#include "InetAddress.h"
#include "../Timestamp.h"
#include "../Noncopyable.h"

namespace ping
{

class Channel;
class EventLoop;
class Connection;

using ConnectionPtr = std::shared_ptr<Connection>;
using CloseCallback = std::function<void (const ConnectionPtr)>;
using ConnectCallback = std::function<void (const ConnectionPtr)>;
using DisconnectCallback = std::function<void (const ConnectionPtr)>;
using WriteCompleteCallback = std::function<void (const ConnectionPtr)>;
using MessageCallback = std::function<void (const ConnectionPtr, Buffer &buffer, const Timestamp &recvTime)>;

class Connection: Noncopyable, public std::enable_shared_from_this<Connection>
{
public:
    using EventLoopPtr = std::shared_ptr<EventLoop>;

    Connection(EventLoopPtr eventLoop, const string &name, int sockfd, 
            const InetAddress &localAddr, const InetAddress &peerAddr);
    ~Connection();

    void connected();
    void disconnected();
    
    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setConnectCallback(const ConnectCallback &cb) { m_connectCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setDisconnectCallback(const DisconnectCallback &cb) { m_disconnectCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

    string getConnectionInfo() const;
    EventLoopPtr getEventLoop() { return m_eventLoop };

private:
    void handleRead(const Timestamp &time);
    void handleWrite(const Timestamp &time);
    void handleClose(const Timestamp &time);
    void handleError(const Timestamp &time);

private:
    const int m_sockfd;
    const InetAddress m_localAddr;
    const InetAddress m_peerAddr;
    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
    Channel m_channel;
    EventLoopPtr m_eventLoop;

    CloseCallback m_closeCallback;
    ConnectCallback m_connectCallback;
    MessageCallback m_messageCallback;
    DisconnectCallback m_disconnectCallback;
    WriteCompleteCallback m_writeCompleteCallback;
};
}

#endif

