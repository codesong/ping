/*************************************************************************
* File Name: Poll.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时10分31秒
*************************************************************************/

#include <poll.h>
#include "Poll.h"
#include "../Log.h"
#include "../Util.h"

namespace ping
{

Poll::~Poll()
{
    m_mapChannel.clear();
    m_vecPollfd.clear();
}

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
    return now;
}

void Poll::addChannel(ChannelPtr channel)
{
    map<int, ChannelPtr>::iterator it = m_mapChannel.find(channel->fd());
    if(it != m_mapChannel.end())
    {
        LOG_FATAL << "fd[" << channel->fd() << "] is exist.";
    }else
    {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        m_vecPollfd.push_back(pfd);

        int index = static_cast<int>(m_vecPollfd.size()) - 1;
        channel->setIndex(index);
        m_mapChannel[channel->fd()] = channel;
    }
}

void Poll::delChannel(ChannelPtr channel)
{
    LOG_TRACE << "delChannel fd = " << channel->fd();
    m_mapChannel.erase(channel->fd());

    int index = channel->index();
    if(static_cast<size_t>(index) == m_vecPollfd.size() - 1)
    {
        m_vecPollfd.pop_back();
    }else
    {
        int fd = m_vecPollfd.back().fd;
        iter_swap(m_vecPollfd.begin() + index, m_vecPollfd.end() - 1);
        m_mapChannel[fd]->setIndex(index);
        m_vecPollfd.pop_back();
    }
}

void Poll::updateChannel(ChannelPtr channel)
{
    struct pollfd &pfd = m_vecPollfd[channel->index()];
    pfd.events = static_cast<short>(channel->events());
}

void Poll::fillActiveChannels(int numEvents, vector<ChannelPtr> &activeChannels)
{
    for(const pollfd &pfd: m_vecPollfd)
    {
        if(pfd.revents > 0) 
        {
            if(numEvents-- <= 0) break;
            map<int, ChannelPtr>::iterator it = m_mapChannel.find(pfd.fd);
            ChannelPtr channel = it->second;
            channel->setRevents(pfd.revents);
            activeChannels.push_back(channel);
        }
    }
}

}


