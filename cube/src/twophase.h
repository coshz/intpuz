#pragma once
#include <array>
#include <vector>
#include <queue>
#include <tuple>
#include "def.h"
#include "coord.h"
#include "table.h"

class TwoPhaseSolver 
{
public:
    auto solveCube(const CubieCube &cc, size_t max_step=30) 
    -> std::tuple<bool, std::vector<TurnMove>, std::vector<TurnMove>>;

protected:
    enum enum_phase { Ph1=0, Ph2=1 };
    void search_phase1(Coord c, size_t togo);
    void search_phase2(Coord c, size_t togo);

    /* extract solution in phase 1/2 from sofar_ph */
    template<enum_phase I> std::vector<TurnMove> get_solution_ph() const;

    /* move-table based coord transform */
    template<enum_phase I> static Coord transform(Coord c, TurnMove m);

    /* prunning-talbe based infimum distance on phase 1/2 */
    template<enum_phase I> static size_t distance(Coord c);

    /* subgroup G1,G2 indicating the end of phase 1/2 */
    template<enum_phase I> static bool isInSubG(Coord c);

    /* max depth to search in phase 1/2 */
    static constexpr size_t depth_ph1 = 20, depth_ph2 = 15;

    /* elementary moves in two phases */
    static constexpr std::array<TurnMove,18> 
    EM0 = { Ux1,Ux2,Ux3,Rx1,Rx2,Rx3,Fx1,Fx2,Fx3,Dx1,Dx2,Dx3,Lx1,Lx2,Lx3,Bx1,Bx2,Bx3 };
    static constexpr std::array<TurnMove,10> 
    EM1 = { Ux1,Ux2,Ux3,Rx2,Fx2,Dx1,Dx2,Dx3,Lx2,Bx2 };

private:
    /* state data shared for search */
    
    CubieCube cube_to_solve;
    bool exit_phase1, exit_phase2;
    size_t max_length, curr_length;
    std::array<int,depth_ph1+1> sofar_ph1;
    std::array<int,depth_ph2+1> sofar_ph2;
    std::array<std::vector<TurnMove>,2> solution;
};