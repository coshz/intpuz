#include <set>
#include "cube.h"
#include "solver.h"
#include <cstdio>

TwoPhaseSolver TPS;

std::string move_str[18] = 
{ "U","U2","U'","R","R2","R'","F","F2","F'","D","D2","D'","L","L2","L'","B","B2","B'" };

std::string CubeSolver::solve(std::string src, std::string tgt, int max_length)
{
    auto cc_src = CubieCube::fromString(src);
    auto cc_tgt = (tgt == "" || tgt == cid) ? CubieCube::id : CubieCube::fromString(tgt);
    CubieCube cc = ~cc_tgt*cc_src;

    if(!cc.isSolvable()) return "unsolvable.";
    
    const auto & [found, s1, s2] = TPS.solveCube(cc, max_length);
    if(!found) return "solution not existing since the search depth is too small.";
    std::string r = "";
    r += "[ ";
    for(auto &m: s1) r =  r + move_str[m] + " ";
    r += ";;";
    for(auto &m: s2) r = r + " " + move_str[m];
    r += " ]";
    return r;
}


