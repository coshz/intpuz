#include "model.h"

#include <random>
#include <stdexcept>

bool Tetromino::update_if(int dX, int dY, int dO, bool cond) 
{ 
    if(cond) { 
        base[0] += dX; base[1] += dY; orient = ((orient+dO)%4+4)%4; 
        co = make_coord(shape,orient,base);
    }
    return cond;
}

template<typename Pred>
bool Tetromino::update_if(int dX, int dY, int dO, Pred &&pred)
{
    Tetromino tmp { *this };
    tmp.update_if(dX, dY, dO, true);
    if(pred(tmp)) { *this = tmp; return true; }
    else { return false; }
}

Coord Tetromino::make_coord(Shape s, int o, Pos b)
{
    assert(s != Empty);
    Coord co;
    for(int i=0; i<4; i++) 
        co[i] = Pos { b[0]+raw_pos[s][o][i][0], b[1]+raw_pos[s][o][i][1] };
    return co;
}

template<size_t L1, size_t L2>
bool Matrix<L1,L2>::fix_tetro(const Tetromino &t)
{
    auto co = t.coord();
    for(const auto &p: co) {
        if(!is_valid(p[0],p[1])) return false;
    }
    for(const auto &p: co) data_[p[0]][p[1]] = t.get_shape();
    return true;
}

template<size_t L1, size_t L2>
int Matrix<L1,L2>::clear_first() 
{   
    auto full_row = [this](int y) -> bool {
        for(int x=0; x<L1; x++) if(data_[x][y] == Empty) return false;
        return true;
    };

    int y1, y2;

    for(y1=0; y1<L2 && !full_row(y1); y1++);
    for(y2=y1; y2<L2 && full_row(y2); y2++);

    // reset rows [y1, y2)
    for(int y=y1; y<y2; y++) for(int x=0; x<L1; x++) data_[x][y] = Empty;

    return y2-y1;
}

template<size_t L1, size_t L2>
bool Matrix<L1,L2>::apply_gravity_effect()
{
    /// current implementation adheres to the standard practice: 
    /// the minos translate downward to fill the gaps as entire rows

    auto is_empty_line = [this](int y) -> bool {
        for(int x=0; x<L1; x++) if(data_[x][y] != Empty) return false;
        return true;
    };
    int dy = 0;
    for(int y=0; y<L2; y++) {
        if(is_empty_line(y)) {
            dy++;
        } else if(dy>0) {
            for(int x=0; x<L1; x++) data_[x][y-dy] = data_[x][y];
        }
    }
    for(int y=L2-dy; y<L2; y++) for(int x=0; x<L1; x++) 
        data_[x][y] = Empty;
    
    return dy > 0;
}

TetrisModel::TetrisModel()
:matrix_{},tetro_{},state_{},tgen_{},thol_{}
{
}

void TetrisModel::reset()
{
    matrix_={}, tetro_={}, state_={}, tgen_={}, thol_={};
    renew();
}

void TetrisModel::renew(const std::optional<Tetromino> &tetro)
{
    // create fresh tetro 
    tetro_ = tetro ? tetro.value() : tgen_.next();

    // update holdadble state 
    holdable_ = true;
}

bool TetrisModel::transform(Tran t)
{
    switch(t) {
    case Tran_L: return tetro_.update_if(-1,0,0,permissible_pred);
    case Tran_R: return tetro_.update_if(+1,0,0,permissible_pred);
    case Tran_C: return walltick(tetro_,true);
    case Tran_AC:return walltick(tetro_,false);
    default:     throw std::logic_error("??? tran is unrecognized");
    }
}

bool TetrisModel::walltick(Tetromino &t, bool cw) 
{
    assert(tetro_.orient <= 3 && tetro_.orient >=0);
    auto DISP = raw_disp[tetro_.shape==I][tetro_.orient * 2 + !cw];
    for(int i = 0; i < 5; i++) {
        if(t.update_if(DISP[i][0], DISP[i][1], cw?1:-1, permissible_pred)) return true;
    }
    return false;
}

bool TetrisModel::permissible(const Coord &co) const
{
    for(const auto &p: co) {
        if(!matrix_.is_empty(p[0],p[1])) return false;
    }
    return true;
}

int TetrisModel::check_tspin(const Tetromino &tetro) const
{ 
    auto is_block = [this](int x, int y) -> bool {
        return matrix_.is_valid(x,y) && !matrix_.is_empty(x,y);
    };

    int type = 0;
    if(tetro.shape == T)
    {
        auto co = tetro.coord();
        auto P0 = co[0], P1 = co[1], P2 = co[2], P3 = co[3];
        Pos A { P1[0] + P3[0] - P0[0], P1[1] + P3[1] - P0[1] };
        Pos B { P2[0] + P3[0] - P0[0], P2[1] + P3[1] - P0[1] };
        Pos C { 2 * P0[0] - B[0], 2 * P0[1] - B[1] };
        Pos D { 2 * P0[0] - A[0], 2 * P0[1] - A[1] };

        bool b1 = !matrix_.is_empty(A[0], A[1]), b2 = !matrix_.is_empty(B[0], B[1]);
        bool b3 = is_block(C[0],C[1]), b4 = is_block(D[0],D[1]);

        if(b1 && b2 && (b3 || b4)) type = 2;
        else if(b3 && b4 && (b1 || b2)) type = 1;
    }
    return type;
}

