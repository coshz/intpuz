#ifndef __DEF_H__
#define __DEF_H__

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <type_traits>

#include <iostream>

/* tetrimino types */
enum Shape { O=0, I=1, T=2, L=3, J=4, S=5, Z=6, Empty };

/* transform types */
enum Tran { Tran_L, Tran_R, Tran_C, Tran_AC, NUM_TRAN };

/* drop types */
enum Drop { Drop_N, Drop_S, Drop_H, NUM_DROP };

/* pos type */
struct Pos { 
    int x,y; 
    int&        operator[](size_t i)        { return (0==i%2) ? x : y; }
    const int&  operator[](size_t i) const  { return (0==i%2) ? x : y; }
    Pos         operator-(Pos p) const      { return Pos{ x-p.x, y-p.y }; }
}; 

/* actions */
enum Action { 
    /* elementary actions */
    Single, Double, Triple, Tetris, 
    Mini_TSpin, Mini_TSpin_Single, Mini_TSpin_Double,
    TSpin, TSpin_Single, TSpin_Double, TSpin_Triple,
    /* drop actions */
    Drop_Hard, Drop_Soft,
    /* no action */
    NoAction,
    NUM_ACTION,
    /* pesudo actions */
    Combo, 
    Bravo_Single, Bravo_Double, Bravo_Triple, Bravo_Tetris, Bravo_B2B_Tetris,
    NUM_EXTENDED_ACTION
};

static constexpr uint64_t scoring_coef[NUM_EXTENDED_ACTION] = {
    /* actions: level x */
    100,300,500,800, 
    100,200,400,
    400,800,1200,1600,
    /* drop: cell x */
    2,1,
    /* no action*/
    0,
    /* placeholder */
    0,
    /* combo: combo_count x level x */
    50,
    /* bravo: level x */
    800,1200,1800,2000,3200,
};

/* store if an action is difficult */
static constexpr bool action_diff[NUM_ACTION] = {
    0,0,0,1,
    0,1,1,
    0,1,1,1,
    0
};

#ifndef DEFINE_SELF_AND_CSELF_FOR
#define DEFINE_SELF_AND_CSELF_FOR(Derived) \
        auto self()         { return static_cast<Derived*>(this); }\
        auto cself() const  { return static_cast<const Derived*>(this); }
/// we use `cself` instead of cv- overloading `self` to provide flexibility,
/// albeit at the expense of naming simplicity.
#else
    error("macro `DEFINE_SELF_AND_CSELF_FOR` is occupied")
#endif

template <typename Derived>
struct StateTemplate
{
    DEFINE_SELF_AND_CSELF_FOR(Derived)
    
    int     get_score() const           { return cself()->get_score(); }
    int     get_level() const           { return cself()->get_level(); }
    int     get_count_of(Action a) const{ return cself()->get_count_of(a); }
};

template <typename Derived>
struct TetrominoTemplate
{ 
    DEFINE_SELF_AND_CSELF_FOR(Derived)

    Shape   get_shape() const           { return cself()->get_shape(); }
    int     get_orient() const          { return cself()->get_orient(); }
    Pos     get_base() const            { return cself()->get_base(); }

    // coord's return type is an iterable, ranged type over Pos
    auto    coord() const               { return cself()->coord(); }
    Pos     operator[](size_t i) const  { return coord()[i]; }    
};

template <typename Derived>
struct TetrisModelTemplate
{
    DEFINE_SELF_AND_CSELF_FOR(Derived)

    Shape       get_shape_at(int x,int y) const { return cself()->get_shape_at(x,y); }
    auto const* get_tetro() const               { return cself()->get_tetro(); }
    auto const* get_state() const               { return cself()->get_state(); }
    auto const* tetro_from_next(int i) const    { return cself()->tetro_from_next(i); }
    auto const* tetro_from_hold() const         { return cself()->tetro_from_hold(); }

    // return the ghost tetro
    auto        glance_ghost() const            { return cself()->glance_ghost(); }

    // :transform tetro:
    // @return `false` - do noting
    bool        transform(Tran t)               { return self()->transform(t); }

    // :dropdown tetro:
    // @return the displacement of tetro
    int         dropdown(Drop d)                { return self()->dropdown(d); }

    // :lockdown tetro:
    // @return  `true` - renew tetro and go ahead;
    //          `false` - game should be terminated;
    bool        lockdown()                      { return self()->lockdown(); }

