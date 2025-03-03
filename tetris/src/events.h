#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <wx/wx.h>
#include "def.h"

class DropEvent;
class GameStateEvent;

wxDECLARE_EVENT(EVT_HOLD, wxCommandEvent);
wxDECLARE_EVENT(EVT_DROP, DropEvent);
wxDECLARE_EVENT(EVT_GAMESTATE, GameStateEvent);

class DropEvent: public wxCommandEvent 
{
    Drop drop_;
public:
    DropEvent(Drop drop, wxEventType et=EVT_DROP, int winid=wxID_ANY)
        :drop_{drop}
        ,wxCommandEvent(et,winid) 
    {}

    Drop value() const { return drop_; }

    virtual wxEvent* Clone() const override { return new DropEvent(*this); }
};

enum GameState {
    READY, START, RUNNING, PAUSE, END
};

class GameStateEvent: public wxCommandEvent
{
    GameState state_;
public:
    GameStateEvent(GameState s, wxEventType et=EVT_GAMESTATE, int winid=wxID_ANY)
        :state_(s)
        ,wxCommandEvent(et,winid)
    {}

    GameState value() const { return state_; }

    virtual wxEvent* Clone() const override { return new GameStateEvent(*this); }
};
#endif // __EVENTS_H__