/*************************************************************************
* File Name: Poll.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时10分31秒
*************************************************************************/

#include "Poll.h"

namespace ping
{
Timestamp Poll::poll(int timeoutMs, vector<ChannelPtr> &activeChannels)
{
    int numEvents = ::poll(&m_vecPollfd[0], m_vecPollfd.size(), timeoutMs);
    int errorNo = errno;
    Timestamp now = Util::currTime();
    if(numEvents > 0)
    {
        LOG_TRACE << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
    }else if(0 == numEvents)
    {
        LOG_TRACE << "nothing happened";
    }else
    {
        if(errorNo != EINTR)
        {
            LOG_ERROR << "Poll::poll()";
        }
    }
}

void Poll::addChannel(ChannelPtr channel)
{

}

void Poll::delChannel(ChannelPtr channel)
{
    LOG_TRACE << "delChannel fd = " << channel->fd();
    m_mapChannel.erase(channel->fd());
}

void Poll::updateChannel(ChannelPtr channel)
{

}

void Poll::fillActiveChannels(int numEvents, vector<ChannelPtr> &activeChannels)
{
    for(const pollfd &pfd: m_vecPollfd)
    {
        if(pfd->revents > 0) 
        {
            if(numEvents-- <= 0) break;
            map<int, ChannelPtr>::iterator it = m_mapChannel.find(pfd->fd);
            ChannelPtr channel = it->second;
            channel->setRevents(pfd->revents);
            activeChannels.push_back(channel);
        }
    }
}

}


