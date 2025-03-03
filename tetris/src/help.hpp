#ifndef __HELP_H__
#define __HELP_H__

#include <array>
#include <string>
#include <cassert>
#include <stdexcept>

namespace Utils {
    /** 
     * convert an RGB string (`#ff0000` or `0xff0000` for red) to int value,
     * with the BGR order from most to least significant byte
    */
    inline int make_bgr(std::string rgbstr) 
    {
        switch(rgbstr.length()) {
        case 6: // "ff0000"
            break;
        case 7: // "#ff0000"
            rgbstr=rgbstr.substr(1); break;
        case 8: // "0xff0000"
            rgbstr=rgbstr.substr(2); break;
        default: 
            throw std::invalid_argument("invalid RGB string");
        }

        int rgb = std::stoi(std::string(rgbstr), 0, 16);
        int bgr = ((0xff & rgb) << 16) + (0xff00 & rgb) + ((0xff0000 & rgb) >> 16);
        return bgr;
    }

    inline int operator""_bgr(const char* rgb, size_t len) {
        return make_bgr(std::string(rgb));
    }
}
#endif // __HELP_H__