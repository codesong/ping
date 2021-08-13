/*************************************************************************
* File Name: Singleton.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月12日 星期四 01时48分24秒
*************************************************************************/

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <memory>
#include "Noncopyable.h"

namespace ping
{

template<typename T>
class Singleton: Noncopyable
{
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T &instance()
    {
        static T v;
        return v;
    }
};

template<typename T>
class SingletonPtr: Noncopyable
{
public:
    SingletonPtr() = delete;
    ~SingletonPtr() = delete;

    static std::shared_ptr<T> &instance()
    {
        static std::shared_ptr<T> v = std::make_shared<T>();
        return v;
    }
};

}

#endif

