#include "tetris.h"

IMPLEMENT_APP(Tetris)

bool Tetris::OnInit()
{
    srand(time(NULL));
    TetrisFrame *frame = new TetrisFrame(wxT("Tetris"));
    frame->Show(true);
    frame->SetSize(wxSize(600,600));
    return true;
}

TetrisFrame::TetrisFrame(const wxString &title)
:wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,
wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER|wxMAXIMIZE_BOX)),
board_{}, rs_{}, ts_{200,300,500}, gs_{}, flag_{Ready_}
{
    board_ = new Board(model_, this);
    board_->SetSize(GetSize());

    drop_timer_ = new wxTimer(this);
    Bind(wxEVT_TIMER, &TetrisFrame::autoDrop, this, drop_timer_->GetId());

    Bind(wxEVT_CHAR_HOOK, &TetrisFrame::onKeyDown, this); 
    // Bind(wxEVT_PAINT, &TetrisFrame::onPaint, this);

    Bind(tetris_lockdown, &TetrisFrame::onSettle, this);

    lock_timer_ = new wxTimer(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent& event){
        wxCommandEvent evt{tetris_lockdown};
        wxPostEvent(this, evt);
    }, lock_timer_->GetId());

    // statusbar = new wxStatusBar(this);
    CreateStatusBar();
    updateBar();
    // statusbar->SetStatusText("")
}


void TetrisFrame::ready()
{
    model_.reset();
    updateBar();
}

void TetrisFrame::start()
{
    flag_ = Running_;
    drop_timer_->Start(timerInterval());
    const_cast<TetrisModel::State&>(model_.getState()).initNextQueue(rs_);
    model_.generate();
    updateBar();
}

void TetrisFrame::pause()
{
    flag_ = (flag_ == Running_) ? Pause_: Running_ ;
    if(flag_ == Running_) drop_timer_->Start(timerInterval());
    else drop_timer_->Stop();
    updateBar();
}

void TetrisFrame::end()
{
    flag_ = End_;
    drop_timer_->Stop();
    updateBar();
}

void TetrisFrame::updateBar()
{
    auto str_flag = [](StateFlag sf) {
        switch(sf){
            case Ready_: return wxString("Ready");
            case Running_: return wxString("Running");
            case Pause_: return wxString("Pause");
            case End_: return wxString("End");
        }
    };

    SetStatusText(str_flag(flag_),0);
}

void TetrisFrame::onSettle(wxCommandEvent &event)
{
    
    model_.settle(gs_);

    if(model_.isover()) { end(); return; }

    // update drop_timer to control drop speed
    drop_timer_->Start(ts_.speed(model_.getState().level, 0));

    // update next queue
    while(!model_.getState().isFullNext()) { model_.pushNext(rs_); }

    model_.generate();
}

int TetrisFrame::timerInterval() const
{
    return ts_.speed(model_.getState().level, 0);
}

void TetrisFrame::onTimer(wxTimerEvent &event)
{
    // autoDrop();
    event.Skip();
}

void TetrisFrame::onKeyDown(wxKeyEvent &event)
{
    // static bool allow_repeat = true;
    timerToggle(false);
    int kc = event.GetKeyCode();

    if(flag_ == Ready_ && kc == WXK_RETURN ) { start(); return; }
    if(flag_ == End_ && kc == WXK_RETURN) { ready(); start(); return; }
    if(kc == 'p' || kc == 'P' || kc == WXK_ESCAPE) { pause(); return; }
    if(flag_ != Running_) return;

    switch(kc)
    { 
        // move left / right
        case WXK_LEFT:      model_.transform(0); break;
        case WXK_RIGHT:     model_.transform(1); break;
        // rotation
        case 'x':
        case 'X':
        case WXK_UP:        model_.transform(2); break;
        case 'z':
        case 'Z':
        case WXK_CONTROL:   model_.transform(3); break;
        // drop
        case WXK_DOWN:      model_.drop(1); break;
        case WXK_SPACE:     model_.drop(2); lock_timer_->StartOnce(1); break;
        // hold
        case 'c':
        case 'C':
        case WXK_SHIFT:     model_.hold(); break;
        default: event.Skip();
    }
    Refresh();
    drop_timer_->Start(timerInterval());
}

// void TetrisFrame::onPaint(wxPaintEvent &event)
// {
//     switch(flag_)
//     {
//         case Running_: event.Skip(); break;
//         case Pause_: cout << "Pause\n"; break;
//         case End_: cout << "End\n"; break;
//         case Ready_: cout << "Ready\n"; break;
//     }
// }

void TetrisFrame::autoDrop(wxTimerEvent &event)
{
    if(model_.lockable()) {
        lock_timer_->StartOnce(ts_.lockdown_delay);
    } else {
        model_.drop(0);
    } 
    Refresh();
}

void TetrisFrame::timerToggle(bool s)
{
    if(s) {
        if(drop_timer_) drop_timer_->Start(timerInterval());
        if(lock_timer_) lock_timer_->StartOnce(ts_.lockdown_delay);
    } else {
        if(drop_timer_) drop_timer_->Stop();
        if(lock_timer_ && lock_timer_->IsRunning()) lock_timer_->Stop();
    }
}


