/*************************************************************************
* File Name: Connection.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时06分21秒
*************************************************************************/

#include "Connection.h"

namespace ping
{

Connection::Connection(EventLoopPtr eventLoop, const string &name, int connFd, 
            const InetAddress &localAddr, const InetAddress &peerAddr)
{

}

Connection::~Connection()
{

}

void Connection::connected()
{
    m_eventLoop->checkThread();
    m_channel->enableRead();
    m_connectCallback(shared_from_this());
}

void Connection::disconnected()
{
    m_eventLoop->checkThread();
    m_channel->disableAll();
    m_connectCallback(shared_from_this());

}

string Connection::getConnectionInfo() const
{
    char buf[1024] = {0x00};
    struct tcp_info tcpinfo;
    socklent_t len = sizeof(tcpinfo);
    int ret = getsockopt(m_sockfd, SOL_TCP, TCP_INFO, tcpinfo, &len);
    if(0 == ret)
    {
        snprintf(buf, len, "unrecovered=%u "
                           "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                           "lost=%u retrans=%u rtt=%u rttvar=%u "
                           "sshthresh=%u cwnd=%u total_retrans=%u",
                tcpinfo.tcpi_retransmits, 
                tcpinfo.tcpi_rto, tcpinfo.tcpi_ato, tcpinfo.tcpi_snd_mss, tcpinfo.tcpi_rcv_mss,
                tcpinfo.tcpi_lost, tcpinfo.tcpi_retrans, tcpinfo.tcpi_rtt, tcpinfo.tcpi_rttvar,
                tcpinfo.tcpi_snd_ssthresh, tcpinfo.tcpi_snd_cwnd, tcpinfo.tcpi_total_retrans);
    }else
    {
        LOG_ERROR << "getTcpInfo::getsockopt TCP_INFO error";
    }
    return buf;
}

}
