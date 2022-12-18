#ifndef __24POINT_H__
#define __24POINT_H__

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using std::vector;
using std::string;
using std::shared_ptr;


// INS: -SUB; IND: 1/DIV
enum Operator{ NOP=0, ADD=1, SUB=2, MUL=3, DIV=4, INS=-SUB, IND=-DIV };

const vector<string> OPS_CHAR = { "<>", "+", "-", "*", "/" };

template<class T> struct Rational;
struct compTree;
template<typename value_type, typename operator_type> class Expr;
template<size_t size> class Model;


template<class T=int>
struct Rational
{
    T p, q;
    Rational(const T& p=1, const T& q=1);

    /* Non-negative greatest common devisor. */
    static T gcd(T x, T y);
};

template<typename T>
Rational<T> ratmul(const Rational<T> &lhs, const Rational<T> &rhs, const Operator &op);

template<class T=int>
bool operator==(const Rational<T> &lhs, const Rational<T> &rhs);

template<class T=int>
bool operator<(const Rational<T> &lhs, const Rational<T> &rhs);

/* A tree to simulate the (non-empty) partition of operands.
* 1) smallness: the smallest tree, lt == rt == nullptr, has size 1;
* 2) equivalence: A ~ B iff A.lt ~ B.lt && A.rt ~ B.rt; */
struct compTree
{
    shared_ptr<compTree> lt, rt;
    compTree(shared_ptr<compTree> lt_=nullptr,shared_ptr<compTree> rt_=nullptr);

    size_t size() const;

    bool is_terminal() const { return !lt && !rt; }
};


/*
* Expr built using `compTree` as infix operator tree
* 1) atomicity: lexpr = rexpr = nullptr; 
* 2) value: atomic -> val; non-atomic -> op(lexpr,rexpr). */
template<typename value_type=Rational<int>, typename operator_type=Operator>
class Expr
{
public:
    Expr(value_type val_);
    Expr(const shared_ptr<compTree> ct, const vector<operator_type> &ops, 
         const vector<value_type> &vs);
    
    value_type evaluate() const;
    string to_str() const;
    bool is_atomic() const;

    template<size_t size> friend class Model;
    // template<size_t size> 
    // friend vector<shared_ptr<compTree>> Model<size>::find_ct_groups();

protected:
    static const value_type NaN; // default non-initialized val to signify not evaluated expr;
    static const operator_type NaO; // default non-initialized op to signify terminal expr;

private:
    value_type val;
    operator_type op;
    std::unique_ptr<Expr> lexpr, rexpr;

public:
    Expr(const shared_ptr<compTree> ct, typename vector<operator_type>::const_iterator it_op,
         typename vector<value_type>::const_iterator it_val);
};

template<typename value_type, typename operator_type>
const value_type Expr<value_type,operator_type>::NaN{0,0};

template<typename value_type, typename operator_type>
const operator_type Expr<value_type,operator_type>::NaO{NOP};


template<size_t size=4>
class Model
{
    using value_t = Rational<int>;
    using operator_t = Operator;
    using expr_t = Expr<value_t, operator_t>;  
public:
    template<typename T=int>
    vector<expr_t> solve(const vector<T> & vs, T tgt=24, bool greedy=false) const;
    vector<expr_t> solve(const vector<value_t>& vs, value_t tgt, bool greedy) const;
   
    // all idx vectors of size n taking values (*rep*lication or not) in range [0,m)
    static vector<vector<size_t>> obtain_idxs(size_t n, size_t m, bool rep);

    // for debug/test
    static size_t cts_size() { return cts.size(); }
    static size_t ops_size() { return ops.size(); }
private:
    static vector<shared_ptr<compTree>> find_ct_groups();
    static vector<vector<operator_t>> find_op_groups(const vector<operator_t> &ops);

    static const vector<shared_ptr<compTree>> cts;
    static const vector<vector<operator_t>> ops;
};

template<size_t size>
const vector<shared_ptr<compTree>> Model<size>::cts = find_ct_groups();

template<size_t size>
const vector<vector<typename Model<size>::operator_t>> Model<size>::ops = find_op_groups(
    vector<Operator>{ ADD, SUB, MUL, DIV}
);

#include "24points.inl"
#endif //__24POINT_H__