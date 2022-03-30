example

```c++
    // vector<int> data{3,8,3,8};
    vector<int> data{23,29,77,83,65};
    
    compModel<int> model{data};
    auto sts= model.solve();

    if(sts.size() == 0)
        cout<<"No solution.\n";
    else 
    for(auto &st: sts) {
        for(auto &s:st)
            cout<<s<<' ';
        cout<<endl;}        
```

input1: `vector<int> data{23,29,77,83,65}`
output1: 
```plain
[ 29 + [ [ 23 - 83 ] / [ 77 - 65 ] ] ] 
[ 29 - [ [ 83 - 23 ] / [ 77 - 65 ] ] ] 
[ [ [ 23 - 83 ] / [ 77 - 65 ] ] + 29 ] 
```

input2: `vector<int> data{3,3,8,8}`
output2: 
```plain
[ 8 / [ 3 - [ 8 / 3 ] ] ] 
```

