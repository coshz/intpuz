About

Usage
```c++
vector<int> data{23,29,77,83,65};
auto model = Model<5>();
auto exprs = model.solve(data, 24);
for(auto &expr:exprs) cout << expr.to_str() << endl;
```


