/*************************************************************************
* File Name: TcpServer.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分42秒
*************************************************************************/

#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include ""

namespace ping
{

// base eventloop 专门为TcpServer的Acceptor作为监听使用，存在于主线程，由Acceptor操作
// eventLoopPool, io eventloop，用来处理io请求，单独开线程处理io请求，避免io请求耗时过长影响连接
// 若io eventloop不存在，io请求和acceptor请求共用base eventloop
class TcpServer: Noncopyable
{
public:
    using EventLoopPtr = std::shared_ptr<EventLoop>;
    using AcceptorPtr = std::unique_ptr<Acceptor>;

    TcpServe(const InetAddress &serverAddr, const string &name = "", bool reusePort = false);
    ~TcpServe();

    void start();
    void stop();

private:
    void addConnection(int sockfd, const InetAddress &peerAddr);
    void delConnection(const ConnectionPtr &conn);
    void removeConnection(const ConnectionPtr &conn);

private:
    int getConnId();

private:
    string m_name;
    string m_ipPort;
    int m_nextConnId;
    AcceptorPtr m_acceptor;
    EventLoopPtr m_baseLoop; 
    EventLoopPool m_eventLoopPool;
    map<string, ConnectionPtr> m_mapConnection;
};

}

#endif

