/*************************************************************************
* File Name: Socket.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月07日 星期二 17时08分52秒
*************************************************************************/

#include "../Log.h"
#include "Socket.h"
#include "InetAddress.h"
#include <unistd.h>

namespace ping
{
int Socket::createTcpSocket(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    return sockfd;
}

int Socket::createUdpSocket(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
    return sockfd;
}

void Socket::bind(int sockfd, const InetAddress &addr)
{
    if(::bind(sockfd, addr.sockAddr(), addr.sockAddrLen()) < 0)
    {
        LOG_FATAL << "Socket::bind";
    }
}

void Socket::listen(int sockfd)
{
    if(::listen(sockfd, SOMAXCONN) < 0)
    {
        LOG_FATAL << "Socket::listen";
    }
}

int Socket::connect(int sockfd, const InetAddress &addr)
{
    return ::connect(sockfd, addr.sockAddr(), addr.sockAddrLen());
}

void Socket::close(int sockfd)
{
    if(::close(sockfd) < 0)
    {
        LOG_ERROR << "Socket::close";
    }
}

ssize_t Socket::read(int sockfd, void *buf, size_t count)
{
    return ::read(sockfd, buf, count);
}

ssize_t Socket::write(int sockfd, const void *buf, size_t count)
{
    return ::write(sockfd, buf, count);
}

int Socket::accept(int sockfd, InetAddress &addr)
{
    struct sockaddr_in6 addr6; 
    socklen_t addrlen = static_cast<socklen_t>(sizeof(addr6));
    int connfd = ::accept4(sockfd, static_cast<sockaddr *>((void *)&addr6), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd >= 0)
    {
        addr.setSockAddr(addr6);
    }else
    {
        int errorNo = errno;
        LOG_ERROR << "Socket::accept failed!";
        switch(errorNo)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO:
        case EPERM:
        case EMFILE:
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            LOG_FATAL << "unexpected error of accept";
            break;
        default:
            LOG_FATAL << "unknown error of accept";
            break;
        }
    }
    return connfd;
}

uint16_t Socket::hton16(uint16_t host)
{
    return htobe16(host);
}

uint32_t Socket::hton32(uint32_t host)
{
    return htobe32(host);
}

uint64_t Socket::hton64(uint64_t host)
{
    return htobe64(host);
}

uint16_t Socket::ntoh16(uint16_t net)
{
    return be16toh(net);
}

uint32_t Socket::ntoh32(uint32_t net)
{
    return be32toh(net);
}

uint64_t Socket::ntoh64(uint64_t net)
{
    return be32toh(net);
}

}
