/*************************************************************************
* File Name: Exception.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月12日 星期四 05时08分59秒
*************************************************************************/

#include <memory>
#include <cxxabi.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include "Exception.h"

namespace ping
{

Exception::Exception(const string &msg)
    : m_message(msg), m_stackInfo(stackTrace())
{
}

void Exception::demangleSymbol(string &symbol)
{
    const char KMangledSymbolPrefix[] = "_Z";
    const char KSymbolCharacters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    string::size_type pos = 0; 
    while(pos < symbol.size())
    {
        string::size_type mangledStart = symbol.find(KMangledSymbolPrefix, pos);
        if(mangledStart == string::npos) break;

        string::size_type mangledEnd = symbol.find_first_not_of(KSymbolCharacters, mangledStart);
        if(mangledEnd == string::npos) mangledEnd = symbol.size();
        string mangledSymbol(symbol, mangledStart, mangledEnd);
        int status = 0;
        char *demangled = abi::__cxa_demangle(symbol.c_str(), nullptr, nullptr, &status);
        std::unique_ptr<char, void (*)(void *)> demangledSymbol(demangled, std::free);
        if(0 == status)
        {
            symbol.erase(mangledStart, mangledEnd - mangledStart);
            symbol.insert(mangledStart, demangledSymbol.get());
            pos = mangledStart + strlen(demangledSymbol.get());
        }else
        {
            pos = mangledStart + 2;
        }
    }
}

string Exception::stackTrace()
{
    string stackInfo;
    const int KStackLength = 200;
    void *stack[KStackLength];
    int nptr = ::backtrace(stack, KStackLength);
    char **strings = ::backtrace_symbols(stack, nptr);
    std::unique_ptr<char *, void(*)(void *)> symbols(strings, std::free);

    for(int i = 0; i < nptr; ++i)
    {
        string symbol = symbols.get()[i];
        demangleSymbol(symbol);
        stackInfo.append(symbol);
        stackInfo.append("\n");
    }
    return stackInfo;
}

/*
string Exception::stackTrace()
{
    string stackInfo;
    const int KStackLength = 200;
    void *stack[KStackLength];
    int nptr = ::backtrace(stack, KStackLength);
    char **strings = ::backtrace_symbols(stack, nptr);
    if(strings)
    {
        for(int i = 0; i < nptr; ++i)    
        {
            auto demangle = [&](const char *symbol)
            {
                int status = 0;
                char *demangled = abi::__cxa_demangle(symbol, nullptr, nullptr, &status);
                if(0 == status && demangled)
                {
                    stackInfo.append(demangled);
                    free(demangled);
                }else
                {
                    stackInfo.append(symbol);
                }
                stackInfo.push_back('\n');
            };

            demangle(strings[i]);
        }
        free(strings);
    }
    return stackInfo;
}*/

/*
string Exception::stackTrace()
{
    string stackInfo;
    const int KStackLength = 200;
    void *stack[KStackLength];
    int nptr = ::backtrace(stack, KStackLength);
    char **strings = ::backtrace_symbols(stack, nptr);
    if(strings)
    {
        size_t len = 256;
        char *demangled = static_cast<char *>(::malloc(len));
        for(int i = 0; i < nptr; ++i)    
        {
            auto demangle = [&](const char *symbol)
            {
                int status = 0;
                char tmp[128] = {0x00};
                memset(demangled, 0, sizeof(demangled));
                if(1 == sscanf(symbol, "%*[^(]%*[^_]%255[^)+]", tmp))
                {
                    char *ret = abi::__cxa_demangle(tmp, demangled, &len, &status);
                    if(0 == status)
                    {
                        demangled = ret;
                        stackInfo.append(demangled);
                    }else
                    {
                        stackInfo.append(symbol);
                    }
                    stackInfo.push_back('\n');
                }else if(1 == sscanf(symbol, "%225s", tmp))
                {
                    stackInfo.append(tmp);
                    stackInfo.push_back('\n');
                }
            };

            demangle(strings[i]);
        }
        free(demangled);
        free(strings);
    }
    return stackInfo;
}
/*
string Exception::stackTrace()
{
    string stackInfo;
    const int max_frames = 200;
    void *frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char **strings = ::backtrace_symbols(frame, nptrs);
    if (strings)
    {
        size_t len = 256;
        char *demangled = static_cast<char *>(::malloc(len));
        for (int i = 1; i < nptrs; ++i) 
        {
            {
                char *left_par = nullptr;
                char *plus = nullptr;
                for (char *p = strings[i]; *p; ++p)
                {
                    if (*p == '(')
                        left_par = p;
                    else if (*p == '+')
                        plus = p;
                }
                
                if (left_par && plus)
                {
                    *plus = '\0';
                    int status = 0;
                    char *ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
                    *plus = '+';
                    if (status == 0)
                    {
                        demangled = ret; 
                        stackInfo.append(strings[i], left_par + 1);
                        stackInfo.append(demangled);
                        stackInfo.append(plus);
                        stackInfo.push_back('\n');
                        continue;
                    }
                }
            }
            stackInfo.append(strings[i]);
            stackInfo.push_back('\n');
        }
        free(demangled);
        free(strings);
    }
    return stackInfo;
}
*/
}
