#ifndef __TETRIS_H__
#define __TETRIS_H__

#include "style.h"
#include "controller.h"

class TetrisWindow: public wxFrame
{
    TetrisConfig                cfg_;
    TetrisModel                 model_;
    TetrisBoard<TetrisModel>    view_;
    TetrisController            con_;

    enum { 
        ID_ViewThemeLight = wxID_HIGHEST + 1,
        ID_ViewThemeDark,
        ID_ViewThemeAuto
    };
public:
    TetrisWindow(wxString title);

protected: 
    void OnThemeChanged(wxCommandEvent &e);
    void OnAbout(const wxCommandEvent &e);
    void OnGameStateChanged(const GameStateEvent &e);

private:
    void build_menubar();
    void build_statusbar();

};

#endif // __TETRIS_H__