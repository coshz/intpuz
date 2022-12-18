#include <set>

template<class T>
Rational<T>::Rational(const T& p, const T& q)
{
    if(q == 0) {
        this->p = 0, this->q = 0;
    } else {
        auto k = gcd(p,q) * ((q > 0) ? 1 : -1);
        this->p = p/k, this->q = q/k;
    }
}

template<class T>
T Rational<T>::gcd(T x, T y)
{
    if (x == 0 && y == 0) return 1;
    else if (x * y == 0) return (x) ? x : y;
    else
    {
        // let x >= y > 0
        x = (x<0)?-x:x, y = (y<0)?-y:y;
        if (x < y){auto t = x; x = y; y = t;}

        if (x%y == 0)
            return y;
        else
            return gcd(y, x % y);
    }
}

template<typename T>
Rational<T> ratmul(const Rational<T> &lhs, const Rational<T> &rhs, const Operator &op)
{
    T p1 = lhs.p, q1 = lhs.q;
    T p2 = rhs.p, q2 = rhs.q;
    T p, q;
    switch(op)
    {
        case ADD: { p = p1*q2 + p2*q1, q = q1*q2; break; }
        case SUB: { p = p1*q2 - p2*q1, q = q1*q2; break; }
        case MUL: { p = p1*p2, q = q1*q2; break; }
        case DIV: { p = p1*q2, q = q1*p2; break; }
        case INS: { p = p2*q1 - p1*q2, q = q1*q2; break; }
        case IND: { p = q1*p2, q = p1*q2; break; }
        default: std::cerr << "Rational: invalid operator (" << int(op) << ")!\n"; exit(1);
    }
    return Rational<T>{p,q}; 
}

template<class T>
bool operator==(const Rational<T> &lhs, const Rational<T> &rhs)
{
    return lhs.p == rhs.p && lhs.q == rhs.q;
}

template<class T>
bool operator<(const Rational<T> &lhs, const Rational<T> &rhs)
{
    return lhs.p * rhs.q < rhs.p * lhs.q;
}

compTree::compTree(shared_ptr<compTree> lt_,shared_ptr<compTree> rt_)
:lt{lt_},rt{rt_}
{}

size_t compTree::size() const
{
    size_t n = 1;
    if(lt) n += lt->size();
    if(rt) n += rt->size();
    return n;
}

template<typename value_type, typename operator_type>
Expr<value_type,operator_type>::Expr(value_type val_)
:lexpr{nullptr},rexpr{nullptr},val{val_},op{NaO}
{}

template<typename value_type,typename operator_type>
Expr<value_type,operator_type>::
Expr(const shared_ptr<compTree> ct, const vector<operator_type> &ops, const vector<value_type> &vs)
:Expr(ct,ops.cbegin(),vs.cbegin())
{}

template<typename value_type,typename operator_type>
Expr<value_type,operator_type>::
Expr(const shared_ptr<compTree> ct, typename vector<operator_type>::const_iterator it_op,
    typename vector<value_type>::const_iterator it_val)
{
    if(!ct){
        val = *it_val;
        op = NaO;
    } else {
        auto ln = (ct->lt) ? ct->lt->size() : 0;
        val = NaN;
        op = *it_op;
        lexpr = std::make_unique<Expr>(ct->lt,it_op+1,it_val);
        rexpr = std::make_unique<Expr>(ct->rt,it_op+1+ln,it_val+ln+1);
    }
}

template<typename value_type,typename operator_type>
value_type Expr<value_type,operator_type>::
evaluate() const
{
    if(is_atomic()){
        return val;
    } else {
        auto lv = lexpr->evaluate();
        auto rv = rexpr->evaluate();
        return ratmul(lv, rv, op);
    }
}

template<typename value_type,typename operator_type>
bool Expr<value_type,operator_type>::is_atomic() const 
{
    return op == NaO;
}

template<typename value_type, typename operator_type>
string Expr<value_type,operator_type>::to_str() const
{
    if(!is_atomic()) {
        auto ls = lexpr->to_str();
        auto rs = rexpr->to_str();

        switch(op){
            case ADD:
            case SUB:
            case MUL:
            case DIV: return string() + "[ " + ls + " " + OPS_CHAR[op] + " "+ rs + " ]";
            case INS:
            case IND: return string() + "[ " + rs + " " + OPS_CHAR[-op] + " " + ls + " ]";
            default:  return string();
        }
    } else {
        return std::to_string(val.p); // val.q == 1
    }
}


