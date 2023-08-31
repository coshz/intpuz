#include <iostream>
#include <stack>
#include "maneuver.h"
#include "help.hpp"

const char* CornerToString[8]       = { "urf", "ufl", "ulb", "ubr", "dfr", "dlf", "dbl", "drb" };
const char* EdgeToString[12]        = { "ur","uf","ul","ub","dr","df","dl","db","fr","fl","bl","br" };
const char* CenterToStringp[6]      = { "u", "r", "f", "d", "l", "b" };
const char* OrientationToString[]   = { "", "+", "-" };
const char* ColorSet                = "URFDLB" ;

std::vector<TurnAxis> str2vecTA(const char* ts, size_t n)
{
    auto repeat_substr = [](std::string s){
        auto repeat_str = [](const std::string &input, size_t n){
            std::string res = "";
            for(size_t i = 0; i < n; i++) res += input;
            return res;
        };
        std::stack<size_t> stack;
        for(size_t i = 0; i < s.length(); i++){
            if(s[i] == '(') {
                stack.push(i);
            } else if(s[i] == ')') {
                if(stack.empty()) return s;
                size_t lparen = stack.top(); stack.pop();
                size_t lbrace = s.find('{', lparen);
                if(lbrace != std::string::npos && lbrace > i){
                    size_t rbrace = s.find('}', lbrace);
                    if(rbrace != std::string::npos && rbrace > lbrace){
                        auto sub = s.substr(lparen + 1, i - lparen - 1);
                        auto num = std::stoi(s.substr(lbrace + 1, rbrace - lbrace - 1));
                        auto rep_str = repeat_str(sub, num);
                        s.replace(lparen, rbrace - lparen + 1, rep_str);
                        i = lparen + rep_str.length() - 1;
                    }
                }
            }
        }
        return s;
    };

    auto match_move = [](const char ch) -> int {
        const char M[] = "URFDLB";
        for(int i = 0; i < 6; i++) if(M[i] == ch) return i;
        return -1;
    };

    std::string ts_new = repeat_substr(ts);
    struct Mn { int m, n; };
    std::vector<Mn> xs;
    Mn x{-1,0};
    int memory = -1;
    for(size_t i = 0; i < ts_new.length(); i++)
    {
        if(-1 == match_move(ts_new[i])) continue;
        if(memory == match_move(ts_new[i])) {
            x.n += 1;
        } else {
            if(-1 != memory) xs.push_back(x);
            memory = match_move(ts_new[i]);
            x = Mn{memory, 1};
        }
        switch(ts_new[i+1])
        {
            case '\'': 
            case '3':   x.n += 2; break;
            case '2':   x.n += 1; break;
            default:    break;
        }
    }
    if(-1 != memory) xs.push_back(x);

    std::vector<TurnAxis> ms;
    for(auto &x: xs) 
    for(auto l = 0; l < x.n % 4; l++) ms.push_back((TurnAxis)x.m);
    return ms;
}

std::vector<TurnAxis> operator""_T(const char* ts, size_t n)
{
   return str2vecTA(ts, n);
}

Maneuver::Maneuver(const std::string &s):Maneuver(str2vecTA(s.c_str(), s.length())){}
Maneuver::Maneuver(const std::vector<TurnAxis> &ms)
{
    // default: identity
    if(ms.size() == 0) {
        for(size_t i = 0; i < 54; i++) facelets[i] = (Face) i;
        for(size_t i = 0; i < 8; i++) std::get<0>(cubies)[i] = OrientedCorner{(Corner)i,0};
        for(size_t i = 0; i < 12; i++) std::get<1>(cubies)[i] = OrientedEdge{(Edge)i,0};
        return;
    }

    // facelets
    for(int i = 0; i < 54; i++) {
        int j = i;
        for(auto &m: ms) { j = FaceletMove[m][j]; }
        facelets[i] = (Face) j;
    }

    // cubies
    for(unsigned i = 0; i < 8; i++){
        unsigned j = i, o = 0;
        for(auto rit = ms.rbegin(); rit != ms.rend(); ++rit) {
            o = (o + CornerCubieMove[*rit][j].o) % 3, 
            j = CornerCubieMove[*rit][j].c;
        }
        std::get<0>(cubies)[i] = OrientedCorner{(Corner)j, o};
    }

    for(unsigned i = 0; i < 12; i++){
        unsigned j = i, o = 0;
        for(auto rit = ms.rbegin(); rit != ms.rend(); ++rit) {
            o = (o + EdgeCubieMove[*rit][j].o) % 2,
            j = EdgeCubieMove[*rit][j].e;
        }
        std::get<1>(cubies)[i] = OrientedEdge{(Edge)j, o%2};
    }
}

