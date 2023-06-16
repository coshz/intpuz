#include <iostream>
#include <vector>
#include "24points.h"

using namespace std;

int main()
{
    // vector<int> data{3,3,8,8};
    // auto model = Model<4>();
    vector<int> data{23,29,77,83,65};
    auto model = Model<5>();

    auto exprs = model.solve(data,24,true);
    if(exprs.size()){
        for(auto &expr: exprs)
            cout << expr.to_str() << endl;
    } else {
        cout << "No solution.\n";
    }
}