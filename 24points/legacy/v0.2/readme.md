About

(I) latest

:v0.2 

update
-  using fixed-sized template to improve performance;
- `model.solve(data_vec,target=24,greedy=false)`;

usage
```c++
vector<int> data{23,29,77,83,65};
auto model = Model<5>();
auto exprs = model.solve(data,24);
for(auto &expr:exprs) cout << expr.to_str() << endl;
```

(II) legacy

- [v0.1](./legacy/v0.1)

