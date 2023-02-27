#ifndef __MODEL_H__
#define __MODEL_H__

#include <sstream>
#include <string>
#include <deque>
#include <array>
#include <vector>
#include <set>

namespace TetrisModel
{
    
/* tetrimino type */
enum Shape { O=0, I=1, T=2, L=3, J=4, S=5, Z=6, Empty };

enum GameVariant { Marathon, Sprint, Ultra };

/* action used for line award & score award */
enum Action { 
    Single, Double, Triple, Tetris, 
    Mini_TSpin, Mini_TSpin_Single, 
    TSpin, TSpin_Single, TSpin_Double, TSpin_Triple,
    Nothing
};

/* drop type for speed & drop award */
using Drop = enum { Normal, Soft, Hard };
using Score = int;
using Pos = std::array<int,2>;
using Coord = std::array<Pos,4>;

template<int W, int H>class Matrix;
class Tetromino;
struct State;
template<class matrix_t, class tetromino_t, class state_t> class Model;
template<int t>struct RandomSystem;
template<int t>struct GoalSystem;
struct TimeSystem;

/* tetromino class */
class Tetromino
{
public:
    Tetromino(Shape shape=Empty, int orient=0, Pos base={});
    Tetromino(const Tetromino &) = default;

    const Shape& getShape() const   { return shape_; }
    const Pos& getBase() const      { return base_; }
    const int& getOrient() const    { return orient_; }

    /* update teromino by base/orient change */
    bool update(const Pos &db, const int &dr=0); 

    /* all four components' pos in Matrix */
    Coord coord(int dx = 0, int dy = 0, int dr = 0) const;
    static Coord coord(Shape shape, int orient, Pos base);

    static const int (&getPos(Shape shape, int orient))[4][2] { 
        return raw_pos[shape][orient]; 
    }
    static const int (&getDisp(Shape shape, int orient, bool cw))[5][2] {
        return raw_disp[shape == I][2 * orient + (cw ? 0 : 1)];
    }

protected:

private:
    Shape shape_; // shape type 
    int orient_;  // value 0, 1, 2, 3 representing 90 * value degrees clockwise
    Pos base_;    // x-y coordinates of tetromino's origin in Matrix

    /* [shapes] / [orient] / [component] / [pos] storing relative pos to base of tetromino */
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

    /* kinds [5] of x-y displacement [2] of tetromino, used in walltick, with regard to: 
       1) shape types [2]: type-I: T, L, J, S, Z; type-II: I
       2) orient and rotation direction [8] */
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
};

/* matrix serving as coordinate system for tetromino */
template<int W=10, int H=20>
class Matrix
{
public:
    Matrix();
    const Shape& getShapeAt(int x, int y) const { return data_[x][y]; }
    void setShapeAt(Shape shape, int x, int y)  { data_[x][y] = shape; }
    bool isEmpty(int x, int y) const            { return data_[x][y] == Empty; }
    static constexpr int width = W, height = H;
private:  
    constexpr static int hide_H = 4;
    Shape data_[W][H+hide_H];
};

/* Model connecting Matrix, Tetro and score-system, etc. by virtue of State */
template<class matrix_t=Matrix<10,20>,class tetromino_t=Tetromino,class state_t=State>
class Model
{
    enum Trans{ tran_L = 0, tran_R, tran_CW, tran_ACW };
public:
    Model();
    void reset();

    /*===================================================================*|
     * Motion Control 
     *-------------------------------------------------------------------*/

    /* tetromino transformation: 0, 1, 2, 3 for left, right, clockwise, 
       anticlockwise, return true if success */
    bool transform(int tran_type);

    /* drop tetromino in matrix: 0, 1, 2 for normal-drop, soft-drop, hard-drop.
       return value 0 for drop not take effect */
    void drop(int drop_type=0);

    /*===================================================================*|
     * Phase control
     *-------------------------------------------------------------------*/

    /* generate from hold queue */
    void generate(Shape shape = Empty);

