/*************************************************************************
* File Name: Acceptor.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时03分51秒
*************************************************************************/

#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <functional>
#include "Socket.h"
#include "EventLoop.h"
#include "../Noncopyable.h"

namespace ping
{

class InetAddress;
class Acceptor: Noncopyable
{
public:
    using EventLoopPtr = std::shared_ptr<EventLoop>;
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

    Acceptor(EventLoopPtr eventLoop, const NewConnectionCallback &cb,  const InetAddress &listenAddr, bool reusePort);
    ~Acceptor();

private:
    void listen();
    void handleRead();

private:
    int m_idleFd;
    const int m_listenFd;
    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
};
}

#endif

