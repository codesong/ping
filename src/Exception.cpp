/*************************************************************************
* File Name: Exception.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月12日 星期四 05时08分59秒
*************************************************************************/

#include <cxxabi.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include "Exception.h"

namespace ping
{

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

                }
                char *ret = abi::__cxa_demangle(symbol, demangled, &len, &status);
                if(0 == status)
                {
                    demangled = ret;
                    stackInfo.append(demangled);
                }else
                {
                    stackInfo.append(symbol);
                }
                stackInfo.push_back('\n');
            };

            demangle(strings[i]);
        }
        free(demangled);
        free(strings);
    }
    return stackInfo;
}

}
