/*************************************************************************
* File Name: Poll.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时10分21秒
*************************************************************************/

#ifndef __POLL_H__
#define __POLL_H__

#include <memory>
#include "Poller.h"

namespace ping
{
class Poll: public Poller
{
public:
    Poll() = default; 
    ~Poll();

    Timestamp poll(int timeoutMs, vector<Channel *> &activeChannels) override;
    void addChannel(Channel *channel) override;
    void delChannel(Channel *channel) override;
    void updateChannel(Channel *channel) override;

private:
    void fillActiveChannels(int numEvents, vector<Channel *> &activeChannels);

private:
    vector<struct pollfd> m_vecPollfd;
};
}

#endif

