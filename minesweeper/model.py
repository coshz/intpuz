import numpy as np
from config import GameState


class Board(np.ndarray):
    pass


class MinesweeperModel:
    def __init__(self, shape, n):
        self.shape = shape
        self.n_mine = n
        self.board = None
        self.remain = shape[0] * shape[1] - n
        self.state_ = GameState.Ready

    def register(self, cfg):
        self.shape = cfg.shape
        self.n_mine = cfg.mines
        self.remain = cfg.shape[0] * cfg.shape[1] - cfg.mines
        self.state_ = GameState.Ready

    def start(self, start_point=None):
        # L0: is_dug; L1: is_mine, L2: num_mine of neighbors
        size = self.shape[0] * self.shape[1]
        b = np.zeros((size, 3), np.byte).view(Board)
        all_pos = list(range(size))
        if start_point is not None:
            all_pos.pop(start_point)
            b[start_point, 0] = 1
        mine_pos = np.random.choice(all_pos, self.n_mine, replace=False)
        for pos in mine_pos:
            b[pos, 1] = 1
        for i in range(size):
            b[i, 2] = np.sum(b[self.neighbor(i), 1])

        self.board = b
        self.remain = size - self.n_mine
        self.state = GameState.Running

    def sweep(self, pos):
        self.board[pos, 0] = 1
        self.remain -= 1
        return {pos: self.board[pos, 2]}

    def dig(self, pos):
        # return all swept pos and num of mine within neighbor in this step
        if self.is_mine(pos):
            self.state = GameState.Lose
            return pos
        else:
            fd = self.sweep(pos)
            if self.is_clean(pos):
                for p in self.neighbor(pos):
                    if self.is_dug(p):
                        continue
                    d = self.dig(p)
                    fd.update(d)
            if self.remain == 0:
                self.state = GameState.Win
            return fd

    def peek(self):
        self.state = GameState.Check
        return list(self.board[:, 1].nonzero()[0])

    @property
    def state(self):
        return self.state_

    @state.setter
    def state(self, s):
        # return self.state, self.peek() if state == GameState.Check else None
        # feedback = self.peek() if state == GameState.Check else None
        self.state_ = s

    def neighbor(self, idx):
        neighbors = list()
        x, y = idx % self.shape[0], idx // self.shape[0]
        tmp_p = [(x - 1, y - 1), (x - 1, y), (x - 1, y + 1),
                 (x, y - 1), (x, y + 1),
                 (x + 1, y - 1), (x + 1, y), (x + 1, y + 1)]
        for _x, _y in tmp_p:
            if 0 <= _x < self.shape[0] and 0 <= _y < self.shape[1]:
                neighbors.append(_x + _y * self.shape[0])
        return neighbors

    def is_mine(self, pos):
        return self.board[pos, 1] == 1

    def is_dug(self, pos):
        return self.board[pos, 0] == 1

    def is_clean(self, pos):
        return self.board[pos, 2] == 0
