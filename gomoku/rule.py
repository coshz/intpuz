# renju rule implement

from enum import IntEnum,Enum
from common import *
import copy

all = ['Shape', 'Pattern', 'evalPoint']


class Shape(IntEnum):
    XXXXXX = 1,
    XXXXX = 2,
    XXXX = 3,
    XXXXO = 4,
    XXX = 5,
    XX = 6,
    OTHER = 7


class Pattern(IntEnum):
    XXXXXX = 1
    XXXXX = 2
    XXXX2 = 3
    XXX2 = 4
    OTHER = 5


def evalPoint(board, pos):
    assert board[pos] == Cell.EMPTY, f'Position {pos} is not EMPTY'
    shapes = []
    pattern = None
    for universe in Multiverse:
        # contis: 1~9, breaks: 0~2, ends: 0~2
        ncontis = 1
        nends = 0
        breaks = []
        emptys = []
        shape = None
        for v in [-1,1]: 
            i = 0
            while True:
                i += 1
                _pos = pos + i * universe.t * v
                _cell = board[_pos]
                if  _cell == Cell.BLACK:
                    ncontis += 1
                elif _cell in [Cell.BOUND, Cell.WHITE]:
                    nends += 1
                    break
                elif _cell == Cell.EMPTY:
                    if len(breaks) >= 1:
                        break
                    elif board[pos + (i+1) * universe.t * v] == Cell.BLACK:
                        breaks.append(_pos)
                    else:
                        emptys.append(_pos)
                        break

        if ncontis > 5 and len(breaks) == 0:
            shape = Shape.XXXXXX
        elif ncontis == 5 and len(breaks) == 0:
            shape = Shape.XXXXX
        elif ncontis == 4:
            if len(breaks) == 0 and nends == 0:
                shape = Shape.XXXX
            elif len(breaks) == 1 or (nends == 1 and len(breaks) == 0):
                shape = Shape.XXXXO 
        elif ncontis == 3 and nends == 0 and len(breaks)<= 1 :
            _board = copy.deepcopy(board)
            _board[pos] = Cell.BLACK
            
            if len(breaks) == 1:
                new_pos = breaks[0]
                _pattern, _shapes = evalPoint(_board, new_pos)
                if _pattern not in [Pattern.XXXXXX, Pattern.XXXX2, Pattern.XXX2]:
                    shape = Shape.XXX
                else:
                    shape = Shape.OTHER
            else:
                for new_pos in emptys:
                    _pattern, _shapes = evalPoint(_board, new_pos)
                    _scale = max([abs(new_pos[i] - pos[i]) for i in [0,1]])
                    _vec_t = [int((new_pos[i] - pos[i])/_scale) for i in [0,1]]
                    if _board[new_pos + _vec_t * 1] == Cell.EMPTY and _pattern not in [Pattern.XXXXXX, Pattern.XXXX2, Pattern.XXX2]:
                        shape = Shape.XXX
                        break
                else:
                    shape = Shape.OTHER           
        else:
            shape = Shape.OTHER

        shapes.append(shape)
    
    if Shape.XXXXX in shapes:
        pattern = Pattern.XXXXX
    elif Shape.XXXXXX in shapes:
        pattern = Pattern.XXXXXX
    elif len([shape for shape in shapes if shape in [Shape.XXXX, Shape.XXXXO]]) >= 2:
        pattern = Pattern.XXXX2
    elif len([1 for shape in shapes if shape == Shape.XXX]) >= 2:
        pattern = Pattern.XXX2
    else:
        pattern = Pattern.OTHER

    return pattern, shapes

    