#pragma once
#include <string>
#include "twophase.h"

inline bool is_valid_cube(const char *cube)
{
    return isValidConfig(std::string(cube));
}

struct CubeSolver
{
    /* solve cube from `src` to `tgt` in max_length steps if possible */    
    static std::string solve(std::string src, std::string tgt=cid, int max_length=20);
    static constexpr const char *cid = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
};

