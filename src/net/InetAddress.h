/*************************************************************************
* File Name: InetAddress.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月07日 星期二 17时30分13秒
*************************************************************************/

#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include "Socket.h"
#include <string>
#include <arpa/inet.h>

namespace ping
{
using std::string;

class InetAddress
{
public:
    explicit InetAddress(uint16_t port, bool loopback = false, bool ipv6 = false);
    InetAddress(const string ip, uint16_t port, bool ipv6 = false);
    explicit InetAddress(const struct sockaddr_in &addr)
        : m_addr(addr) {}
    explicit InetAddress(const struct sockaddr_in6 &addr)
        : m_addr6(addr) {}

    sa_family_t family() const { return m_addr.sin_family; }
    string ip() const;
    uint16_t port() const;
    string ipPort() const;
    struct sockaddr *sockAddr() { return static_cast<struct sockaddr *>((void *)&m_addr6); }
    const struct sockaddr *sockAddr() const { return static_cast<const struct sockaddr *>((const void *)&m_addr6); }
    const socklen_t sockAddrLen() const { return static_cast<socklen_t>(sizeof(sockaddr_in6)); }

    void setScopeId(uint32_t scopeId); // IPv6

private: 
    union
    {
        struct sockaddr_in m_addr;
        struct sockaddr_in6 m_addr6;
    };
};

}

#endif


