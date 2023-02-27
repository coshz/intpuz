#ifndef __TETRIS_H__
#define __TETRIS_H__
#include <wx/wx.h>
#include "model.h"
#include "view.h"

class Tetris: public wxApp
{
public:
    virtual bool OnInit();
};

class TetrisFrame: public wxFrame
{
public: 
    TetrisFrame(const wxString &title);

    void ready();
    void start();
    void pause();
    void end();

protected:
    /* conntrol motion of tetromino */
    void onKeyDown(wxKeyEvent &event);
    void onTimer(wxTimerEvent &event);
    void onPaint(wxPaintEvent &event);

    void autoDrop(wxTimerEvent &event);
    void onSettle(wxCommandEvent &event);
    void timerToggle(bool);
    int timerInterval() const;
    
    void updateBar();
private:
    enum StateFlag { Ready_, Running_, Pause_, End_ };
    StateFlag flag_;
    wxTimer *drop_timer_;
    wxTimer *lock_timer_;
    TetrisModel::Model<> model_;
    Board *board_;
    TetrisModel::TimeSystem ts_;
    TetrisModel::RandomSystem<0> rs_;
    TetrisModel::GoalSystem<1> gs_;
    wxStatusBar *statusbar;
};


// lockdown event to prelude matrix lockdown */
wxDEFINE_EVENT(tetris_drop, wxCommandEvent);
wxDEFINE_EVENT(tetris_lockdown, wxCommandEvent);

#endif //__TETRIS_H__