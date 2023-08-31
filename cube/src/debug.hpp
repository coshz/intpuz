/* C++20 required */
#pragma once
#include <sstream>
#include <format>
#include "cube.h"
#include "coord.h"
#include "help.hpp"

std::string toString(const Coord &c);
std::string toString(const CubieCube &cc);


std::string toString(const Coord &c)
{
    return std::format("(twist={},flip={},slice={},corner={},edge4={},edge8={})",
        c.twist, c.flip, c.slice, c.corner, c.edge4, c.edge8);
}

std::string toString(const CubieCube &cc)
{
    return std::format("{\n  cp={},\n  co={},\n  ep={}\n  eo={}\n}",
        seq2str(cc.cp,"[","]",","),
        seq2str(cc.co,"(",")",","),
        seq2str(cc.ep,"[","]",","),
        seq2str(cc.eo,"(",")",",")
    );
}