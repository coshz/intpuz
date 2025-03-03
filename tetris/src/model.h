#ifndef __MODEL_H__
#define __MODEL_H__

#include "def.h"

#include <array>
#include <random>
#include <optional>
#include <algorithm>
#include <functional>


class State: public StateTemplate<State>
{
    std::array<size_t,NUM_ACTION> cnt_actions;
    int cnt_lines;
    bool back2back;
    int combo;
    int level;
    int score;
public:
    State()
    :cnt_actions{},cnt_lines{0},back2back{false},combo{-1},level{1},score{0} 
    {}

    auto get_score() const              { return score; }
    auto get_level() const              { return level; }
    auto get_count_of(Action a) const   { return cnt_actions[a]; }
    auto get_combo() const              { return combo; }
    auto get_back2back() const          { return back2back ? 1 : 0; }

    /* memerize action and update state */
    void mem_action(Action a, bool bravo=false);

private:
    /* max cleared lines of given level */
    static int limit_lines_of(int level) { return 5*(1+level)*level/2; }
};

using Coord = std::array<Pos,4>;

/** 
 * \brief Tetromino class
 * \note: either 3-tuple (shpae,orient,base) or coord[4] is logically enough;
 *        we use both just for convenience. 
 */
class Tetromino: public TetrominoTemplate<Tetromino>
{
    Shape   shape;
    int     orient;
    Pos     base;
    Coord   co;
    friend class TetrisModel;
public:
    
    Tetromino(Shape s=O, int o=0, Pos b={0,0})
    :shape{s},orient{(o%4 + 4)%4},base{b},co{make_coord(s,o,b)}
    {}

    Shape   get_shape() const   { return shape; }
    int     get_orient() const  { return orient; }     
    Pos     get_base() const    { return base; }
    
    Pos     operator[](size_t i) const      { return co[i%4]; }
    auto    coord() const -> const Coord&   { return co; }

    bool    update_if(int dX, int dY, int dO, bool cond=true);
    template<typename Pred>
    bool    update_if(int dX, int dY, int dO, Pred &&);

    static Coord make_coord(Shape s, int o, Pos b);
}; 

template<size_t L1, size_t L2>
class Matrix 
{
    static constexpr size_t dL = 4; // reserved for invisible tetromino
    std::array<std::array<Shape,L2+dL>,L1> data_; 
    friend class TetrisModel;
public:
    Matrix():data_{}{ for(int i=0;i<L1;i++)for(int j=0;j<L2+dL;j++) data_[i][j]=Empty; }

    auto shape_at(size_t x, size_t y) const -> Shape { return data_.at(x).at(y); }
    bool fix_tetro(const Tetromino &);

    /* clear lines (without gravity-effect) and return the num of cleared lines */
    int clear_first();

    /* rearrange the matrix according to gravity effect */
    bool apply_gravity_effect();

private:
    /* positions that tetro could move into */
    bool is_empty(int x, int y) const { 
        return x >= 0 && x < L1 && y >=0 && y < L2+dL && data_[x][y] == Empty; 
    }
    /* position that tetro could be fixed into */
    bool is_valid(int x, int y) const { 
        return x >= 0 && x < L1 && y >=0 && y < L2 && data_[x][y] == Empty; 
    }
};

class TetrisModel: public TetrisModelTemplate<TetrisModel>
{
    // 7-bag tetro generator
    struct tetro_generator 
    {
        tetro_generator(std::optional<uint64_t> seed=std::nullopt)
        :gen{std::mt19937(seed.has_value()?seed.value():std::random_device{}())}
        ,cur{0},tetro7{}
        {}

        const Tetromino& look(size_t idx) const { 
            return tetro7[(cur+idx)%7];
        }

        const Tetromino& next() { 
            if(1==(cur=(cur+1)%7)) regen_7bag(); 
            return tetro7[cur];
        }

        void regen_7bag() {
            std::array<Shape,7> shape7 { O,I,T,L,J,S,Z };
            std::shuffle(shape7.begin(), shape7.end(), gen); 
            for(int i=0; i<7; i++) {
                int ori = std::uniform_int_distribution(0,3)(gen);
                tetro7[i] = new_tetro(shape7[i], ori);
            }
        }

        std::mt19937 gen;
        size_t cur;
        std::array<Tetromino,7> tetro7;
    };

    struct tetro_holder {
        tetro_holder():tetro{std::nullopt} {}

        auto replace(const Tetromino &t) -> std::optional<Tetromino> {
            auto old = tetro; 
            tetro = new_tetro(t.shape);
            return old;
        }

        std::optional<Tetromino> tetro;
    };

    Matrix<10,20> matrix_; 
    Tetromino tetro_;
    State state_;
    tetro_generator tgen_;
    tetro_holder    thol_;
    bool holdable_;

public:
    constexpr static size_t W=10, H=20; 
    TetrisModel();

    constexpr size_t size_x() const { return W; }
    constexpr size_t size_y() const { return H; }

    /* reset data to default and generate first tetro */
    void reset();

    /* renew tetro using generator or given tetro */
    void renew(const std::optional<Tetromino> &tetro=std::nullopt); 

    bool transform(Tran t);

    /* try to hold tetro; if haldable, renew tetro */
    void hold();

    /* try to drop down tetro; return the displacement of tetro */
    int dropdown(Drop d);

    /* lockdown active tetro; if successful, re-generate tero */
    bool lockdown();

    auto get_matrix() const -> const Matrix<10,20>&         { return matrix_; }
    auto get_shape_at(int x,int y) const -> Shape           { return matrix_.shape_at(x,y); }
    auto get_tetro() const -> const Tetromino *             { return &tetro_; }
    auto get_state() const -> const State *                 { return &state_; } 

    const Tetromino* tetro_from_next(size_t idx) const      { return &tgen_.look(idx%7); }
    const Tetromino* tetro_from_hold() const                { return thol_.tetro ? &thol_.tetro.value() : nullptr; }
  
    auto glance_ghost() const -> Tetromino;

private:

    /// create a tetromino with fixed base { (W-1)/2,H }
    /// NOTE: in current implementation, the argument `orient` is NOT used!
    static Tetromino new_tetro(Shape shape, int orient=0) { 
        return Tetromino { shape,0,Pos{(W-1)/2,H} }; 
    }
    
    /* evaluate action from tspin type and num of cleared lines */
    static Action eval_action(int tspin, int line);

    /* whether a coord could be contained in matrix */
    bool permissible(const Coord &co) const;

    /* whether a tetromino could be contained in matrix */
    std::function<bool(const Tetromino&)> 
        permissible_pred = [this](const Tetromino &t) -> bool { return permissible(t.coord()); };

    /* walltick algorithm used in rotation */
    bool walltick(Tetromino &t, bool cw);

    /* check T-spin, 0, 1, 2 for nothing, mini-tspin, tspin */
    int check_tspin(const Tetromino &tetro) const;
};

#endif // __MODEL_H__