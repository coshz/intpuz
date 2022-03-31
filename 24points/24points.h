#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

enum Operator {ADD=0, SUB, MUL, DIV};
const vector<string> OPVEC={"+", "-", "*", "/"};

//T: int, long
template<class T=int>
struct Rational
{
    vector<T> r;
    Rational(T p=1, T q=1)
    {
        auto k = gcd(p,q);
        this->r = (q==0)?vector<T>{0,0}:vector<T>{p/k,q/k};
    }
    T gcd(T x, T y)
    {
        if (x == 0 && y == 0) return 1;
        else if (x * y == 0) return (x)?x:y;
        else
        {
            // let x >= y > 0
            x = (x<0)?-x:x, y = (y<0)?-y:y;
            if (x < y){auto t = x; x = y; y = t;}

            if (x%y == 0)
                return y;
            else
                return gcd(y,x%y);
        }
    }
    bool isnan()const {return this->r[1] == 0;}
};

template<class T=int>
Rational<T> ratmul(Rational<T> &lhs, Rational<T> &rhs, Operator op)
{
    auto p1 = lhs.r[0], q1 = lhs.r[1];
    auto p2 = rhs.r[0], q2 = rhs.r[1];
    T p, q;
    switch(op)
    {
        case ADD: {p = p1*q2 + p2*q1, q = q1*q2; break;}
        case SUB: {p = p1*q2 - p2*q1, q = q1*q2; break;}
        case MUL: {p = p1*p2, q = q1*q2; break;}
        case DIV: {p = p1*q2, q = q1*p2; break;}
    }
    return Rational<T>{p,q}; 
}

template<class T=int>
bool operator==(const Rational<T> &lhs, const Rational<T> &rhs)
{
    return lhs.r == rhs.r;
}

struct binaryTree
{
    /*
    !!! binaryTree MUST be constructed by `new`.
    1) the smallest tree (size 1): bt!=nullptr, bt->ltree == bt->rtree == nullptr;
    2) trees A and B are equivalent if both their left trees and their right trees 
        are equivalent; all smallest trees are equivalent. 
    */
    binaryTree *ltree, *rtree;
    binaryTree():ltree{nullptr},rtree{nullptr}{}
    binaryTree(const binaryTree &t)
    {
        this->ltree = (t.ltree)?new binaryTree(*t.ltree):nullptr;
        this->rtree = (t.rtree)?new binaryTree(*t.rtree):nullptr;
    }
    binaryTree(binaryTree &&t)
    {
        
        this->ltree = std::move(t.ltree);
        this->rtree = std::move(t.rtree);
    }
    binaryTree& operator=(const binaryTree &other)
    {
        if(this != &other)
        {
            auto new_ltree = (other.ltree)?new binaryTree(*other.ltree):nullptr;
            auto new_rtree = (other.rtree)?new binaryTree(*other.rtree):nullptr;

            delete this->ltree, delete this->rtree;

            this->ltree = new_ltree, this->rtree = new_rtree;
        }
        return *this;   
    }
    binaryTree& operator=(binaryTree && other)
    {
        if(this != &other)
        {
            delete this->ltree, delete this->rtree;

            this->ltree = other.ltree, this->rtree = other.rtree;
            other.ltree = nullptr, other.ltree = nullptr;
        }
        return *this;
    }
    ~binaryTree()
    {
        if(ltree)
            delete ltree;
        if(rtree)
            delete rtree;
    }
   
    int size() const
    {
        if(this->ltree == nullptr && this->rtree == nullptr)
            return 1;
        else
        {
            auto lsize = (ltree)?ltree->size():0;
            auto rsize = (rtree)?rtree->size():0;
            return lsize + rsize + 1;
        }
    }
};

bool operator==(const binaryTree &lhs, const binaryTree &rhs) 
{
    return (lhs.size() == rhs.size() && lhs.ltree->size() == rhs.ltree->size() && lhs.rtree->size() == rhs.rtree->size());
}

vector<vector<std::size_t>> permutations(std::size_t n, std::size_t m, bool reduce)
{
    //vector size n, value range 0~m-1;
    //reduce: if ture, remove duplicates;
   
    vector<vector<std::size_t>> all{};
    vector<std::size_t> items(m);
    for(std::size_t i=0;i<items.size();i++) items[i] = i;

    if (reduce && n>m) cerr<<"invalid arguments.\n", exit(1);
    else if(n <= 0) ;
    else if (n==1)
        for (std::size_t x = 0; x<m; x++)
            all.push_back(vector<std::size_t>{x});
    else
    {
        auto subperms = permutations(n-1,m,reduce);
        vector<std::size_t> choosing{items};

        for(auto &tail: subperms)
        {
            if(!reduce) ;
            else
            {
                choosing.clear();
                auto tmp{tail};
                std::sort(tmp.begin(),tmp.end());
                std::set_difference(items.begin(), items.end(), tmp.begin(), tmp.end(),
                    std::inserter(choosing, choosing.begin()));
            }

            for(auto &v :choosing)
            {
                auto perm{tail};
                perm.insert(perm.end(),v);
                all.push_back(std::move(perm));
            }
        }
    }
    return all;  
}

