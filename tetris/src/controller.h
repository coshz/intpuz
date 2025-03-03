#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "model.h"
#include "view.h"
#include "events.h"
#include <wx/wx.h>

using model_t = class TetrisModel;
using view_t  = class TetrisBoard<TetrisModel>;

class TetrisController: public wxEvtHandler
{
    class adaptive_timer;
    using timer_t = class adaptive_timer;

    GameState   state_;
    model_t     *model_;
    view_t      *view_;
    timer_t     *timer_;
public:
    TetrisController(model_t *m, view_t *v);
    ~TetrisController();
    
    void Ready();
    void Start();
    void Pause();
    void Resume();
    void End();

protected:
    void OnHold(const wxCommandEvent&);
    void OnDrop(const DropEvent&);
    void OnKeyDown(wxKeyEvent&);

private:
    void Lockdown();
    void Transform(const Tran&);

private:
    /* A timer class that will auto-update interval when it starts */
    class adaptive_timer: public wxTimer {
        model_t *model;
        /* move speed (msec per cell)*/
        static int speed_of(std::size_t level) { return 1000*std::pow(0.8-level*0.007,level); }    
    public:
        adaptive_timer(model_t *m, wxEvtHandler *owner)
        :model(m), wxTimer(owner) 
        {}

        bool Start() {
            int interval = speed_of(model->get_state()->get_level());
            return wxTimer::Start(interval, false);
        }
    };
};

#endif // __CONTROLLER_H__