size_t Maneuver::order() const
{
    static size_t o = orderOf(facelets);
    return o;
}

std::string Maneuver::str(int style) const
{
    switch(style) {
    // case 0: // vec of facelets;  
    //     return seq2str(facelets,"[","]",",");
    // case 1: // vec of cubies {
    //     auto ceoo = ceoo_extractor();
    //     return seq2str(std::get<0>(ceoo),"[","]",",") + seq2str(std::get<1>(ceoo),"[","]",",")
    //             + " -- " + seq2str(std::get<2>(ceoo)) + " " + seq2str(std::get<3>(ceoo));
    // }
    // case 2: cycle decompostion of vec of facelets {
    //     auto cycles = decomposite(facelets);
    //     if(cycles.size() == 1) return std::string("I");
    //     std::string s = "";
    //     // bypass fix-points
    //     for(auto i = 1; i < cycles.size(); i++) {
    //         s += seq2str(cycles[i], "(",")",",");
    //     }
    //     return s;
    // }
    case 0: // color cube
    {
        std::string str_cube(54,' ');
        for(size_t i = 0; i < 54; i++){ str_cube[facelets[i]] = ColorSet[i/9]; }
        return str_cube;
    }
    case 1: // cycle decomposition of vec of cubies
    {
        auto ceoo_extractor = [this]() {
            std::vector<Corner> cp(8);
            std::vector<Edge> ep(12);
            std::vector<unsigned> co(8);
            std::vector<unsigned> eo(12);
            for(size_t i = 0; i < 8; i++) cp[i] = std::get<0>(cubies)[i].c;
            for(size_t i = 0; i < 12; i++) ep[i] = std::get<1>(cubies)[i].e;
            for(size_t i = 0; i < 8; i++) co[i] = std::get<0>(cubies)[i].o;
            for(size_t i = 0; i < 12; i++) eo[i] = std::get<1>(cubies)[i].o;
            return make_tuple(cp,ep,co,eo);
        };
        auto str_corners_or_edges = [this](const auto &xs, int ce){
            std::vector<std::string> rs(xs.size());
            for(size_t i = 0; i < rs.size(); i++){
                rs[i] = (ce == 0) 
                    ? std::string(OrientationToString[std::get<0>(cubies)[xs[i]].o]) 
                      + CornerToString[xs[i]]
                    : std::string(OrientationToString[std::get<1>(cubies)[xs[i]].o]) 
                      + EdgeToString[xs[i]];
            }
            return rs;
        };
        std::string s1 = "", s2 = "";
        auto ceoo = ceoo_extractor();
        auto cycles_corner = decomposite(std::get<0>(ceoo));
        auto cycles_edge = decomposite(std::get<1>(ceoo));
        for(size_t i = 1; i < cycles_corner.size(); i++){
            s1 += seq2str(str_corners_or_edges(cycles_corner[i], 0),"(",")",",");
        }
        for(auto &c: cycles_corner[0]) {
            // bypass location-orientaton double fixed points
            if(std::get<0>(cubies)[c].o != 0)
            s1 += seq2str(str_corners_or_edges(std::vector<Corner>{c}, 0),"(",")",",");
        }
        for(size_t i =  1; i < cycles_edge.size(); i++){
            s2 += seq2str(str_corners_or_edges(cycles_edge[i], 1),"(",")",",");
        }
        for(auto &e: cycles_edge[0]) {
            if(std::get<1>(cubies)[e].o != 0) 
            s2 += seq2str(str_corners_or_edges(std::vector<Edge>{e}, 1),"(",")",",");
        }
        if(s1 == "" && s2 == "") return std::string("I");
        else return s1 + s2;
    }
    }
    return std::string("");
}

auto Maneuver::ceoo() const -> std::tuple<
    std::vector<Corner>, std::vector<unsigned>,std::vector<Edge>, std::vector<unsigned>>
{
    std::vector<Corner> cp(8);
    std::vector<Edge> ep(12);
    std::vector<unsigned> co(8);
    std::vector<unsigned> eo(12);
    for(size_t i = 0; i < 8; i++) cp[i] = std::get<0>(cubies)[i].c;
    for(size_t i = 0; i < 12; i++) ep[i] = std::get<1>(cubies)[i].e;
    for(size_t i = 0; i < 8; i++) co[i] = std::get<0>(cubies)[i].o;
    for(size_t i = 0; i < 12; i++) eo[i] = std::get<1>(cubies)[i].o;
    return make_tuple(cp,co,ep,eo);
}