#ifndef __STYLE_H__
#define __STYLE_H__

#include "def.h"
#include "help.hpp"

#include <wx/wx.h>

namespace {
    ///
    /// shape[8](BGR): O=yellow, I=cyan, T=purple, L=orange, J=blue, S=green, Z=red, Empty=black
    /// mode[2]: light/dark appearance
    ///
    constexpr int SHAPE_COLORS[2][8] = {
        0x00ffff,0xffff00,0x800080,0x007fff,0xff0000,0x00ff00,0x0000ff,0xffffff,
        0x00ffff,0xffff00,0x800080,0x007fff,0xff0000,0x00ff00,0x0000ff,0x000000
    }; 
    constexpr int BACKGROUND_COLORS[] = { 0xffffff, 0x000000 }; 
    constexpr int BLOCK_SIZE = 26;
    constexpr int BORDER_WIDHT = 1;
    constexpr int BORDER_COLOR = 0x666666;
}

struct TetrisConfig 
{
    enum Theme { ThemeLight=0, ThemeDark, ThemeAuto };

    Theme   theme;
    size_t  blocksize;
    size_t  borderwidth;
    int     bordercolor;
    int     bgcolors[2];
    int     scolors[2][8];

    int     color_of(Shape s) const { return scolors[is_dark_theme()][s]; }
    int     bgcolor()         const { return bgcolors[is_dark_theme()]; }

    constexpr TetrisConfig() 
    :blocksize(BLOCK_SIZE)
    ,borderwidth(BORDER_WIDHT),bordercolor(BORDER_COLOR)
    ,scolors{},bgcolors{}
    ,theme(ThemeLight)
    {
        for(int t=0; t<2; t++) for(int s=0; s<8; s++) 
            scolors[t][s] = SHAPE_COLORS[t][s];
    }

    bool is_dark_theme() const {
        return theme == ThemeDark || 
               (theme==ThemeAuto && wxSystemSettings::GetAppearance().IsDark());
    }

    static const TetrisConfig& make_default() {
        static const TetrisConfig cfg{};
        return cfg;
    }

    static TetrisConfig & make_shared() {
        static TetrisConfig cfg = make_default();
        return cfg;
    }

    static void reset_shared() {
        auto shared = make_shared(); shared = make_default();
    }

    // fallback nullptr to default instance
    static void fallback_to_default(const TetrisConfig *&cfg) {
        if(!cfg) cfg = &make_default();
    }

    // fallback nullptr to shared instance
    static void fallback_to_shared(TetrisConfig *&cfg) {
        if(!cfg) cfg = &make_shared();
    }
};

#endif //__STYLE_H__