/*************************************************************************
* File Name: TimeStamp.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 23时27分48秒
*************************************************************************/

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <sys/time.h>
#include "Noncopyable.h"

namespace ping
{

const int KMicroSecondsPerSecond = 1000 * 1000;

class TimeStamp: Noncopyable
{
public:

private:
    struct timeval m_time;
};

}

#endif

