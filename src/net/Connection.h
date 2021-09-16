/*************************************************************************
* File Name: Connection.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分14秒
*************************************************************************/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "../Noncopyable.h"

namespace ping
{
class Channel;
class EventLoop;
class Connection: Noncopyable
{
public:
    Connection(EventLoop &loop, const string &name, int connFd, 
            const InetAddress &localAddr, const InetAddress &peerAddr);
    ~Connection();
    
    string getConnectionInfo() const;

private:
    const int m_connFd;
};
}

#endif

