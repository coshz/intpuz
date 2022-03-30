example

```c++
    // vector<int> data{3,8,3,8};
    vector<int> data{23,29,77,83,65};
    
    compModel<int> model{data};
    auto sts= model.solve(24);

    if(sts.size() == 0)
        cout<<"No solution.\n";
    else 
    for(auto &st: sts) {
        for(auto &s:st)
            cout<<s<<' ';
        cout<<endl;}        
```
parameters

1. data: vector<int> or vector<long>, size >=2
2. target: same type as data[0], default = 24

input & output

1. input1: `vector<int> data{23,29,77,83,65}`, output1: 
```plain
[ 29 + [ [ 23 - 83 ] / [ 77 - 65 ] ] ] 
[ 29 - [ [ 83 - 23 ] / [ 77 - 65 ] ] ] 
[ [ [ 23 - 83 ] / [ 77 - 65 ] ] + 29 ] 
```

2. input2: `vector<int> data{3,3,8,8}`, output2: 
```plain
[ 8 / [ 3 - [ 8 / 3 ] ] ] 
```