template<class T>
vector<vector<T>> numsAll(const vector<T> &nums, bool reduce=true)
{
    vector<vector<T>> all{};
    auto indicesAll = permutations(nums.size(),nums.size(),true);
    for (auto &indices: indicesAll)
    {
        vector<T> p(nums.size());
        for(int i=0; i<nums.size(); i++)
            p[i] = nums[indices[i]];
        all.push_back(std::move(p));
    }
    
    if (reduce)
    {
        std::sort(all.begin(),all.end());
        all.erase(std::unique(all.begin(),all.end()),all.end());
    }
    return all;

}

vector<vector<Operator>> opsAll(int size, const vector<Operator> &ops, bool reduce = true)
{
    vector<vector<Operator>> all{};
    auto indicesAll = permutations(size,ops.size(),false);
    for (auto &indices: indicesAll)
    {
        vector<Operator> p(size);
        for(int i=0; i<size; i++)
            p[i] = ops[indices[i]];
        all.push_back(std::move(p));
    }
    if(reduce)
    {
        std::sort(all.begin(),all.end());
        all.erase(std::unique(all.begin(),all.end()),all.end());
    }
    return all;
}

vector<binaryTree> binaryTreesAll(int size)
{
    vector<binaryTree> all{};

    if(size <= 0) ;    
    else if(size == 1)
    {
        binaryTree *tinytree = new binaryTree{};
        all.push_back(*tinytree);
        delete tinytree;
    }    
    else for(int ln = 0, rn = size - 1; ln <= size - 1; ln++, rn--)
    {
        auto lsubtreeAll = binaryTreesAll(ln);
        auto rsubtreeAll = binaryTreesAll(rn);
        binaryTree *bt = new binaryTree {};
        if(0 == lsubtreeAll.size())
        {
            for(auto &rt: rsubtreeAll)
            {  
                bt->rtree = &rt;
                all.push_back(*bt);
                bt->rtree = nullptr; 
            }   
        }
        else if(0 == rsubtreeAll.size())
        {
            for(auto &lt: lsubtreeAll)
            {
                bt->ltree = &lt;
                all.push_back(*bt);
                bt->ltree = nullptr;
            }  
        }
        else
        {
            for(auto &lt: lsubtreeAll)
            for(auto &rt: rsubtreeAll)
            {
                bt->ltree = &lt;
                bt->rtree = &rt;
                all.push_back(*bt);
                bt->ltree = nullptr, bt->rtree = nullptr;  
            }
        } 
        delete bt;
    }
    
    return all;
}

template<class T>
struct compModel
{
    vector<T> nums;
    vector<Operator> ops;
    compModel(vector<T> &data):nums{data},ops{ADD,SUB,MUL,DIV}{}
    bool isvalidExpr(Rational<T> lnum, Rational<T> rnum, Operator op)
    {
        return !(lnum.isnan() || rnum.isnan() || (op == DIV && rnum.r[0] == 0));
    }

    tuple<Rational<T>, vector<string>> evaluate(const binaryTree *bt, const vector<T> &vi, const vector<Operator> &vo)
    {
        vector<string> expr{};
        Rational<T> result{0,0};

        if(!bt)
        {
            expr.push_back(std::to_string(vi[0]));
            result = Rational<T>{vi[0]};   
        }
        else
        {    
            Rational<T> lresult, rresult;
            vector<string> lexpr{}, rexpr{};
            int ln = (bt->ltree)?bt->ltree->size():0;
            auto lvi = std::vector<T>(vi.begin(), vi.begin()+ln+1);
            auto rvi = std::vector<T>(vi.begin()+ln+1, vi.end());
            auto lvo = std::vector<Operator>(vo.begin()+1, vo.begin()+ln+1);
            auto rvo = std::vector<Operator>(vo.begin()+ln+1, vo.end());

            std::tie(lresult, lexpr) = this->evaluate(bt->ltree,lvi,lvo);
            std::tie(rresult, rexpr) = this->evaluate(bt->rtree,rvi,rvo);
            
            bool valid = this->isvalidExpr(lresult, rresult, vo[0]);
            if(valid)
            {
                result = ratmul(lresult, rresult, vo[0]);
                expr.push_back(OPVEC[vo[0]]);
                expr.insert(expr.begin(), lexpr.begin(), lexpr.end());
                expr.insert(expr.end(), rexpr.begin(), rexpr.end());
                expr.insert(expr.begin(), "[");
                expr.insert(expr.end(), "]");
            }
        }
        return make_tuple(result, expr);
    }

    vector<vector<string>> solve(T target=24, bool greedy=true)
    {
        //greedy == true, find all solutions.
        //[TODO] remove equivalent (commutativity of ops) solutions 
        auto nums_vec = numsAll(this->nums);
        auto ops_vec = opsAll(this->nums.size()-1, this->ops);
        auto tree_vec = binaryTreesAll(this->nums.size()-1);

        vector<vector<string>> exprs;

        for(auto &ct: tree_vec)
        for(auto &ops: ops_vec)
        for(auto &nums: nums_vec)
        {          
            Rational <T> res;
            vector<string> expr;

            std::tie(res,expr) = evaluate(&ct, nums, ops);
            if(res == Rational<T>{target})
            {
                exprs.push_back(expr);
                if(!greedy)
                    goto end;
            }        
        }
        end: ; // haha
        return exprs;
    }
};