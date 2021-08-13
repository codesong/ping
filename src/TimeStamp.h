/*************************************************************************
* File Name: TimeStamp.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 23时27分48秒
*************************************************************************/

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <string>
#include <stdint.h>
#include <sys/time.h>

namespace ping
{

const int KMicroSecondsPerSecond = 1000 * 1000;

using std::string;

enum TimeFmt
{
    YYYY,
    YYYYMM,
    YYYYMM2,
    YYYYMMDD,
    YYYYMMDD2,
    YYYYMMDDHH,
    YYYYMMDDHH2,
    YYYYMMDDHHMM,
    YYYYMMDDHHMM2,
    YYYYMMDDHHMMSS,
    YYYYMMDDHHMMSS2,
    YYYYMMDDHHMMSSMS,
    YYYYMMDDHHMMSSMS2,
};

class TimeStamp
{
public:
    TimeStamp(): m_time{0, 0} {}    
    explicit TimeStamp(const timeval &tv): m_time(tv) {}
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    TimeStamp(time_t sec, int64_t usec): m_time{sec, usec} {}
    ~TimeStamp() {}

    time_t secondsSinceEpoch() const { return m_time.tv_sec; }
    int64_t microSecondsSinceEpoch() const { return m_time.tv_sec + m_time.tv_usec * KMicroSecondsPerSecond; }

    bool valid() const { return m_time.tv_sec > 0; }
    void invalid() { m_time.tv_sec = m_time.tv_usec = 0; }

    TimeStamp addSeconds(double seconds);

    string toString(TimeFmt fmt);

private:
    struct timeval m_time;
};

inline bool operator==(const TimeStamp &lhs, const TimeStamp &rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator<(const TimeStamp &lhs, const TimeStamp &rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline double timeDiff(const TimeStamp &high, const TimeStamp &low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / KMicroSecondsPerSecond;
}

}

#endif

