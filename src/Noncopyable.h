/*************************************************************************
* File Name: Noncopyable.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月09日 星期一 21时11分41秒
*************************************************************************/

#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace ping
{

class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;
};

}

#endif

