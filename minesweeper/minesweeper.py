from PySide2 import QtWidgets, QtCore
from config import GameState
from gui import Frame
from config import GameState
from model import MinesweeperModel


class Minesweeper(QtWidgets.QWidget):
    ready_sig = QtCore.Signal()
    start_sig = QtCore.Signal()
    end_sig = QtCore.Signal(GameState)

    def __init__(self, cfg, par=None):
        super().__init__(par)
        self.cfg = cfg
        self.frame = Frame(cfg, self)
        self.model = MinesweeperModel(cfg.shape, cfg.mines)

        self.frame.panel.controller.ready.connect(self.ready)
        self.frame.board.start.connect(self.start)
        self.frame.board.dig_click.connect(self.dig)
        self.frame.panel.controller.check.connect(self.check)
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.frame.panel.update_time_panel)

        self.setFixedSize(self.frame.size())

    def ready(self, cfg=None):
        self.cfg = cfg if cfg is not None else self.cfg
        self.timer.stop()
        self.frame.ready(cfg)
        self.model = MinesweeperModel(self.cfg.shape, self.cfg.mines)
        self.update()
        self.ready_sig.emit()
        self.setFixedSize(self.frame.size())

    def start(self, sp):
        self.timer.start(1000)
        self.model.start(sp)
        self.frame.start()
        self.start_sig.emit()

    def dig(self, pos):
        # assert(self.state == GameState.Running)
        s, f = self.model.go(pos)
        self.frame.board.response(s, f)
        if s != GameState.Running:
            self.end(s)

    def check(self):
        if len(self.frame.board.m) != self.model.mine:
            return
        s, f = self.model.go(is_check=True)
        self.frame.board.response(s, f)
        # end state: GameState.Check -> Union(GameState.Win, GameState.Lose)
        self.end(GameState.Win if set(f) == set(self.frame.board.m) else GameState.Lose)

    def end(self, s):
        self.timer.stop()
        self.frame.end(s)
        self.end_sig.emit(s)