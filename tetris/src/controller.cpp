#include "controller.h"

TetrisController::TetrisController(model_t *m, view_t *v)
:model_(m),view_(v),timer_(new timer_t(model_,this))
{
    Bind(EVT_DROP, &TetrisController::OnDrop, this);
    Bind(EVT_HOLD, &TetrisController::OnHold, this);

    view_->Bind(wxEVT_CHAR_HOOK, &TetrisController::OnKeyDown, this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent &e) {
        wxPostEvent(this, DropEvent(Drop_N));
    });

    Ready();
}

TetrisController::~TetrisController() 
{
    if(timer_->IsRunning()) { timer_->Stop(); }
}

void TetrisController::Ready()
{
    model_->reset();
    view_->update_all();
    state_=READY;
    wxPostEvent(this, GameStateEvent(state_));
}

void TetrisController::Start()
{
    timer_->Start();
    state_=RUNNING;
    wxPostEvent(this, GameStateEvent(state_));
}

void TetrisController::Pause()
{
    timer_->Stop();
    state_=PAUSE;
    wxPostEvent(this, GameStateEvent(state_));
}

void TetrisController::Resume()
{
    timer_->Start();
    state_=RUNNING;
    wxPostEvent(this, GameStateEvent(state_));
}

void TetrisController::End()
{
    timer_->Stop();
    state_=END;
    wxPostEvent(this, GameStateEvent(state_));
}

void TetrisController::Transform(const Tran& tran)
{
    model_->transform(tran);
    view_->update_playfiled();
    view_->update_scorearea();
}

void TetrisController::Lockdown()
{
    timer_->Stop();
    bool suc = model_->lockdown();
    if(!suc) {
        End(); 
        return;
    } else {
        model_->renew();
        view_->update_nextarea();
        view_->update_scorearea();
    }
    timer_->Start();
}

void TetrisController::OnDrop(const DropEvent& e)
{
    int d = model_->dropdown(e.value());
    if(d == 0) { 
        Lockdown(); 
    }
    else {
        view_->update_playfiled();
        view_->update_scorearea();
    }
}

void TetrisController::OnKeyDown(wxKeyEvent &e)
{
    const auto kc = e.GetKeyCode();

    // switch state
    if(state_==END && kc==WXK_RETURN)       { Ready(); return; }
    if(state_==READY && kc==WXK_RETURN)     { Start(); return; } 
    if(state_==RUNNING && kc==WXK_ESCAPE)   { Pause(); return; }
    if(state_==PAUSE && kc==WXK_ESCAPE)     { Resume(); return; }

    // force to restart!
    if(kc=='R')                             { Ready(); Start(); return; } 

    // forward event 
    if(state_!=RUNNING) return;
    switch(kc) {
        case WXK_LEFT:      Transform(Tran_L); break;
        case WXK_RIGHT:     Transform(Tran_R); break;
        case 'X':
        case WXK_UP:        Transform(Tran_C); break;
        case 'Z':
        case WXK_CONTROL:   Transform(Tran_AC); break;
        case 'C':       
        case WXK_SHIFT:     wxPostEvent(this, wxCommandEvent(EVT_HOLD)); break;
        case WXK_DOWN:      wxPostEvent(this, DropEvent(Drop_S)); break;
        case WXK_SPACE:     wxPostEvent(this, DropEvent(Drop_H)); break;
        default: e.Skip();
    }
}

void TetrisController::OnHold(const wxCommandEvent &e)
{
    model_->hold();
    view_->update_holdarea();
}