#include "debug.h"
#include <iomanip>

std::string to_string(const TetrisModel::Coord &co)
{
    std::stringstream ss;
    ss << "[(" << co[0][0] << "," << co[0][1] <<")" << ";"
       << "(" << co[1][0] << "," << co[1][1] <<")" << ";"
       << "(" << co[2][0] << "," << co[2][1] <<")" << ";"
       << "(" << co[3][0] << "," << co[3][1] <<")" << "]";
    return ss.str();
}

std::string to_string(const TetrisModel::Tetromino &tetro) 
{
    std::stringstream ss;
    ss << "[ shape, orient, base ] = [ " 
        << tetro.getShape() << ", " << tetro.getOrient() << ", " 
        << tetro.getBase()[0] << "--" << tetro.getBase()[1] << " ]";
    return ss.str();
}

std::string to_string(const TetrisModel::Matrix<10,20> &matrix) 
{
    std::stringstream ss;
    for(auto j = matrix.height - 1; j >= 0; j--)
    {
        for(auto i = 0; i < matrix.width; i++)
        {
            ss << std::setw(3) << matrix.getShapeAt(i,j) << "  ";
        }
        ss << std::endl;
    }
    return ss.str();
}

bool outscope(int x, int y, const TetrisModel::Matrix<10,20> *m)
{
    return x < 0 || x >= m->width || y < 0 || y >= m->height;
}
