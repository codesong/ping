/*************************************************************************
* File Name: Poll.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时10分21秒
*************************************************************************/

#ifndef __POLL_H__
#define __POLL_H__

namespace ping
{
class Poll: public Poller
{
public:
    Poll() = default; 
    ~Poll() = default;

    Timestamp poll(int timeoutMs, vector<ChannelPtr> &activeChannels) override;
    void addChannel(ChannelPtr channel) override;
    void delChannel(ChannelPtr channel) override;
    void updateChannel(ChannelPtr channel) override;

private:
    void fillActiveChannels(int numEvents, vector<ChannelPtr> &activeChannels);

private:
    vector<struct pollfd> m_vecPollfd;
};
}

#endif

