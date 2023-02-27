#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__
#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include "help.hpp"
#include "model.h"

using std::cout;
using std::endl;

std::string to_string(const TetrisModel::Coord &co);
std::string to_string(const TetrisModel::Tetromino &tetro);
std::string to_string(const TetrisModel::Matrix<10,20> &matrix);

bool outscope(int x, int y, const TetrisModel::Matrix<10,20> *m);


#endif // __DEBUG_HPP__