template<size_t size>
vector<vector<size_t>> Model<size>::
obtain_idxs(size_t n, size_t m, bool rep)
{
    vector<vector<size_t>> all;

    if (!rep && n > m) std::cerr<<"obtain_idxs: invalid arguments.\n", exit(-1);

    if(n <= 0) {}
    else if(n == 1) 
    {
        for (size_t x = 0; x < m; x++)
            all.push_back(vector<size_t>{x});
    } 
    else 
    {
        vector<size_t> idx(m);
        for(size_t i=0; i < m; i++) idx[i] = i;

        auto sub_all = obtain_idxs(n - 1, m, rep);
        vector<size_t> choosing{idx};

        for(auto &sub: sub_all)
        {
            if(!rep)
            {
                choosing.clear();
                auto tmp{sub};
                std::sort(tmp.begin(),tmp.end());
                std::set_difference(idx.begin(), idx.end(), tmp.begin(), tmp.end(),
                    std::inserter(choosing, choosing.begin()));
            }

            for(auto &v :choosing)
            {
                auto perm{sub};
                perm.push_back(v);
                // perm.insert(perm.end(),v);
                all.push_back(std::move(perm));
            }
        }
    }
    return all;  
}

template<size_t size>
vector<vector<typename Model<size>::operator_t>> Model<size>::
find_op_groups(const vector<operator_t> &ops_seed)
{
    auto idx_size = size - 1;
    vector<vector<operator_t>> all{};
    auto idxs = obtain_idxs(idx_size,ops_seed.size(),true);

    for (auto &idx: idxs)
    {
        vector<operator_t> p(idx_size);
        for(auto i = 0; i < idx_size; i++)
            p[i] = ops_seed[idx[i]];
        all.push_back(std::move(p));
    }

    return all;
}

template<size_t size>
vector<shared_ptr<compTree>> Model<size>::
find_ct_groups()
{
    vector<vector<shared_ptr<compTree>>>ctg(size);
    for(auto i = 0; i < size; i++) switch(i)
    {
        case 0: ctg[0].emplace_back(nullptr); continue;
        case 1: ctg[1].emplace_back(std::make_shared<compTree>()); continue;
        default: 
            for(size_t lsize = 0, rsize = i - 1; lsize <= i-1; lsize++, rsize--)
            for(auto lt: ctg[lsize]) for(auto rt: ctg[rsize])
            {
                ctg[i].emplace_back(std::make_shared<compTree>(lt,rt));
            }
    }
    
    return ctg[size-1];
}

template<size_t size>
vector<typename Model<size>::expr_t> Model<size>::
solve(const vector<value_t>& v_seed, value_t tgt, bool greedy) const
{
    vector<expr_t> exprs;
    auto vs_idxs = obtain_idxs(v_seed.size(),v_seed.size(),false);
    if(!greedy) {
        for(auto ct: cts) for(auto &op: ops) for(auto &v_idx: vs_idxs)
        {
            auto vs_tmp = vector<value_t>(v_seed.size());
            for(auto i = 0; i < vs_tmp.size(); i++)
                vs_tmp[i] = v_seed[v_idx[i]];
            auto expr = expr_t{ct,op,vs_tmp};
            if(expr.evaluate() == tgt)
            {
                exprs.push_back(std::move(expr));
                goto ok;
            } 
        }
    } else {
        vector<vector<value_t>> vs(vs_idxs.size());
        for(auto i = 0; i < vs_idxs.size(); i++)
        {
            auto vs_tmp = vector<value_t>(v_seed.size());
            for(auto j = 0; j < vs_tmp.size(); j++)
                vs_tmp[j] = v_seed[vs_idxs[i][j]];
            vs[i] = std::move(vs_tmp);
        }
        
        std::set<vector<value_t>> s(vs.begin(),vs.end());
        vs.assign(s.begin(),s.end());

        for(auto ct: cts) for(auto &op: ops) for(auto &vs_tmp: vs)
        {
            auto expr = expr_t{ct,op,vs_tmp};
            if(expr.evaluate() == tgt)
            {
                exprs.push_back(std::move(expr));
            } 
        }
    }
    ok: ;
    return exprs;
}

template<size_t size>
template<typename T>
vector<typename Model<size>::expr_t> Model<size>::
solve(const vector<T>& vs, T tgt, bool greedy) const
{
    vector<Rational<T>> vs_new(vs.size());
    for(auto i = 0; i < vs.size(); i++)
    {
        vs_new[i] = Rational<T>(vs[i]);
    }
    auto tgt_new = Rational<T>{tgt};
    return solve(vs_new,tgt_new,greedy);
}