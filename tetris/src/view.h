#ifndef __VIEW_H__
#define __VIEW_H__

#include "def.h"
#include "style.h"

#include <wx/wx.h>
#include <wx/dc.h>
#include <memory>
#include <algorithm>

using matrix_t = Matrix<10,20>;

/* draw a shape in a given rectangle */
static void drawShapeInRect(
    const Shape&, wxRect, wxGraphicsContext*, const TetrisConfig *cfg=nullptr);

/* place tetro' base in a given rectangle */
template<class tetromino_t>
static void drawTetroInRect(
    const tetromino_t&, wxRect, wxGraphicsContext*, const TetrisConfig *cfg=nullptr);

/* place the whole tetro in a given box (w:h=4:2)*/
template<class tetromino_t>
static void drawTetroInBox(
    const tetromino_t&, wxRect, wxGraphicsContext*, double scale=1.0, const TetrisConfig *cfg=nullptr);

template<class model_t>
class HoldArea: public wxPanel
{
    const model_t &model_;
public:
    HoldArea(const model_t &model, wxWindow *parent);
    
private:
    void OnPaint(wxPaintEvent&);
};

template<class model_t>
class NextArea: public wxPanel
{
    static constexpr int next_size = 3;
    const model_t &model_;
public:
    NextArea(const model_t &model, wxWindow *parent);

private:
    void OnPaint(wxPaintEvent&);
};

template<class model_t>
class StateArea: public wxPanel
{
    const model_t &model_;
public:
    StateArea(const model_t &model, wxWindow *parent);

private:
    void OnPaint(wxPaintEvent&);   
};

/// NOTE: HelpArea does NOT depend on model; making it a template class like 
/// other classes in this file to avoid link error `duplicate symbol`
template<class model_t>
class HelpArea: public wxPanel
{
public:
    HelpArea(const model_t&, wxWindow *parent);

private:
    void OnPaint(wxPaintEvent&);
};

template<class model_t>
class PlayField: public wxPanel
{
    const model_t &model_;
    const TetrisConfig *config_;
    const size_t W, H;
public:
    PlayField(const model_t &model, const TetrisConfig *cfg, wxWindow *parent);

private:
    void OnPaint(wxPaintEvent&);

    wxRect bounding_rect() const;

    /* Pos transform from TetrisModel to Playfield */
    Pos ppos_from_mpos(Pos p) const;
};

/** 
 * layout of view components
 */
template<class model_t>
class TetrisBoard: public wxPanel
{
    PlayField<model_t>  *pf_;
    HoldArea<model_t>   *ha_;
    NextArea<model_t>   *na_;
    StateArea<model_t>  *sa_;
public:
    TetrisBoard(const model_t& model, const TetrisConfig *cfg, wxWindow *parent);

    void update_holdarea()  { if(ha_)ha_->Refresh(); }
    void update_nextarea()  { if(na_)na_->Refresh(); }
    void update_scorearea() { if(sa_)sa_->Refresh(); }
    void update_playfiled() { if(pf_)pf_->Refresh(); }
    void update_all() {
        update_holdarea();
        update_nextarea();
        update_scorearea();
        update_playfiled();
    }
};

#include "view.inl"
#endif // __VIEW_H__