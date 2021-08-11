/*************************************************************************
* File Name: Util.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 21时42分12秒
*************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

namespace ping
{

using std::string;

extern thread_local pid_t t_threadId;
extern thread_local string t_threadName;

namespace Util
{
pid_t CurrThreadId();
const string CurrThreadName();

} // namespace Util

} // namespace ping

#endif
