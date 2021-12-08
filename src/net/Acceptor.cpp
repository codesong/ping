/*************************************************************************
* File Name: Acceptor.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时03分58秒
*************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include "../Log.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "../Exception.h"

namespace ping
{
Acceptor::Acceptor(EventLoopPtr eventLoop, const InetAddress &listenAddr, bool reusePort)
    : m_listenFd(Socket::createTcpSocket(listenAddr.family())), 
      m_channel(eventLoop.get(), m_listenFd), m_idleFd(open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    if(m_listenFd < 0)
    {
        LOG_FATAL << "Socket::createTcpSocket error.";
    }
    int optval = 1;
    CHECK_RETZERO(::setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval))));
    optval = reusePort ? 1 : 0; 
    CHECK_RETZERO(::setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval))));
    Socket::bind(m_listenFd, listenAddr);
    m_channel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    LOG_TRACE << "Acceptor fd " << m_listenFd << " disableAll, ~Acceptor.";
    m_channel.disableAll();
    m_channel.destory();
    close(m_idleFd);
    close(m_listenFd);
}

void Acceptor::listen()
{
    Socket::listen(m_listenFd);
    LOG_TRACE << "Acceptor fd " << m_listenFd << " enableRead, listen.";
    m_channel.enableRead();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = Socket::accept(m_listenFd, peerAddr);
    if(connfd >= 0)
    {
        if(m_newConnectionCallback) 
        {
            m_newConnectionCallback(connfd, peerAddr);
        }else
        {
            Socket::close(connfd);
        }
    }else
    {
        LOG_ERROR << "Acceptor::handleRead";
        if(errno == EMFILE)
        {
            Socket::close(m_idleFd);
            m_idleFd = Socket::accept(m_listenFd, peerAddr);
            Socket::close(m_idleFd);
            m_idleFd = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

}
