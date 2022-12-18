from PySide2 import QtWidgets, QtGui, QtCore
from config import GameState


class Frame(QtWidgets.QWidget):
    def __init__(self, cfg, par=None):
        super().__init__(par)
        self.shape = cfg.shape
        self.mine = cfg.mines
        self.fsize = cfg.fsize
        self.csize = cfg.csize
        self.res = cfg.res
        # hard-code number, tailored to match `skin/classic.bmp`.
        self.setFixedSize(
            96 + self.csize * self.shape[0], 
            136 + self.fsize + self.csize * self.shape[1]
        )
        self.board = Board(cfg, self)
        self.board.move(48, 88 + self.fsize)
        self.panel = ControlPanel(cfg, par)
        self.panel.move(48, 44)
        self.board.mark_click.connect(self.panel.update_mine_panel)
        self.ready()

    def ready(self, cfg=None):
        self.board.ready(cfg)
        self.panel.ready(cfg)
        if cfg is not None:
            self.shape = cfg.shape
            self.mine = cfg.mines
            self.setFixedSize(
                96 + self.csize * self.shape[0], 
                136 + self.fsize + self.csize * self.shape[1]
            )
            self.update()
            
    def start(self):
        self.panel.controller.update_controller(GameState.Running)

    def end(self, s):
        self.panel.controller.update_controller(s)

    def paintEvent(self, event: QtGui.QPaintEvent):
        painter = QtGui.QPainter(self)
        painter.drawPixmap(
            0, 0, 
            self.res.wall['lu'].scaled(48, 44)
        )
        painter.drawPixmap(
            0, 44 + self.fsize, 
            self.res.wall['lm'].scaled(48, 44)
        )
        painter.drawPixmap(
            0, 44 + 44 + self.fsize + self.shape[1] * self.csize,
            self.res.wall['ld'].scaled(48, 48)
        )
        painter.drawPixmap(
            48 + self.shape[0] * self.csize, 0, 
            self.res.wall['ru'].scaled(48, 44)
        )
        painter.drawPixmap(
            48 + self.shape[0] * self.csize, 44 + self.fsize, 
            self.res.wall['rm'].scaled(48, 44)
        )
        painter.drawPixmap(
            48 + self.shape[0] * self.csize, 
            44 + 44 + self.fsize + self.shape[1] * self.csize,
            self.res.wall['rd'].scaled(48, 48)
        )

        for i in range(self.csize * self.shape[0]):
            painter.drawPixmap(48 + i, 0, self.res.wall['xu'].scaled(1, 44))
            painter.drawPixmap(48 + i, 44 + self.fsize, self.res.wall['xm'].scaled(1, 44))
            painter.drawPixmap(
                48 + i, 44 + 44 + self.fsize + self.shape[1] * self.csize,
                self.res.wall['xd'].scaled(1, 48)
            )
        for i in range(self.csize * self.shape[1]):
            painter.drawPixmap(0, 44 + 44 + self.fsize + i, self.res.wall['ydl'].scaled(48, 1))
            painter.drawPixmap(48 + self.shape[0] * self.csize, 44 + 44 + self.fsize + i,
                                self.res.wall['ydr'].scaled(48, 1))
        for i in range(self.fsize):
            painter.drawPixmap(0, 44 + i, self.res.wall['yul'].scaled(48, 1))
            painter.drawPixmap(48 + self.shape[0] * self.csize, 44 + i, 
                                self.res.wall['yur'].scaled(48, 1))


