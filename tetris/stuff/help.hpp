#ifndef __HELP_H__
#define __HELP_H__
#include <iostream>
#include <iomanip>
#include <string>

template<typename T>
void disp(T *arr, int n)
{
    for (auto i = 0; i < n; i++)
        std::cout << arr[i] << ", ";
    std::cout << std::endl;
}

template<class C>
void disp(const C &c)
{
    for(auto &v: c) std::cout << v << ", ";
    std::cout << std::endl; 
}
#endif
