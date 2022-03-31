#include <iostream>
#include <vector>
#include "24points.h"

using namespace std;

int main()
{
    vector<int> data{3,8,3,8};
    //vector<int> data{23,29,77,83,65};
    compModel<int> model{data};
    auto sts= model.solve(24);

    if(sts.size() == 0)
        cout<<"No solution.\n";
    else for(auto &st: sts)
    {
        for(auto &s:st) cout<<s<<' ';
        cout<<endl;
    }        
    
    return 0;

}