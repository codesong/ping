/*************************************************************************
* File Name: EPoll.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时10分56秒
*************************************************************************/

#ifndef __EPOLL_H__
#define __EPOLL_H__

namespace ping
{
class EPoll: public Poller
{
public:
    EPoll(); 
    ~EPoll();

    Timestamp poll(int timeoutMs, vector<ChannelPtr> &activeChannels) override;
    void addChannel(ChannelPtr channel) override;
    void delChannel(ChannelPtr channel) override;
    void updateChannel(ChannelPtr channel) override;

private:
    void fillActiveChannels(int numEvents, vector<ChannelPtr> &activeChannels);

private:
    int m_epollfd;
    vector<struct epoll_event> m_vecEvent; //保存活跃的fd event
};
}

#endif


