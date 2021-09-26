/*************************************************************************
* File Name: Connection.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分14秒
*************************************************************************/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

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
class Connection: Noncopyable, public std::enable_shared_from_this<Connection>
{
public:
    Connection(EventLoopPtr eventLoop, const string &name, int connFd, 
            const InetAddress &localAddr, const InetAddress &peerAddr);
    ~Connection();

    void connected();
    void disconnected();
    
    void setCloseCallback(const ConnectionPtr &cb) { m_closeCallback = cb; }
    void setConnectCallback(const ConnectionPtr &cb){ m_connectCallback = cb; }
    void setDisconnectCallback(const ConnectionPtr &cb) { m_disconnectCallback = cb; }
    void setWriteCompleteCallback(const ConnectionPtr &cb) { m_writeCompleteCallback = cb; }

    string getConnectionInfo() const;

private:
    void handleRead(Timestamp time);
    void handleWrite();
    void handleClose();
    void handleError();

private:
    const int m_connFd;
    const InetAddress m_localAddr;
    const InetAddress m_peerAddr;
    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
    Channel m_channel;
    EventLoopPtr m_eventLoop;

    CloseCallback m_closeCallback;
    ConnectCallback m_connectCallback;
    DisconnectCallback m_disconnectCallback;
    WriteCompleteCallback m_writeCompleteCallback;
};
}

#endif