class Board(QtWidgets.QWidget):
    dig_click = QtCore.Signal(int)  # send to model
    mark_click = QtCore.Signal(int)  # send to frame
    start = QtCore.Signal(int)  # send to minesweeper

    def __init__(self, cfg, par=None):
        super().__init__(par)
        self.shape = cfg.shape
        self.mine = cfg.mines
        self.csize = 30
        self.res = cfg.res
        self.state = GameState.Ready
        self.d = dict()  # dug
        self.m = list()  # marked
        self.b = None    # boom
        self.t = list()  # true-mine
        self.setFixedSize(self.shape[0] * self.csize, self.shape[1] * self.csize)

    def ready(self, cfg=None):
        self.d = dict()  
        self.m = list()  
        self.b = None  
        self.t = list()  
        self.state = GameState.Ready
        if cfg is not None:
            self.shape = cfg.shape
            self.mine = cfg.mines
            self.setFixedSize(self.shape[0] * self.csize, self.shape[1] * self.csize)
        self.update()

    def paintEvent(self, event: QtGui.QPaintEvent):
        painter = QtGui.QPainter(self)
        for i in range(self.shape[0] * self.shape[1]):
            painter.drawPixmap(
                self.i2p(i), 
                self.res.mark['default'].scaled(self.csize, self.csize)
            )
        for k, v in self.d.items():
            painter.drawPixmap(
                self.i2p(k), 
                self.res.number[v].scaled(self.csize, self.csize)
            )
        for i in self.t:
            painter.drawPixmap(
                    self.i2p(i), 
                    self.res.mark['mine_1'].scaled(self.csize, self.csize)
            )
        for i in self.m:
            painter.drawPixmap(
                    self.i2p(i), 
                    self.res.mark['flag'].scaled(self.csize, self.csize)
            )
        # endgame
        if self.state != GameState.Running:
            A, B = set(self.m), set(self.t)
            # true_mark = set.intersection(A, B)
            fasle_mark = set.difference(A, B)
            for i in fasle_mark:
                painter.drawPixmap(
                    self.i2p(i), 
                    self.res.mark['mine_2'].scaled(self.csize, self.csize)
                )
            if self.b is not None:
                painter.drawPixmap(
                    self.i2p(self.b), 
                    self.res.mark['boom'].scaled(self.csize, self.csize)
                )

    def mouseReleaseEvent(self, event: QtGui.QMouseEvent):
        if self.state != GameState.Ready and self.state != GameState.Running:
            return
        pos = event.pos()
        x = int(pos.x() / self.csize)
        y = int(pos.y() / self.csize)
        idx = x + self.shape[0] * y
        limit = 0.15 * self.csize
        # idx is dug already or pos not in the scope, skip
        if idx in self.d.keys() \
                or pos.x() - x * self.csize < limit \
                or (x + 1) * self.csize - pos.x() < limit \
                or pos.y() - y * self.csize < limit \
                or (y + 1) * self.csize - pos.y() < limit:
            return
        if len(self.d) == 0:  # game start
            self.state = GameState.Running
            self.start.emit(idx)
        if event.button() == QtCore.Qt.LeftButton:
            if idx not in self.m:  # marked is not allowed to dig
                self.dig_click.emit(idx)
        else:  # event.button() == QtCore.Qt.RightButton:
            if self.d:  # mark before running is not allowed.
                if idx in self.m:
                    self.m.remove(idx)
                elif len(self.m) < self.mine:
                    self.m.append(idx)
                    self.mark_click.emit(self.mine - len(self.m))
                self.update()

    def response(self, state, feedback):
        """process dig/check response"""
        if state == GameState.Running or state == GameState.Win:
            self.d.update(feedback)
        elif state == GameState.Check:
            self.t = feedback
        else:  # state == GameState.Lose:
            self.t = feedback[0]
            self.b = feedback[1]
        self.state = state
        self.update()

    def i2p(self, idx):
        return QtCore.QPoint(
            idx % self.shape[0] * self.csize, 
            idx // self.shape[0] * self.csize
        )


class ControlPanel(QtWidgets.QWidget):
    def __init__(self, cfg, parent: None):
        super().__init__(parent)
        
        self.controller = Controller(cfg, self)
        self.time = 0
        self.time_panel = QtWidgets.QLCDNumber(3, self)
        self.mine = cfg.mines
        self.mine_panel = QtWidgets.QLCDNumber(3, self)

        self.setFixedSize(cfg.shape[0] * cfg.csize, cfg.fsize)
        self.time_panel.setFixedSize(2 * cfg.fsize, cfg.fsize)
        self.mine_panel.setFixedSize(2 * cfg.fsize, cfg.fsize)
        self.controller.move(self.width() / 2 - cfg.fsize / 2, 0)
        self.time_panel.move(self.width() - self.time_panel.width() - 5, 0)
        self.mine_panel.move(5, 0)
        
        self.ready()
    
    def ready(self, cfg=None):
        if cfg is not None:
            self.mine = cfg.mines
            self.setFixedSize(cfg.shape[0] * cfg.csize, cfg.fsize)
            self.time_panel.setFixedSize(2 * cfg.fsize, cfg.fsize)
            self.mine_panel.setFixedSize(2 * cfg.fsize, cfg.fsize)
            self.controller.move(self.width() / 2 - cfg.fsize / 2, 0)
            self.time_panel.move(self.width() - self.time_panel.width() - 5, 0)
            self.mine_panel.move(5, 0)
            self.update()
        self.time_panel.display(f"{0:03d}")
        self.mine_panel.display(f"{self.mine:03d}")
        self.controller.update_controller(GameState.Ready)
        self.time = 0

    def update_time_panel(self):
        self.time += 1
        self.time = min(self.time, 999)
        self.time_panel.display(f"{self.time:03d}")

    def update_mine_panel(self, m):
        self.mine_panel.display(f"{m:03d}")


class Controller(QtWidgets.QWidget):
    ready = QtCore.Signal()
    check = QtCore.Signal()

    def __init__(self, cfg, par=None):
        super().__init__(par)
        self.state = GameState.Ready
        self.res = cfg.res
        self.setFixedSize(cfg.fsize, cfg.fsize)
        self.checkable = False

    def update_controller(self, s):
        self.state = s
        self.update()

    def paintEvent(self, event: QtGui.QPaintEvent):
        painter = QtGui.QPainter(self)
        painter.drawPixmap(0, 0, self.res.state[str(self.state)].scaled(self.size()))

    def mouseReleaseEvent(self, event: QtGui.QMouseEvent):
        if event.button() == QtCore.Qt.RightButton:
            self.check.emit()

    def mouseDoubleClickEvent(self, event: QtGui.QMouseEvent):
        self.ready.emit()
