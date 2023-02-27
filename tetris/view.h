#ifndef __VIEW_H__
#define __VIEW_H__

#include <wx/wx.h>
#include <wx/dc.h>
#include "model.h"

using matrix_t = TetrisModel::Matrix<10,20>;
using tetromino_t = TetrisModel::Tetromino;
using model_t = TetrisModel::Model<>;

/* Tetris GUI widgets */
class Board: public wxPanel
{
public:
    Board(const model_t& model, wxWindow *parent=NULL);
    using pos_local = struct { int x, y; };

    static void drawSquareAt(wxDC &dc, pos_local pos, int shape, int width=0, double scale=1.0);
    static void drawTetroAt(wxDC &dc,  const TetrisModel::Tetromino &tetro, 
                            pos_local offset, int width=2, double scale=1.0);

    void drawBoard(wxDC &dc, pos_local offset) const;
    void drawHold(wxPaintDC &dc, pos_local offset) const;
    void drawNext(wxPaintDC &dc, pos_local offset) const;
    void drawScore(wxPaintDC &dc, pos_local offset) const;

protected:
    /* draw matrix and tetromino */
    void onPaint(wxPaintEvent &event);

private:
    enum { Width = 10, Height = 20 };
    constexpr static int square_width_ = 26, square_height_ = 26; 
    /* color for shapes O, I, T, L, J, S, Z */
    static const wxColor color_scheme[8];
    // const matrix_t& matrix_;
    // const tetromino_t& tetro_;
    const model_t& model_;
};


#endif // __VIEW_H__