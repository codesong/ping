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
#include <sys/uio.h>

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

int Socket::socketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
    if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen))
    {
        return errno;
    }else
    {
        return optval;
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

ssize_t Socket::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
    return ::readv(sockfd, iov, iovcnt);
}

int Socket::accept(int sockfd, InetAddress &addr)
{
    socklen_t addrlen = addr.sockAddrLen();
    int connfd = ::accept4(sockfd, addr.sockAddr(), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd < 0)
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

struct sockaddr_in6 Socket::localAddr(int sockfd)
{
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(addr));
    if(::getsockname(sockfd, static_cast<struct sockaddr *>((void *)&addr), &addrlen) < 0)
    {
        LOG_ERROR << "Socket::localAddr";
    }
    return addr;
}

struct sockaddr_in6 Socket::peerAddr(int sockfd)
{
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(addr));
    if(::getpeername(sockfd, static_cast<struct sockaddr *>((void *)&addr), &addrlen) < 0)
    {
        LOG_ERROR << "Socket::peerAddr";
    }
    return addr;
}

bool Socket::isSelfConnect(int sockfd)
{
    struct sockaddr_in6 localaddr = localAddr(sockfd);
    struct sockaddr_in6 peeraddr = peerAddr(sockfd);
    if(localaddr.sin6_family == AF_INET)
    {
        const struct sockaddr_in *laddr = static_cast<struct sockaddr_in *>((void *)&localaddr);
        const struct sockaddr_in *paddr = static_cast<struct sockaddr_in *>((void *)&peeraddr);
        return laddr->sin_port == paddr->sin_port && laddr->sin_addr.s_addr == paddr->sin_addr.s_addr; 
    }else if(localaddr.sin6_family == AF_INET6)
    {
        return localaddr.sin6_port == peeraddr.sin6_port && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof(localaddr.sin6_addr)) == 0;
    }else
    {
        return false;
    }
}

}


















