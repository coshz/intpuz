#include <iostream>
#include <vector>
#include "24points.h"

using namespace std;

template<class T>
void prtvec(vector<T> &v)
{
    for (auto &i: v)
        cout<<i<<' ';
    cout<<endl;
} 

void per()
{
    auto x = permutations(3,4,true);
    for (auto &i:x)
        prtvec(i);
}

void numsall()
{
    auto u = numsAll<int>(vector<int>{1,5,3,5},true);
    for (auto &v:u)
    {
        for(auto &i :v)
            cout<<i<<'\t';
        cout<<endl;
    }
    cout<<u.size()<<endl;

}

void opsall()
{
    auto w = opsAll(3,vector<Operator>{ADD,SUB,MUL,DIV},true);
    for (auto &v:w)
    {
        for(auto &i :v)
            cout<<i<<'\t';
        cout<<endl;
    }

    cout<<w.size()<<endl;
}

void btall()
{
    for(int i = 1; i< 10; i++)
    {
        auto x = binaryTreesAll(i);
        cout<<x.size()<<endl;
    }
}

int main()
{
    btall();
    return 0;
}