    /* settle up scores, lines, level, etc. */
    template<typename goal_system_t=GoalSystem<1>> 
    void settle(const goal_system_t &gs);

    void hold();

    /* set next queue */
    template<typename random_system_t=RandomSystem<0>> 
    void pushNext(const random_system_t &rs);

    /*===================================================================*|
     * Attributes
     *-------------------------------------------------------------------*/

    const tetromino_t& getTetro() const { return tetro_; }
    const matrix_t& getMatrix() const   { return matrix_; }
    const state_t& getState() const     { return state_; }
    /* endgame check */
    bool isover() const { return end; }

     /* lockdown check */
    bool lockable() const;
   
    /*===================================================================*|
     * Auxiliary
     *-------------------------------------------------------------------*/

     /* ghost, i.e. the most lowest droppable coord in matrix */
    Coord ghost() const;

    /* default base for different shape */
    static Pos default_base(const Shape &shape=Empty);

protected:
    /* tetromino is translatable or not considering obstacle in Matrix
       where argument (-1,0), (1,0), (0,-1) for left, right, drop  */
    bool translatable(int dx, int dy) const;
    /* tetromino is rotatable or not with walltick considering obstacle in Matrix */
    bool rotatable(bool cw, const int disp[2]) const;
    /* coord is permissible, ie. coord can be contained in matrix, or not */
    bool permissible(const Coord &co) const;

    /* check T-spin, 0, 1, 2 for nothing, mini-tspin, tspin */
    static int checkTspin(const matrix_t &matrix, const tetromino_t &tetro);
    /* check line id of matrix to clear if tetromino locks */
    static std::vector<int> checkClear(const matrix_t &matrix, const tetromino_t &tetro);

    /* lockdown tetromino and update matrix */
    Action lockdown();

private:
    matrix_t matrix_;                   
    tetromino_t tetro_; 
    state_t state_;
    bool end;    
};  

/* necessary state information used for goal-score system ans so on */
struct State
{
    /* hold / next queue */
    State(int _level = 1, int _size_next = 7)
    :level(_level),size_next(_size_next),back2back(false),holdable(true),
    hold(Empty),next(Empty),action{Nothing},
    cnt_drop{},cnt_action{},score(0),line(0), rotate_idx(0)
    {}

    template<typename random_system_t>
    void initNextQueue(const random_system_t &rs) {
        for(auto i = 0; i < size_next; i++) next_queue.push_back(rs.next());
        next = next_queue.front();
        next_queue.pop_front();
    }
    bool isFullNext() const { return size_next == 1 + next_queue.size(); }

    Shape hold; 
    Shape next;
    std::deque<Shape> next_queue;
    int size_next;
    
    /* maintained (update when do statistics) during the whole game */

    Score score;            
    int level;           
    int line; 

    int cnt_action[10];     // count of actions 
    int cnt_drop[3];        // count of lines of normal/hard/soft drop       

    /* maintained for the tetrimino in play */

    Action action;          // match result
    int rotate_idx;         // rotation point to check T-spin
    bool back2back;         // back-to-back bonus flag
    bool holdable;          // holdable flag
};

/* random system: 0, 1 for N-7-bag, random */
template<int rs_type=0> 
struct RandomSystem
{
    Shape next() const;
    template<typename T, int N=7> static void reset(T (&arr)[N]);
};

/* goal system: type = 0, 1 for fixed, variable goal. */
template<int gs_type=0>
struct GoalSystem
{
    static int lineAction(const Action &action, bool b2b);
    static int scoreAction(const Action &action, bool b2b);
    static int scoreDrop(int line, Drop drop);
    static bool isLevelUp(int level, int line);
};

/* time system */
struct TimeSystem
{
    /* duration between completion and next generation; unit=milliseconod */
    const int generate_delay;
    /* duration of single R/L translation turning into repetitive translation */
    const int autorept_delay;
    /* duration between visual lockdown and actual lockdown */
    const int lockdown_delay;
    /* drop speed: unit = millisecond / line */
    static int speed(int level, int drop_type);
};

} // namespace TetrisModel

#endif // __MODEL_H__