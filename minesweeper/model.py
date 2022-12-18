import numpy as np
from config import GameState


class Board(np.ndarray):
    pass


class MinesweeperModel:
    def __init__(self, shape, num_mines):
        self.shape = shape
        self.mine = num_mines
        # board, L0: is_dug; L1: is_mine, L2: num_mine of neighbors
        self.board = np.zeros((shape[0] * shape[1], 3), np.byte).view(Board)
        self.remain = shape[0] * shape[1] - num_mines
        self.state = GameState.Ready

    def start(self, start_point=None):
        size = self.shape[0] * self.shape[1]
        all_pos = list(range(size))
        if start_point is not None:
            all_pos.pop(start_point)
            self.board[start_point, 0] = 1
        mine_pos = np.random.choice(all_pos, self.mine, replace=False)
        for pos in mine_pos:
            self.board[pos, 1] = 1
        for i in range(size):
            self.board[i, 2] = np.sum(self.board[self.neighbor(i), 1])

        self.remain = size - self.mine
        self.state = GameState.Running

    def go(self, pos=None, is_check=False):
        """Interface method

        Args:
            pos: pos index to dig;
            is_check: is check request or not;
        Returns: 
            model state and feedback data.
        """
        # assert(self.state == GameState.Running)
        if is_check:
            self.state = GameState.Check
            return self.state, list(self.peek(1))
        else:  # assert(pos is not None)
            if self.is_mine(pos):
                self.state = GameState.Lose
                return self.state, (list(self.peek(1)), pos)
            else:
                sd = self.dig_(pos)
                if self.remain == 0:
                    self.state = GameState.Win
                return self.state, dict({p: self.board[p,2] for p in sd})

    def dig_(self, pos):
        """return dug pos(s) in this step"""
        self.board[pos, 0] = 1
        self.remain -= 1
        step_dug = [pos]
        if self.is_clean(pos):
            for p in self.neighbor(pos):
                if self.is_dug(p):
                    continue
                step_dug.extend(self.dig_(p))
        return step_dug

    def peek(self, i):
        """return all index of: 0 for dug pos; 1 for mine pos"""
        return list(self.board[:, i].nonzero()[0])

    def neighbor(self, idx):
        x, y = idx % self.shape[0], idx // self.shape[0]
        tem_p = [(x - 1, y - 1), (x - 1, y), (x - 1, y + 1),
                 (x, y - 1), (x, y + 1),
                 (x + 1, y - 1), (x + 1, y), (x + 1, y + 1)]
        neighbors = [_x + _y * self.shape[0] for _x, _y in tem_p if \
                    0 <= _x < self.shape[0] and 0 <= _y < self.shape[1]]
        return neighbors

    def is_mine(self, pos):
        return self.board[pos, 1] == 1

    def is_dug(self, pos):
        return self.board[pos, 0] == 1

    def is_clean(self, pos):
        return self.board[pos, 2] == 0
