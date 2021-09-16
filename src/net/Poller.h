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
    ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, vector<ChannelPtr> &activeChannels) = 0;
    virtual void addChannel(ChannelPtr channel) = 0;
    virtual void delChannel(ChannelPtr channel) = 0;
    virtual void updateChannel(ChannelPtr channel) = 0;
    virtual bool hasChannel(ChannelPtr channel) const
    {
        map<int, ChannelPtr>::const_iterator it = m_mapChannel.find(channel->fd());
        return it != m_mapChannel.end() && it->second == channel;
    }

protected:
    map<int, ChannelPtr> m_mapChannel;
};

}

#endif

