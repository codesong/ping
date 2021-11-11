/*************************************************************************
* File Name: Poller.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时08分40秒
*************************************************************************/

#ifndef __POLLER_H__
#define __POLLER_H__

#include <map>
#include <vector>
#include "Channel.h"
#include "../Timestamp.h"
#include "../Noncopyable.h"

namespace ping
{
using std::map;
using std::vector;
class Poller: Noncopyable
{
public:
    Poller() = default;
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, vector<Channel *> &activeChannels) = 0;
    virtual void addChannel(Channel *channel) = 0;
    virtual void delChannel(Channel *channel) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual bool hasChannel(Channel *channel) const
    {
        map<int, Channel *>::const_iterator it = m_mapChannel.find(channel->fd());
        return it != m_mapChannel.end() && it->second == channel;
    }

protected:
    map<int, Channel *> m_mapChannel;
};

}

#endif

