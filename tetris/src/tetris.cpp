#include "tetris.h"
#include <wx/aboutdlg.h>
#include <wx/statusbr.h>

#define ProgName "Tetris"
#define Version  "0.2.0.2025-02-26"
#define Author   "COSHZ <fsinhx@gmail.com>"

TetrisWindow::TetrisWindow(wxString title)
    :cfg_{}
    ,model_{}
    ,view_(model_, &cfg_, this)
    ,con_(&model_, &view_)
    ,wxFrame(NULL, wxID_ANY, title)
{ 
    // propagate event from controller to window 
    con_.SetNextHandler(this);

    auto w = GetBestSize().GetWidth(), h = GetBestSize().GetHeight()+20;
    SetInitialSize(wxSize(w,h));
    SetMinSize(wxSize(w,h));
    SetMaxSize(wxSize(w,h));

    build_menubar();
    build_statusbar(); 

    Bind(EVT_GAMESTATE, &TetrisWindow::OnGameStateChanged, this);
}

void TetrisWindow::build_menubar() 
{
    // View
    auto *view_menu = new wxMenu();

    auto *view_theme = new wxMenu("Theme");
    view_theme->AppendRadioItem(ID_ViewThemeLight,"Light");
    view_theme->AppendRadioItem(ID_ViewThemeDark,"Dark");
    view_theme->AppendRadioItem(ID_ViewThemeAuto,"Auto");

    view_menu->AppendSubMenu(view_theme,"Theme");

    // Help
    auto *help_menu = new wxMenu();

    auto *help_item_about = new wxMenuItem(help_menu, wxID_ABORT, "&About");

    help_menu->Append(help_item_about);

    // Menu Bar 
    auto *menubar = new wxMenuBar();
    menubar->Append(view_menu, "View");
    menubar->Append(help_menu, "Help");

    SetMenuBar(menubar);

    // Event Binding
    Bind(wxEVT_MENU, &TetrisWindow::OnThemeChanged, this, ID_ViewThemeLight, ID_ViewThemeAuto);
    Bind(wxEVT_MENU, &TetrisWindow::OnAbout, this, help_item_about->GetId());
}

void TetrisWindow::build_statusbar() 
{
    auto *statusbar = new wxStatusBar(this);
    SetStatusBar(statusbar);
}

void TetrisWindow::OnThemeChanged(wxCommandEvent &e) 
{
    auto id = e.GetId();
    switch(id) {
        case ID_ViewThemeLight: 
            cfg_.theme = TetrisConfig::ThemeLight; break;
        case ID_ViewThemeDark:  
            cfg_.theme = TetrisConfig::ThemeDark; break;
        case ID_ViewThemeAuto:
            cfg_.theme = TetrisConfig::ThemeAuto; break;
        default: 
            return;
    }        
    auto *theme_menu = GetMenuBar()->FindItem(id)->GetMenu();
    theme_menu->Check(id, true);
}

void TetrisWindow::OnGameStateChanged(const GameStateEvent &e)
{
    auto text = wxString();
    switch(e.value()) {
    case READY: 
        text.Append("Ready"); break;
    case START:
        text.Append("Start"); break;
    case RUNNING:
        text.Append("Running"); break;
    case PAUSE:
        text.Append("Pause"); break;
    case END:
        text.Append("End"); break;
    default: 
        std::logic_error("???");
    }

    GetStatusBar()->SetStatusText(text);
}

void TetrisWindow::OnAbout(const wxCommandEvent &e)
{
    wxAboutDialogInfo info;
    info.SetName(wxT(ProgName));
    info.SetVersion(wxT(Version));
    info.SetCopyright(wxT(Author));
    wxAboutBox(info,this);
}