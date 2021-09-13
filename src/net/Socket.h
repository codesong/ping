/*************************************************************************
* File Name: Socket.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月06日 星期一 21时05分06秒
*************************************************************************/

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdint.h>
#include <arpa/inet.h>

namespace ping
{
class InetAddress;
namespace Socket
{

int createTcpSocket(sa_family_t family);
int createUdpSocket(sa_family_t family);
void bind(int sockfd, const InetAddress &addr);
void listen(int sockfd);
int accept(int sockfd, InetAddress &addr);
int connect(int sockfd, const InetAddress &addr);
void close(int sockfd);

ssize_t read(int sockfd, void *buf, size_t count);
ssize_t write(int sockfd, const void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);

uint16_t hton16(uint16_t host);
uint32_t hton32(uint32_t host);
uint64_t hton64(uint64_t host);
uint16_t ntoh16(uint16_t net);
uint32_t ntoh32(uint32_t net);
uint64_t ntoh64(uint64_t net);

} // namespace Socket
} // namespace ping

#endif

