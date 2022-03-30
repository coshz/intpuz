#include <iostream>
#include <vector>
#include "24points.h"

using namespace std;


/** FOR TEST
============================================*/

void prtvec(vector<int> &v)
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

    auto u = numsAll<int>(vector<int>{1,5,3,5},true);
    
    for (auto &v:u)
    {
        for(auto &i :v)
            cout<<i<<'\t';
        cout<<endl;
    }

    cout<<u.size()<<endl;
    // auto u = opsAll(3,vector<Operator>{ADD,SUB,MUL,DIV},true);
    // for (auto &v:u)
    // {
    //     for(auto &i :v)
    //         cout<<i<<'\t';
    //     cout<<endl;
    // }

    // cout<<u.size()<<endl;
    for(int i = 1; i< 10; i++)
    {
        auto x = binaryTreesAll(i);
        cout<<x.size()<<endl;
    }
}
/*============================================
**/


int main()
{
    vector<int> data{3,8,3,8};
    //vector<int> data{23,29,77,83,65};
    
    compModel<int> model{data};
    auto sts= model.solve(24);

    if(sts.size() == 0)
        cout<<"No solution.\n";
    else
    {
        for(auto &st: sts)
        {
            for(auto &s:st)
                cout<<s<<' ';
            cout<<endl;
        }        
    }
    
    return 0;

}