bool TetrisModel::lockdown()
{
    if(!matrix_.fix_tetro(tetro_)) return false;

    // clear matrix and memorize action
    Action act = NoAction;
    int tspin = check_tspin(tetro_);
    int line = matrix_.clear_first();

    act = eval_action(tspin, line);
    // first NoAction is real no action
    if(act == NoAction) { state_.mem_action(NoAction); }
    else while(NoAction != act) 
    {
        // in current implementation of `apply_gravity_effect`, the loop will 
        // be iterated at most once
        state_.mem_action(act);
        matrix_.apply_gravity_effect();
        line = matrix_.clear_first();
        act = eval_action(0,line);
    }
    return true;
}

int TetrisModel::dropdown(Drop d) 
{
    if(d==Drop_H) {
        int max_dy = [this]() -> int {
            Tetromino t{tetro_};
            int dy=0;
            while(t.update_if(0,-1,0,permissible_pred)) dy++;
            return dy;
        }();
        if(max_dy==0) { 
            return 0;
        } else {
            for(auto i=0;i<max_dy;i++) state_.mem_action(Drop_Hard);
            tetro_.update_if(0,-max_dy,0,true);
            return max_dy;
        }
    } else {
        bool suc = tetro_.update_if(0,-1,0,permissible_pred);
        if(!suc) {
            return 0; 
        } else { 
            if(d==Drop_S) state_.mem_action(Drop_Soft);
            return 1;
        }
    }
}

Tetromino TetrisModel::glance_ghost() const
{
    Tetromino ghost { tetro_ };
    while(ghost.update_if(0,-1,0,permissible_pred));
    return ghost;
}

void TetrisModel::hold()
{
    if(holdable_) 
    {
        renew(thol_.replace(tetro_));
        holdable_ = false; // the order matters since `renew` sets it to True
    }
}

Action TetrisModel::eval_action(int tspin, int line)
{
    if(tspin==2) {
        switch(line) {
            case 0: return TSpin;
            case 1: return TSpin_Single;
            case 2: return TSpin_Double;
            case 3: return TSpin_Triple;
            default: throw std::logic_error("???");
        }
    } else 
    if(tspin==1) {
        switch(line) {
            case 0: return Mini_TSpin;
            case 1: return Mini_TSpin_Single;
            case 2: return Mini_TSpin_Double;
            default: throw std::logic_error("???");
        }
    } else {
        switch(line) {
            case 0: return NoAction;
            case 1: return Single;
            case 2: return Double;
            case 3: return Triple;
            case 4: return Tetris;
            default: throw std::logic_error("???");
        }
    }
}


void State::mem_action(Action a, bool bravo)
{
    // update actions array
    cnt_actions[a]++;

    // update score and back2back/combo
    if(Single <= a && a <= TSpin_Triple) {
        int dscore = level * scoring_coef[a];
        score += dscore;
        // back2back bonus
        score +=  (back2back && action_diff[a]) ? dscore/2 : 0;
        // combo bonus
        score += (combo>0) ? level * scoring_coef[Combo] * combo : 0;
        // bravo bonus
        if(bravo) {
            auto bravo_type = [this](Action act) {
                switch(act) {
                case Single:
                case Mini_TSpin_Single:
                case TSpin_Single: return Bravo_Single;
                case Double:
                case Mini_TSpin_Double:
                case TSpin_Double: return Bravo_Double;
                case Triple:
                case TSpin_Triple: return Bravo_Triple;
                case Tetris:       return back2back ? Bravo_B2B_Tetris : Bravo_Tetris;
                default: throw std::logic_error("???");
                }
            }(a);
            score += level * scoring_coef[bravo_type];
        }
        
        // break or toggle back2back
        if(action_diff[a]) {
            back2back = !back2back; 
        } else if(Single <= a && a <= Triple) {
            back2back = false;
        }
        // accumulate or reset combo
        if(a == TSpin || a == Mini_TSpin) combo=-1; else combo++;
    } else if(Drop_Soft==a || Drop_Hard==a) {
        /**
          *? #lines is requried for Dorp_Hard to update score; currently we 
          *? obtain #lines in TetrisModel::dropdown and update score by calling 
          *? State::mem_action multiple times.
         */
        score += scoring_coef[a];
    } else /* NoAction */ {
        combo=-1;
        back2back=false;
    }

    // update cleared lines
    auto l = [](Action act) -> int {
        switch(act) {
        case Single:
        case Mini_TSpin_Single:
        case TSpin_Single:  return 1;
        case Double:
        case Mini_TSpin_Double:
        case TSpin_Double:  return 2;
        case Triple:
        case TSpin_Triple:  return 3;
        case Tetris:        return 4;
        default:            return 0;
        }
    }(a);
    cnt_lines += l;

    // update level
    if(cnt_lines >= limit_lines_of(level)) level += 1;
}