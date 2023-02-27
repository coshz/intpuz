#include "view.h"
#include <iostream>
#include <string>

//? BGR, not RGB
const wxColor Board::color_scheme[8] = {
    wxColor(0x00ffff), wxColor(0xffff00), wxColor(0x800080), wxColor(0x007fff),
    wxColor(0xff0000), wxColor(0x00ff00), wxColor(0x0000ff), wxColor(0xffffff)
};

Board::Board(const model_t &model, wxWindow *parent)
:wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER),
model_{model}
{
    // timer = new wxTimer(this, 1);
    Bind(wxEVT_PAINT, &Board::onPaint, this, wxID_ANY);

}

void Board::onPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    // auto pen = wxPen(*wxWHITE,2);
    // dc.SetPen(pen);
    // dc.DrawEllipse(300,300,200,250);
    
    drawBoard(dc, {150, 10});
    drawHold(dc, {10,10});
    drawNext(dc, {440,10});
    drawScore(dc, {0,0});
}

void Board::drawSquareAt(wxDC &dc, pos_local pos, int shape, int width, double scale)
{
    if(width == 0) dc.SetPen(*wxTRANSPARENT_PEN);
    else dc.SetPen(wxPen(wxColour(0x666666), width));
    auto dw = int(square_width_ * scale), dh = int(square_height_ * scale);
    dc.DrawLine(pos.x, pos.y, pos.x+dw, pos.y);
    dc.DrawLine(pos.x, pos.y, pos.x, pos.y+dh);
    dc.DrawLine(pos.x+dw, pos.y, pos.x+dw, pos.y+dh);
    dc.DrawLine(pos.x, pos.y+dh, pos.x+dw, pos.y+dh);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(color_scheme[shape]));
    dc.DrawRectangle(pos.x+width, pos.y+width, dw-2*width, dh-2*width);
}

void Board::drawTetroAt(wxDC &dc,  const TetrisModel::Tetromino &tetro, 
                        pos_local offset, int width, double scale)
{
    auto co = tetro.coord();
    auto shape = tetro.getShape();
    for(auto &pos: co)
    {
        pos_local p = { pos[0]*(int)(square_width_*scale)+offset.x, 
                           (Height-1-pos[1])*(int)(square_height_*scale)+offset.y };
        drawSquareAt(dc, p, shape, width, scale);
    }
}

void Board::drawBoard(wxDC &dc, pos_local offset) const
{
    // border Rect
    auto pen = wxPen(wxColour(0x666666), 8);
    dc.SetPen(pen);
    dc.DrawRectangle(
        offset.x, offset.y, 
        square_width_ * Width + 8, square_height_ * Height + 8 + 4
    );

    auto rect = wxRect(
        offset.x + 4, offset.y + 4, 
        square_width_ * Width + 8, square_height_ * Height + 8
    );
    wxDCClipper clip(dc, rect);
    auto coord_trans = [this, &offset](int i, int j){
        return pos_local{
            i * square_width_ + offset.x + 4, 
            (Height-1-j) * square_height_ + offset.y + 8
        };
    };

    // draw matrix

    for(auto i = 0; i < model_.getMatrix().width;  i++)
    for(auto j = 0; j < model_.getMatrix().height; j++)
    {
        auto shape = model_.getMatrix().getShapeAt(i,j);
        auto p = coord_trans(i,j);
        drawSquareAt(dc, p, shape, (shape == 7) ? 0 : 2);
    }

    // draw tetro in play

    drawTetroAt(dc, model_.getTetro(), {offset.x+4, offset.y+8});

    // draw ghost
}

void Board::drawHold(wxPaintDC &dc, pos_local offset) const
{
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    auto pen = wxPen(wxColor(0x666666), 6);
    dc.SetPen(pen);
    int radius = 60;
    dc.DrawCircle(offset.x + radius, offset.y + radius, radius);
    
    if(model_.getState().hold != TetrisModel::Empty) {
        auto t = TetrisModel::Tetromino(model_.getState().hold, 0, {0, 20});
        drawTetroAt(dc, t, { 60, 100 }, 2, 0.8);
    }
}

void Board::drawNext(wxPaintDC &dc, pos_local offset) const
{
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    auto pen = wxPen(wxColor(0x666666), 6);
    dc.SetPen(pen);
    int radius = 60, width = 120;
    dc.DrawCircle(offset.x + radius, offset.y + radius, radius);
    dc.DrawRectangle(offset.x-10, 2 * radius + offset.y + 25, width + 40, 386);
    
    // next
    auto t = TetrisModel::Tetromino(model_.getState().next, 0, {0, 20});
    drawTetroAt(dc, t, {480, 90}, 2, 0.8 );

    std::string tips = ">>> KEY MAP >>>\n"
                       "[Enter]\t=>\tNew\n"
                       "[L]/[R]\t=>\tMove\n"
                       "[UP]\t\t=>\tRotate\n"
                       "[Down]\t=>\tSoft Drop\n"
                       "[Space]\t=>\tHard Drop\n"
                       "[C]\t\t=>\tHold";

    dc.DrawText(wxString(tips.c_str()), offset.x, offset.y + 200);
}

void Board::drawScore(wxPaintDC &dc, pos_local offset) const
{
    auto font = wxFont(20, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    dc.DrawText(wxString::Format("Level: \t%d", model_.getState().level), wxPoint(40,200));
    dc.DrawText(wxString::Format("Score: \t%d", model_.getState().score), wxPoint(40,230));
    dc.DrawText(wxString::Format("Lines: \t%d", model_.getState().line), wxPoint(40,260));
    dc.DrawText(wxString::Format(
        "Tetrises: \t%d", model_.getState().cnt_action[TetrisModel::Tetris]), 
        wxPoint(40,290)
    );
    dc.DrawText(wxString::Format(
        "T-Spins: \t%d", model_.getState().cnt_action[TetrisModel::TSpin] 
        + model_.getState().cnt_action[TetrisModel::TSpin_Single]
        + model_.getState().cnt_action[TetrisModel::TSpin_Double]
        + model_.getState().cnt_action[TetrisModel::TSpin_Triple]), 
        wxPoint(40,330)
    );

}
