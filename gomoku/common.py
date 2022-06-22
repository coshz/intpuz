from http.client import FORBIDDEN
import numpy as np
from enum import IntEnum, Enum

__all__ = ['Pos', 'Universe', 'Cell', 'Piece', 'Multiverse', 's2p', 'p2s']

class Point(IntEnum):
    def __init__(self):
        self.forbidden = False
    
class Pos(np.ndarray):

    def __new__(cls, v, *args, **kwargs):
        obj = np.asarray(v)
        return obj


class Universe:
    def __init__(self, vec_t):
        self.t = vec_t

Multiverse = [
	Universe(np.array([1,0],np.int8)),
	Universe(np.array([0,1],np.int8)),
	Universe(np.array([1,1],np.int8)),
	Universe(np.array([1,-1],np.int8))
]

class Cell(IntEnum):
    BLACK = 1
    WHITE = 2
    EMPTY = 0
    BOUND = -1


class Piece(IntEnum):
    BLACK = 1,
    WHITE = 2,
    def inv(self):
        return Piece.WHITE if self == Piece.BLACK else Piece.BLACK

def p2s(p):
    x, y = p
    return str(x) + chr(96 + y)

def s2p(s):
    sx, sy = s[:-1], s[-1]
    return int(sx), ord(sy) - 96


