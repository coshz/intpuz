from common import *
from rule import *


class Player:
    def __init__(self, game, role):
        self.game = game
        self.piece = role
    
    def go(self, pos):
        self.game.go(pos)

    def isForbidden(self, pos)->bool:
        if self.game.rule == 'gomoku' or self.piece == Piece.WHITE:
            return False
        elif self.rule == 'renju' and self.piece == Piece.BLACK:
            patterns, _ = evalPoint(pos, self.game.board)
            return patterns in [Pattern.XXXXXX, Pattern.XXXX2, Pattern.XXX2]
        else:
            raise NotImplemented
    
    def findForbidden(self):
        all_pos = [Pos([i,j]) for i in range(1, self.board.size + 1) for j in range(1, self.board.size + 1)]
        all_forbidden = []
        for pos in all_pos:
            if evalPoint(pos, self.board):
                all_forbidden.append(pos)
        return all_forbidden


class PlayerAI(Player):
    def __init__(self, game, role):
        self.super().__init__(game, role)
    
    


