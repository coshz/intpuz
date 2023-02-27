#include <ctime>
#include <cmath>
#include <algorithm>
#include "model.h"

namespace TetrisModel
{

template<class matrix_t,class tetromino_t,class state_t>
Model<matrix_t,tetromino_t,state_t>
::Model():matrix_{}, tetro_{}, state_{}, end(false)
{
}

template<class matrix_t,class tetromino_t,class state_t>
void Model<matrix_t,tetromino_t,state_t>
::reset()
{
    matrix_={}, tetro_={}, state_={}, end = false;
}

template<class matrix_t,class tetromino_t,class state_t>
Pos Model<matrix_t,tetromino_t,state_t>
::default_base(const Shape &shape)
{
    // default base is irrelated to shape at present
    return { matrix_t::width / 2 - 1, matrix_t::height + 1 };
}

template<class matrix_t,class tetromino_t,class state_t>
void Model<matrix_t,tetromino_t,state_t>
::generate(Shape shape)
{
    if(shape == Empty) {
        shape = state_.next;
        state_.next = state_.next_queue.front();
        state_.next_queue.pop_front();
    }
    // assert shape != Empty
    tetro_ = tetromino_t(shape, 0, default_base(shape));
}

template<class matrix_t,class tetromino_t,class state_t>
void Model<matrix_t,tetromino_t,state_t>
::hold()
{
    auto prev = state_.hold;
    if(!state_.holdable && prev == Empty) return;
    if(state_.holdable && prev == Empty) {
        state_.hold = tetro_.getShape(); 
        generate();
        return;
    }
    if(state_.holdable && prev != Empty) {
        state_.hold = tetro_.getShape(); 
        state_.holdable = false;
        generate(prev);
        return;
    } 
    if(!state_.holdable && prev != Empty) {
        state_.hold = Empty;
        generate(prev);
        return;
    }
}

template<class matrix_t,class tetromino_t,class state_t>
bool Model<matrix_t,tetromino_t,state_t>
::lockable() const
{
    return !translatable(0,-1);
}

template<class matrix_t,class tetromino_t,class state_t>
template<typename goal_system_t>
void Model<matrix_t,tetromino_t,state_t>
::settle(const goal_system_t &gs)
{
    state_.action = lockdown();
    state_.score += state_.level * (
        gs.scoreAction(state_.action, state_.back2back) + 
        gs.scoreDrop(state_.cnt_drop[Hard], Hard) 
        // + gs.scoreDrop(state_.cnt_drop[Soft], Soft) // add when drop
    );
    state_.line += gs.lineAction(state_.action, state_.back2back);
    if(gs.isLevelUp(state_.level, state_.line)) state_.level++;

    if(state_.action == Single || state_.action == Double || state_.action == Triple){
        state_.back2back = false;
    } else if(state_.action != Nothing) {
        state_.back2back = true;
    }
    state_.cnt_action[state_.action]++;
    state_.holdable = true;
}

template<class matrix_t,class tetromino_t,class state_t>
bool Model<matrix_t,tetromino_t,state_t>
::transform(int tran_type)
{
    /* try 5 disp for rotation until success or  keep still if fails */
    auto do_translate = [this](int dx, int dy) -> bool {
        if(translatable(dx, dy)) {
            return tetro_.update({dx,dy}, 0);
        } else {       
            return false;
        }
    };

    auto do_rotate = [this](bool cw) -> bool {
        auto disp = tetro_.getDisp(tetro_.getShape(), tetro_.getOrient(), cw);
        int idx = -1;
        while(++idx < 5 && !rotatable(cw, disp[idx]));
        if(idx != 5) {
            state_.rotate_idx = idx;
            return tetro_.update({disp[idx][0], disp[idx][1]}, cw ? 1 : -1);
        } else {
            return false;
        }
    };

    switch((Trans)(tran_type))
    {
        case tran_L:    return do_translate(-1,0);
        case tran_R:    return do_translate(+1,0); 
        case tran_CW:   return do_rotate(true); 
        case tran_ACW:  return do_rotate(false);
    }
}

template<class matrix_t,class tetromino_t,class state_t>
void Model<matrix_t,tetromino_t,state_t>
::drop(int drop_type)
{
    auto do_drop = [this](int d = 0) -> int {
        int md;
        for(md = 1; translatable(0, -md); md++);
        d = (d == 0) ? md - 1: (d >= md) ? 0 : d;
        tetro_.update({0, -d});
        return d;
    };

    state_.cnt_drop[drop_type] += do_drop((drop_type == Hard) ? 0 : 1);
    // sort-drop: update score in real-time
    if(drop_type == 1) state_.score += state_.level * 1;
}

template<class matrix_t,class tetromino_t,class state_t>
bool Model<matrix_t,tetromino_t,state_t>
::translatable(int dx, int dy) const
{
    Coord co = tetro_.coord(dx, dy, 0);

    return permissible(co);
}

template<class matrix_t,class tetromino_t,class state_t>
bool Model<matrix_t,tetromino_t,state_t>
::rotatable(bool cw, const int disp[2]) const
{
    Coord co = tetro_.coord(disp[0], disp[1], cw ? 1 : -1);
    return permissible(co);
}

template<class matrix_t,class tetromino_t,class state_t>
bool Model<matrix_t,tetromino_t,state_t>
::permissible(const Coord &co) const
{
    auto out_scope = [this](const Pos &pos) -> bool {
        return pos[0] < 0 || pos[0] >= matrix_.width || pos[1] < 0;
    };

    for(const auto &pos: co)
    {
        if(out_scope(pos) || !matrix_.isEmpty(pos[0], pos[1])) return false;
    }
    return true;
}

template<class matrix_t,class tetromino_t,class state_t>
Coord Model<matrix_t,tetromino_t,state_t>
::ghost() const
{
    int d = 0;
    while(translatable(0,-(++d)));
    return tetro_.coord(0, -d+1);
}

template<class matrix_t,class tetromino_t,class state_t>
int Model<matrix_t,tetromino_t,state_t>
::checkTspin(const matrix_t& matrix, const tetromino_t& tetro)
{
    int type = 0;

    auto is_hole = [&matrix](int x, int y){
        return x > 0 && x < matrix.width && y > 0 && y < matrix.height && matrix.isEmpty(x,y);
    };
    auto is_block = [&matrix](int x, int y){
        return x > 0 && x < matrix.width && y > 0 && y < matrix.height && !matrix.isEmpty(x,y);
    };

    if(tetro.getShape() == T) 
    {
        Coord co = tetro.coord();
        auto P0 = co[0], P1 = co[1], P2 = co[2], P3 = co[3];
        Pos A { P1[0] + P3[0] - P0[0], P1[1] + P3[1] - P0[1] };
        Pos B { P2[0] + P3[0] - P0[0], P2[1] + P3[1] - P0[1] };
        Pos C { 2 * P0[0] - B[0], 2 * P0[1] - B[1] };
        Pos D { 2 * P0[0] - A[0], 2 * P0[1] - A[1] };

        bool b1 = matrix.isEmpty(A[0], A[1]), b2 = matrix.isEmpty(B[0], B[1]);
        bool b3 = is_block(C[0],C[1]), b4 = is_block(D[0],D[1]);
        // TODO: (maybe) sufficient but not necessary condition
        if(!b1 && !b2 && (b3 || b4)) type = 2;
        else if(b3 && b4 && (!b1 || !b2)) type = 1;
    }
    return type;
}

template<class matrix_t,class tetromino_t,class state_t>
std::vector<int> Model<matrix_t,tetromino_t,state_t>
::checkClear(const matrix_t &matrix, const tetromino_t &tetro)
{
    Coord co = tetro.coord();
    std::set<int> id_to_check{};
    for(auto &pos: co){
        id_to_check.insert(pos[1]);
    }

    std::vector<int> id_to_clear{};
    for(auto &j: id_to_check)
    {
        int idx;
        for(idx = 0; idx < matrix.width && !matrix.isEmpty(idx, j); idx++);
        if(idx >= matrix.width) id_to_clear.push_back(j);
    }

    sort(id_to_clear.begin(), id_to_clear.end());

    return id_to_clear;
}

template<class matrix_t,class tetromino_t,class state_t>
Action Model<matrix_t,tetromino_t,state_t>
::lockdown()
{
    /* update matrix */

    auto co = tetro_.coord();
    auto shape = tetro_.getShape();

    for(const auto &pos: co) matrix_.setShapeAt(shape, pos[0], pos[1]);
        
    /* clear matrix */

    auto id_to_clear = checkClear(matrix_, tetro_);
    auto W = matrix_.width, H = matrix_.height;
        
    auto drop_dh = [&id_to_clear](int h) -> int {
        int dh = 0, n = id_to_clear.size();
        int i, j;
        for(i = 0, j = 0; i < n && j < h; j++)
        {
            // assert id_to_clear is increasing
            if(j == id_to_clear[i]) { dh++, i++; }
        }
        return (i < n && id_to_clear[i] == h) ? - 1 : dh;
    };
    auto drop_from = [H, &drop_dh](int h) -> int {
        for(auto j = h; j < H; j++)
        {
            if(h + drop_dh(j) == j) return j;
        }
        return -1; 
    };
    for(auto j = 0; j < H; j++)
    {
        auto j2 = drop_from(j);
        for(auto i = 0; i < W; i++)
        {
            auto shape = (j2 == -1) ? Empty : matrix_.getShapeAt(i,j2);
            matrix_.setShapeAt(shape,i,j);  
        }
    }

    /* check end*/
    for(auto &pos: co) if(pos[1] >= matrix_.height) { end = true; break; }

    /* evaluate clear action */

    int tspin_type = checkTspin(matrix_, tetro_);

    int line = 0;
    for(auto i = 0, j = 0; i < id_to_clear.size() && j <= H-1; j++)
    if(j == id_to_clear[i]) { line++, i++; }

    if(tspin_type == 0) {
        switch(line)
        {
            case 0: return Nothing;
            case 1: return Single;
            case 2: return Double;
            case 3: return Triple;
            case 4: return Tetris;
        }
    } else if(tspin_type == 1) {
        switch(line){
            case 0: return Mini_TSpin;
            case 1: return Mini_TSpin_Single;
        }
    } else {
        switch(line){
            case 0: return TSpin;
            case 1: return TSpin_Single;
            case 2: return TSpin_Double;
            case 3: return TSpin_Triple;
        }
    }
    return Nothing; // nonsense just to suppress the warning of stupid compiler
}

template<class matrix_t,class tetromino_t,class state_t>
template<typename random_system_t>
void Model<matrix_t,tetromino_t,state_t>
::pushNext(const random_system_t &rs)
{
    state_.next_queue.push_back(rs.next());
}

Tetromino::Tetromino(Shape shape, int orient, Pos base)
:shape_(shape),orient_(0),base_(base)
{
}

Coord Tetromino::coord(int dx, int dy, int dr) const
{
    return coord(shape_, (orient_ + dr) % 4, {base_[0] + dx, base_[1] + dy});
}

Coord Tetromino::coord(Shape shape, int orient, Pos base)
{
    //assert shape != Empty
    auto pos = getPos(shape, orient);
    Coord tmp{};
    for(auto i = 0; i < 4; i++)
    {
        tmp[i][0] = pos[i][0] + base[0];
        tmp[i][1] = pos[i][1] + base[1];
    }
    return tmp;
}

bool Tetromino::update(const Pos &db, const int &dr)
{
    base_[0] += db[0];
    base_[1] += db[1];
    orient_ = (orient_ + dr) % 4;
    return !(db[0] == 0 && db[1] == 0 && dr == 0);
}

template<int W, int H>
Matrix<W,H>::Matrix():data_{}
{
    for(auto i = 0; i < W; i++)
    for(auto j = 0; j < H + hide_H; j++)
    data_[i][j] = Empty;
}

template<int rs_type>
template<typename T, int N>
void RandomSystem<rs_type>::reset(T (&arr)[N])
{
    auto seed = time(NULL);
    srand(seed);
    if(rs_type == 0) {
        // ruffle
        for(auto i = N - 1; i > 0; i--)
        {
            auto j = rand() % (i + 1);
            std::swap(arr[i], arr[j]);
        }
    } else {
        // random generate
        T cat_tetro[N];
        for(auto i= 0; i < N; i++) cat_tetro[i] = i;
        for(auto i = 0; i < N; i++)
        {
            auto idx = rand() % N;
            arr[i] = cat_tetro[idx];
        }
    }
}

template<int rs_type>
Shape RandomSystem<rs_type>::next() const
{
    static int next_arr_[7] = { 0, 1, 2, 3, 4, 5, 6 };
    static int idx = 0;
    idx = (idx + 1) % 7;
    if(idx == 1) reset(next_arr_);
    return (Shape)next_arr_[idx];
}

template<int gs_type>
int GoalSystem<gs_type>::scoreAction(const Action &action, bool b2b)
{
    switch(action)
    {
    case Nothing:           return 0;
    case Single:            return 100;
    case Double:            return 300;
    case Triple:            return 500;
    case Tetris:            return b2b ? 1200 : 800;
    case Mini_TSpin:        return 100;
    case Mini_TSpin_Single: return b2b ? 300 : 200;
    case TSpin:             return 400;
    case TSpin_Single:      return b2b ? 1200 : 800;
    case TSpin_Double:      return b2b ? 1800 : 1200;
    case TSpin_Triple:      return b2b ? 2400 : 1600;
    }
}

template<int gs_type>
int GoalSystem<gs_type>::lineAction(const Action &action, bool b2b)
{
    switch(action)
    {
    case Nothing:           return 0;
    case Single:       
    case Mini_TSpin:        return 1;
    case Mini_TSpin_Single: return b2b ? 3 : 2;
    case Double:            return 3;
    case TSpin:             return 4;
    case Triple:            return 5;
    case Tetris:
    case TSpin_Single:      return b2b ? 12 : 8;
    case TSpin_Double:      return b2b ? 18 : 12;
    case TSpin_Triple:      return b2b ? 24 : 16;
    }
}

template<int gs_type>
int GoalSystem<gs_type>::scoreDrop(int line, Drop drop)
{
    switch(drop)
    {
    case Normal:    return 0;
    case Soft:      return 1 * line;
    case Hard:      return 2 * line;
    }
}

template<int gs_type>
bool GoalSystem<gs_type>::isLevelUp(int level, int line)
{
    switch(gs_type)
    {
    case 1:  return line >= 5 * level * (level + 1) / 2;
    default: return line >= 10 * level;
    }
}

int TimeSystem::speed(int level, int drop_type)
{
    auto basic_v = 1000 * pow(0.8 - (level - 1) * 0.007, level - 1);
    switch(drop_type)
    {
        case Soft:  return (int) (basic_v / 20);
        case Hard:  return 1;
        default:    return (int) (basic_v);
    }
}

/* template instances */
template class Matrix<10,20>;
template class Model<Matrix<10,20>,Tetromino,State>;
template void Model<Matrix<10,20>,Tetromino,State>
::pushNext<RandomSystem<0>>(const RandomSystem<0> &);
template void Model<Matrix<10,20>,Tetromino,State>
::settle<GoalSystem<1>>(const GoalSystem<1> &);
template class RandomSystem<0>;

} // namespace TetrisModel