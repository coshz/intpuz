#include "tetris.h"

class TetrisApp: public wxApp
{
public:
    virtual bool OnInit();
};

bool TetrisApp::OnInit()
{
    auto *w = new TetrisWindow("Tetris");
    w->Show();
    return true;
}

IMPLEMENT_APP(TetrisApp)
