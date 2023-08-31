#pragma once
#include <vector>
#include <array>
#include <tuple>
#include <string>
#include "def.h"

/* 
  Convert move-string to sequence of TurnAxis by
    1) pattern `(s){N}` => `ssss...s` (N times s);
    2) pattern `x2` => `xx`, `x'` => `xxx`. 
  
  Here: "=>" means "is interpreted as"; move-string obeys Singmaster notation.

  Example:
    1. U F2 D F => UFFDF
    2. (U'R2){2}F'D => UUURRUUURRFFFD 
 */
std::vector<TurnAxis> operator""_T(const char* ts, size_t n);

struct Maneuver
{
    Maneuver(const std::vector<TurnAxis> &m = {});
    Maneuver(const std::string &);

    /* 
    :style: 
        0 -- color cube
        1 -- cubies decompositon
    */
    std::string str(int style=0) const;

    size_t order() const;

    /* extract permutations and orientations of corners and edges */
    auto ceoo() const -> std::tuple<std::vector<Corner>, std::vector<unsigned>,
                                    std::vector<Edge>, std::vector<unsigned>>;
            
    std::array<Face,54> facelets;
    std::tuple<std::array<OrientedCorner,8>,std::array<OrientedEdge,12>> cubies;
};