    // :hold tetro:
    void        hold()                          { return self()->hold(); }

    // renew tetro
    void        renew()                         { return self()->renew(); }

    // reset the whole model
    void        reset()                         { return self()->reset(); }

    constexpr size_t size_x() const { return cself()->size_x(); }
    constexpr size_t size_y() const { return cself()->size_y(); }
};

#undef DEFINE_SELF_AND_CSELF_FOR

/// 
/// ! Note on XY coordinate system
/// the XY coordinate system follows standard convention, where the x-axis 
/// increases to the right and the y-axix increase to the top
/// 

/* [shapes] / [orient] / [component] / [pos] is the relative pos of:
   the tetromino's base
*/
static constexpr int raw_pos[7][4][4][2] = {
    {
        {{0,0},{1,0},{0,1},{1,1}}, 
        {{0,0},{1,0},{0,1},{1,1}}, 
        {{0,0},{1,0},{0,1},{1,1}}, 
        {{0,0},{1,0},{0,1},{1,1}}
    },
    {
        {{-1,0},{0,0},{1,0},{2,0}}, 
        {{1,1},{1,0},{1,-1},{1,-2}},
        {{2,-1},{1,-1},{0,-1},{-1,-1}}, 
        {{0,-2},{0,-1},{0,0},{0,1}} 
    },
    { 
        {{0,0},{-1,0},{1,0},{0,1}}, 
        {{0,0},{0,1},{0,-1},{1,0}}, 
        {{0,0},{1,0},{-1,0},{0,-1}},
        {{0,0},{0,-1},{0,1},{-1,0}}
    },
    {
        {{0,0},{-1,0},{1,0},{1,1}},
        {{0,0},{0,1},{0,-1},{1,-1}},
        {{0,0},{1,0},{-1,0},{-1,-1}},
        {{0,0},{0,-1},{0,1},{-1,1}}
    },
    {
        {{0,0},{-1,0},{1,0},{-1,1}}, 
        {{0,0},{0,1},{0,-1},{1,1}}, 
        {{0,0},{1,0},{-1,0},{1,-1}}, 
        {{0,0},{0,-1},{0,1},{-1,-1}}
    },
    {
        {{0,0},{-1,0},{0,1},{1,1}},
        {{0,0},{0,1},{1,0},{1,-1}},
        {{0,0},{1,0},{0,-1},{-1,-1}},
        {{0,0},{0,-1},{-1,0},{-1,1}}
    },
    {
        {{0,0},{-1,1},{0,1},{1,0}},
        {{0,0},{1,1},{1,0},{0,-1}},
        {{0,0},{1,-1},{0,-1},{-1,0}},
        {{0,0},{-1,-1},{-1,0},{0,1}}
    }
};

/* [shape-type] / [orient-map] / [disp-variant]/ [disp] is:
   kinds [5] of x-y displacement [8] of tetromino used in walltick, wrt. 
   shape types [2]: type-0: T, L, J, S, Z; type-1: I; and orient map [8] derived 
   from rotation CW/CCW;
*/
static constexpr int raw_disp[2][8][5][2] = {
    {
        {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},   // 0 -> 1
        {{0,0},{1,0},{1,1},{0,-2},{1,-2}},      // 0 -> 3
        {{0,0},{1,0},{1,-1},{0,2},{1,2}},       // 1 -> 2
        {{0,0},{1,0},{1,-1},{0,2},{1,2}},       // 1 -> 0
        {{0,0},{1,0},{1,1},{0,-2},{1,-2}},      // 2 -> 3
        {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},   // 2 -> 1
        {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},    // 3 -> 0
        {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}}     // 3 -> 2
    },
    {
        {{0,0},{-2,0},{1,0},{-2,-1},{1,2}},     // 0 -> 1
        {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},     // 0 -> 3
        {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},     // 1 -> 2
        {{0,0},{2,0},{-1,0},{2,1},{-1,-2}},     // 1 -> 0
        {{0,0},{2,0},{-1,0},{2,1},{-1,-2}},     // 2 -> 3
        {{0,0},{1,0},{-2,0},{1,-2},{-2,1}},     // 2 -> 1
        {{0,0},{1,0},{-2,0},{1,-2},{-2,1}},     // 3 -> 0
        {{0,0},{-2,0},{1,0},{-2,-1},{1,2}}      // 3 -> 2
    }
}; 

#endif // __DEF_H__