#include <queue>
#include "twophase.h"

const TableMove<>       &TM = SingletonTM<>::getInstance().getTable();
const TablePrunning<>   &TP = SingletonTP<>::getInstance().getTable();

bool isValidPair(TurnMove m1, TurnMove m2)
{
    return m1/3 != m2/3;
}

/*!
 * @note The two map: (edge4, m) -> edge4 and (edge8, m) -> edge8, are
 * well-defined if slice is fixed, which is not hold in phase 1. 
 * Therefore, we need other methods (e.g. CubieCube * ElementaryMove) 
 * to compute the correct edge4 and edge8 at the beginning of phase 2.  
 */
template<TwoPhaseSolver::enum_phase I> 
Coord TwoPhaseSolver::transform(Coord c, TurnMove m)
{
    if constexpr (I == Ph1)
    return Coord {
        (*TM.pTMTwist)[m][c.twist], (*TM.pTMFlip)[m][c.flip], (*TM.pTMSlice)[m][c.slice],
        -1,-1,-1
    };  
    else 
    return Coord {
        0,0,0,
        (*TM.pTMCorner)[m][c.corner], (*TM.pTMEdge4)[m][c.edge4], (*TM.pTMEdge8)[m][c.edge8]
    };
}

template<TwoPhaseSolver::enum_phase I>
size_t TwoPhaseSolver::distance(Coord c)
{
    if constexpr (I == Ph1)
    return std::max((*TP.pTPSliceTwist)[c.slice][c.twist], 
                    (*TP.pTPSliceFlip)[c.slice][c.flip]);
    else 
    return std::max((*TP.pTPEdge4Corner)[c.edge4][c.corner], 
                    (*TP.pTPEdge4Edge8)[c.edge4][c.edge8]);
}

template<TwoPhaseSolver::enum_phase I>
bool TwoPhaseSolver::isInSubG(Coord c)
{
    if constexpr (I == Ph1) 
        return c.twist == 0 && c.flip == 0 && c.slice == 0;
    else 
        return c == Coord::id;
}

template<TwoPhaseSolver::enum_phase I>
auto TwoPhaseSolver::get_solution_ph() const -> std::vector<TurnMove> 
{
    std::vector<int> sofar;
    size_t len = 0;

    if constexpr (I == Ph1) { sofar = std::vector<int>(sofar_ph1.begin(),sofar_ph1.end()); } 
    else { sofar = std::vector<int>(sofar_ph2.begin(),sofar_ph2.end()); }
    while(sofar[len] >= 0) len++;
    std::vector<TurnMove> solution(len);
    for(size_t i = 0; i < len; i++) { solution[i] = (TurnMove) sofar[len - 1 - i]; }

    return solution;
}

auto TwoPhaseSolver::solveCube(const CubieCube &cc, size_t max_step)
-> std::tuple<bool, std::vector<TurnMove>, std::vector<TurnMove>>
{
    exit_phase1 = exit_phase2 = false;
    max_length = max_step;
    curr_length = 10000;
    solution = {};
    cube_to_solve = cc; 
    
    auto c = Coord::CubieCube2Coord(cc);
    for(auto i = distance<Ph1>(c); i <= depth_ph1 && !exit_phase1; i++)
    {
        exit_phase2 = false;                        
        sofar_ph1.fill(-1), sofar_ph2.fill(-1);     // reset sofar
        search_phase1(c, i);
    }

    bool found = (curr_length == 10000) ? false : true;
    return std::make_tuple(found, solution[0], solution[1]);
}

void TwoPhaseSolver::search_phase1(Coord c1, size_t togo)
{
    if(togo == 0) {
        assert(isInSubG<Ph1>(c1));
        auto s1 = this->get_solution_ph<Ph1>();
        auto c2 = [](CubieCube cc, std::vector<TurnMove> s){
            for(auto &m: s) cc = cc * ElementaryMove[m];
            return Coord::CubieCube2Coord(cc);
        }(cube_to_solve, s1);

        assert(isInSubG<Ph1>(c2));
        for(auto i = distance<Ph2>(c2); 
                 i < std::min(depth_ph2, curr_length-s1.size()) && !exit_phase2; i++) 
        {
            search_phase2(c2, i);
        }
    } 
    else
    for(auto m: EM0)
    {
        // skip continuous moves of same type
        if(sofar_ph1[togo] != -1 && !isValidPair(m, (TurnMove) sofar_ph1[togo])) continue;

        auto c1_update = transform<Ph1>(c1, m);      
        auto d = distance<Ph1>(c1_update);
        if(d < togo){
            sofar_ph1[togo-1] = m; 
            search_phase1(c1_update, togo-1);
        }
    }
}

void TwoPhaseSolver::search_phase2(Coord c2, size_t togo)
{
    if(togo == 0) { 
        assert(isInSubG<Ph2>(c2));
        solution[0] = get_solution_ph<Ph1>(), solution[1] = get_solution_ph<Ph2>();
        curr_length = solution[0].size() + solution[1].size();
        
        exit_phase2 = true; 
        exit_phase1 = (curr_length <= max_length) ? true : false;
        return; 
    }
    for(auto m: EM1)
    {
        if(sofar_ph2[togo] != -1 && !isValidPair(m, (TurnMove) sofar_ph2[togo])) continue;
        auto c2_update = transform<Ph2>(c2, m);
        auto d = distance<Ph2>(c2_update);
        if(d < togo) {
            sofar_ph2[togo-1] = m;
            search_phase2(c2_update, togo-1);
        }
    }
}

