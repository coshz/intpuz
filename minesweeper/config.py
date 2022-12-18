from enum import Enum


class Config:
    mode = 0
    params = [(9, 9, 10), (16, 16, 40), (30, 16, 99)]
    shape = params[mode][:2]
    mines = params[mode][2]
    res = "skin/classic.bmp"
    csize = 30
    fsize = 50

    def then(self, res_class):
        self.res = res_class(self.res)
        return self


class GameState(Enum):
    Ready = 0
    Running = 1
    Win = 2
    Lose = 3
    Check = 4

    def __str__(self):
        return ['ready', 'running', 'win', 'loss', 'check'][self.value]
