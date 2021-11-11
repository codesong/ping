/*************************************************************************
* File Name: TcpServer.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分42秒
*************************************************************************/

#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include <map>
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoopPool.h"

namespace ping
{
using std::map;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
// base eventloop 专门为TcpServer的Acceptor作为监听使用，存在于主线程，由Acceptor操作
// eventLoopPool, io eventloop，用来处理io请求，单独开线程处理io请求，避免io请求耗时过长影响连接
// 若io eventloop不存在，io请求和acceptor请求共用base eventloop
class TcpServer: Noncopyable
{
public:
    using AcceptorPtr = std::unique_ptr<Acceptor>;
    using EventLoopPtr = std::shared_ptr<EventLoop>;

    TcpServer(const InetAddress &serverAddr, const string &name = "", int threadNum = 0, bool reusePort = false);
    ~TcpServer();

    void start();
    void stop();

    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setConnectCallback(const ConnectCallback &cb) { m_connectCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setDisconnectCallback(const DisconnectCallback &cb) { m_disconnectCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void closeConnection(const ConnectionPtr &conn);
    void removeConnection(const ConnectionPtr &conn);

private:
    string m_name;
    string m_ipPort;
    int m_nextConnId;
    InetAddress m_localAddr; 
    AcceptorPtr m_acceptor;
    EventLoop m_baseLoop; 
    EventLoopPool m_eventLoopPool;
    CloseCallback m_closeCallback;
    ConnectCallback m_connectCallback;
    MessageCallback m_messageCallback;
    DisconnectCallback m_disconnectCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    map<string, ConnectionPtr> m_mapConnection;
};

}

#endif

