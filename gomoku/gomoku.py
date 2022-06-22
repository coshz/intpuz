import numpy as np
from common import *
from rule import *
from record import Record


class Board:
    def __init__(self,size):
        self.size = size
        self.data = Cell.EMPTY * np.ones(shape=(size+2,size+2),dtype=np.int8)
        for i in range(0,size + 2):
            self.data[0,i] = self.data[size + 1, i] = Cell.BOUND
            self.data[i,0] = self.data[i, size + 1] = Cell.BOUND

    def loadFrom(self, record_list):
        for i in range(0, len(record_list), 2):
            self.data[record_list[i]] = Cell.BLACK
        for i in range(1, len(record_list), 2):
            self.data[record_list[i]] = Cell.WHITE

    def __getitem__(self,index):  
        return self.data[tuple(index)]

    def __setitem__(self,index,value):
        self.data[tuple(index)] = value


class Gomoku:
    def __init__(self,size=15,rule='gomoku'):
        """
        rule: 'gomoku' or 'renju'
        """
        self.board = Board(size)
        self.rule = rule
        self.state = True
        self.turn = Piece.BLACK
        self.winner = None
    
    def loadFrom(self, record, check_end = False):
        assert isinstance(record,str) or (isinstance(record,list) and len(record) > 0), "invalid record !"
        self.board = Board(15)
        record = Record.load(record) if isinstance(record, str) else record

        self.turn = Piece.BLACK if len(record) % 2 == 0 else Piece.WHITE
        self.board.loadFrom(record[:-1])
        
        if check_end:   
            self.state = self.isConnect5(record[-1])
            if not self.state:
                self.winner = self.turn.inv()
        else:
            self.state = True
            self.winner = None
        
        self.board[record[-1]] = Cell.WHITE if self.turn == Piece.BLACK else Cell.BLACK

    def go(self, x, y):
        pos = Pos([x,y])
        # self.record.append(tuple(pos))

        if self.isConnect5(pos):
            self.state = False
            self.winner = self.turn
        elif self.isForbidden(pos):
            self.state = False
            self.winner = self.turn.inv()     
        else:
            self.board[pos] = Cell.BLACK if self.turn == Piece.BLACK else Cell.WHITE
            self.turn = self.turn.inv()
            
    def isForbidden(self, pos):
        if self.rule == 'gomoku' or self.turn == Piece.WHITE:
            return False
        elif self.rule == 'renju' and self.turn == Piece.BLACK:
            patterns, _ = evalPoint(self.board, pos)
            return patterns in [Pattern.XXXXXX, Pattern.XXXX2, Pattern.XXX2]
        else:
            raise NotImplementedError

    def isConnect5(self,pos):
        for universe in Multiverse:
            contis = 1
            for v in [-1,1]:
                i = 0
                while self.board[pos + (i + 1) * universe.t * v] == self.turn:
                    i += 1
                contis += i
            if self.rule == 'gomoku':
                if  contis >= 5:
                    return True
            elif self.rule == 'renju':
                if (self.turn == Piece.BLACK and contis == 5) or \
                    (self.turn == Piece.WHITE and contis >= 5):
                    return True
            else: 
                raise NotImplementedError
        else:
            return False      



