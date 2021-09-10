/*************************************************************************
* File Name: InetAddress.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月09日 星期四 17时26分11秒
*************************************************************************/

#include "InetAddress.h"
#include "../Exception.h"
#include <string.h>
#include <sys/socket.h>

namespace ping
{

InetAddress::InetAddress(uint16_t port, bool loopback, bool ipv6)
{
    if(ipv6)
    {
        memset(&m_addr6, 0, sizeof(m_addr6));
        m_addr6.sin6_family = AF_INET6;
        m_addr6.sin6_addr = loopback ? in6addr_loopback : in6addr_any;
        m_addr6.sin6_port = Socket::hton16(port);
    }else
    {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        in_addr_t ip = loopback ? INADDR_LOOPBACK : INADDR_ANY;
        m_addr.sin_addr.s_addr = Socket::hton32(ip);
        m_addr.sin_port = Socket::hton16(port);
    }
}

InetAddress::InetAddress(const string ip, uint16_t port, bool ipv6)
{
    if(ipv6)
    {
        memset(&m_addr6, 0, sizeof(m_addr6));
        m_addr6.sin6_family = AF_INET6;
        CHECK(1 == ::inet_pton(AF_INET6, ip.c_str(), &m_addr6.sin6_addr));
        m_addr6.sin6_port = Socket::hton16(port);
    }else
    {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        CHECK(1 == ::inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr));
        m_addr.sin_port = Socket::hton16(port);
    }
}
    
string InetAddress::ip() const
{
    char buf[64] = {0x00};
    if(family() == AF_INET)
    {
        CHECK(buf == ::inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t>(sizeof(buf))));
    }else if(family() == AF_INET6)
    {
        CHECK(buf == ::inet_ntop(AF_INET6, &m_addr6.sin6_addr, buf, static_cast<socklen_t>(sizeof(buf))));
    }
    return buf;
}

uint16_t InetAddress::port() const
{
    return Socket::ntoh16(m_addr.sin_port);
}

string InetAddress::ipPort() const
{
    string ipPortStr;
    if(family() == AF_INET6)
    {
        ipPortStr = "[" + ip() + "]:" + std::to_string(port());

    }else if(family() == AF_INET)
    {
        ipPortStr = ip() + ":" + std::to_string(port());
    }
    return ipPortStr;
}

void InetAddress::setScopeId(uint32_t scopeId)
{
    if(family() == AF_INET6)
        m_addr6.sin6_scope_id = scopeId;
}

}


