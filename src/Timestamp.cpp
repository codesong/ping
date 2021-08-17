/*************************************************************************
* File Name: Timestamp.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 23时42分56秒
*************************************************************************/

#include "Timestamp.h"

namespace ping
{

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
{
    m_time.tv_sec = microSecondsSinceEpoch / KMicroSecondsPerSecond;
    m_time.tv_usec = microSecondsSinceEpoch % KMicroSecondsPerSecond;
}

Timestamp Timestamp::addSeconds(double seconds)
{
    int64_t microSeconds = seconds * KMicroSecondsPerSecond;
    return Timestamp(microSecondsSinceEpoch() + microSeconds);
}
/*
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
    */
string Timestamp::toString(TimeFmt fmt)
{
    char buf[64] = {0x00};
    struct tm t = *localtime(&m_time.tv_sec);
    switch(fmt)
    {
    case YYYY:
        snprintf(buf, sizeof(buf), "%.4d", t.tm_year+1900);
        break;
    case YYYYMM:
        snprintf(buf, sizeof(buf), "%.4d%.2d", t.tm_year+1900, t.tm_mon+1);
        break;
    case YYYYMM2:
        snprintf(buf, sizeof(buf), "%.4d-%.2d", t.tm_year+1900, t.tm_mon+1);
        break;
    case YYYYMMDD:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
        break;
    case YYYYMMDD2:
        snprintf(buf, sizeof(buf), "%.4d-%.2d-%.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
        break;
    case YYYYMMDDHH:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d %.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);
        break;
    case YYYYMMDDHH2:
        snprintf(buf, sizeof(buf), "%.4d-%.2d-%.2d %.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);
        break;
    case YYYYMMDDHHMM:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d %.2d:%.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min);
        break;
    case YYYYMMDDHHMM2:
        snprintf(buf, sizeof(buf), "%.4d-%.2d-%.2d %.2d:%.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min);
        break;
    case YYYYMMDDHHMMSS:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d %.2d:%.2d:%.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        break;
    case YYYYMMDDHHMMSS2:
        snprintf(buf, sizeof(buf), "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        break;
    case YYYYMMDDHHMMSSMS:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d %.2d:%.2d:%.2d.%.6d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, m_time.tv_usec);
        break;
    case YYYYMMDDHHMMSSMS2:
        snprintf(buf, sizeof(buf), "%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.6d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, m_time.tv_usec);
        break;
    case YYYYMMDDHHMMSSMS3:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d-%.2d%.2d%.2d.%.6d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, m_time.tv_usec);
        break;
    case YYYYMMDDHHMMSSMS4:
        snprintf(buf, sizeof(buf), "%.4d%.2d%.2d%.2d%.2d%.2d.%.6d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, m_time.tv_usec);
        break;
    }
    return buf;
}

}
