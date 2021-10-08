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
Acceptor::Acceptor(EventLoopPtr eventLoop, const NewConnectionCallback &cb,  const InetAddress &listenAddr, bool reusePort)
    : m_listenFd(Socket::createTcpSocket(listenAddr.family())), m_acceptChannel(eventLoop, m_listenFd),
      m_newConnectionCallback(cb), m_idleFd(open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    int optval = 1;
    CHECK_RETZERO(::setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval))));
    optval = reusePort ? 1 : 0; 
    CHECK_RETZERO(::setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval))));
    Socket::bind(m_listenFd, listenAddr);
    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));

    listen();
}

Acceptor::~Acceptor()
{
    m_acceptChannel.disableAll();
    close(m_idleFd);
    close(m_listenFd);
}

void Acceptor::listen()
{
    Socket::listen(m_listenFd);
    m_acceptChannel.enableRead();
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
