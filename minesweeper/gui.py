from PySide2 import QtWidgets, QtGui, QtCore
from config import GameState


class Frame(QtWidgets.QWidget):
    def __init__(self, cfg, par=None):
        super().__init__(par)
        self.shape = cfg.shape
        self.mine = cfg.mines
        self.fsize = 50
        self.csize = 30
        self.res = cfg.res
        self.setFixedSize(
            96 + self.csize * self.shape[0], 136 + self.fsize + self.csize * self.shape[1]
        )

        self.board = Board(cfg, self)
        self.board.move(48, 88 + self.fsize)

        self.controller = Controller(QtCore.QSize(self.fsize, self.fsize), cfg.res, self)
        self.controller.move(self.size().width() / 2 - self.fsize / 2, 44)

        self.time_panel = QtWidgets.QLCDNumber(3, self)
        self.time_panel.setFixedSize(int(1.5 * self.fsize), self.fsize)
        self.time_panel.move(self.size().width() / 2 + self.fsize, 44)
        self.time = 0
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_time_panel)

        self.mine_panel = QtWidgets.QLCDNumber(3, self)
        self.mine_panel.setFixedSize(int(1.5 * self.fsize), self.fsize)
        self.mine_panel.move(self.size().width() / 2 - self.fsize - self.mine_panel.width(), 44)

        self.board.mark_click.connect(self.update_miner_panel)
        # self.board.start.connect(self.start)
        self.ready()

    def register(self, cfg):
        self.board.register(cfg)
        self.shape = cfg.shape
        self.mine = cfg.mines
        self.setFixedSize(
            96 + self.csize * self.shape[0], 136 + self.fsize + self.csize * self.shape[1]
        )
        self.controller.move(self.size().width() / 2 - self.fsize / 2, 44)
        self.time_panel.move(self.size().width() / 2 + self.fsize, 44)
        self.mine_panel.move(self.size().width() / 2 - self.fsize - self.mine_panel.width(), 44)
        self.ready()

    def ready(self):
        self.time_panel.display("000")
        self.mine_panel.display(f"{self.mine:03d}")
        self.board.reset()
        self.controller.update_state(GameState.Ready)
        self.time = 0
        self.timer.stop()

    def start(self, sp):
        self.timer.start(1000)
        self.controller.update_state(GameState.Running)

    def end(self, s, fd=None):
        self.timer.stop()
        self.controller.update_state(s)

    def paintEvent(self, event: QtGui.QPaintEvent):
        painter = QtGui.QPainter(self)
        painter.drawPixmap(0, 0, self.res.wall['lu'].scaled(48, 44))
        painter.drawPixmap(0, 44 + self.fsize, self.res.wall['lm'].scaled(48, 44))
        painter.drawPixmap(0, 44 + 44 + self.fsize + self.shape[1] * self.csize,
                           self.res.wall['ld'].scaled(48, 48))
        painter.drawPixmap(
            48 + self.shape[0] * self.csize, 0, self.res.wall['ru'].scaled(48, 44))
        painter.drawPixmap(
            48 + self.shape[0] * self.csize, 44 + self.fsize, self.res.wall['rm'].scaled(48, 44))
        painter.drawPixmap(
            48 + self.shape[0] * self.csize, 44 + 44 + self.fsize + self.shape[1] * self.csize,
            self.res.wall['rd'].scaled(48, 48))

        for i in range(self.csize * self.shape[0]):
            painter.drawPixmap(48 + i, 0, self.res.wall['xu'].scaled(1, 44))
            painter.drawPixmap(48 + i, 44 + self.fsize, self.res.wall['xm'].scaled(1, 44))
            painter.drawPixmap(48 + i, 44 + 44 + self.fsize + self.shape[1] * self.csize,
                               self.res.wall['xd'].scaled(1, 48))
        for i in range(self.csize * self.shape[1]):
            painter.drawPixmap(0, 44 + 44 + self.fsize + i, self.res.wall['ydl'].scaled(48, 1))
            painter.drawPixmap(48 + self.shape[0] * self.csize, 44 + 44 + self.fsize + i,
                               self.res.wall['ydr'].scaled(48, 1))
        for i in range(self.fsize):
            painter.drawPixmap(0, 44 + i, self.res.wall['yul'].scaled(48, 1))
            painter.drawPixmap(48 + self.shape[0] * self.csize, 44 + i, self.res.wall['yur'].scaled(48, 1))

    def update_time_panel(self):
        self.time += 1
        self.time = min(self.time, 999)
        self.time_panel.display(f"{self.time:03d}")

    def update_miner_panel(self, m):
        self.mine_panel.display(f"{m:03d}")


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
        self.mine_display = cfg.mines
        self.d = dict()  # swept
        self.m = list()  # flag
        self.b = None  # boom
        self.t = list()  # mine
        self.setFixedSize(self.shape[0] * self.csize, self.shape[1] * self.csize)

    def register(self, cfg):
        self.shape = cfg.shape
        self.mine = cfg.mines
        self.mine_display = cfg.mines
        self.setFixedSize(self.shape[0] * self.csize, self.shape[1] * self.csize)
        self.state = GameState.Ready
        self.reset()

    def paintEvent(self, event: QtGui.QPaintEvent):
        painter = QtGui.QPainter(self)
        for i in range(self.shape[0] * self.shape[1]):
            painter.drawPixmap(self.i2p(i), self.res.mark['default'].scaled(self.csize, self.csize))
        for k, v in self.d.items():
            painter.drawPixmap(self.i2p(k), self.res.number[v].scaled(self.csize, self.csize))
        for i in self.m:
            painter.drawPixmap(self.i2p(i), self.res.mark['flag'].scaled(self.csize, self.csize))
        for i in self.t:
            if i in self.m:
                painter.drawPixmap(self.i2p(i), self.res.mark['mine_1'].scaled(self.csize, self.csize))
            else:
                painter.drawPixmap(self.i2p(i), self.res.mark['mine_2'].scaled(self.csize, self.csize))
        if self.b is not None:
            painter.drawPixmap(self.i2p(self.b), self.res.mark['boom'].scaled(self.csize, self.csize))

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
            if idx not in self.m:    # marked is not allowed to dig
                self.dig_click.emit(idx)
        else:  # event.button() == QtCore.Qt.RightButton:
            if self.d:   # mark before running is not allowed.
                if idx in self.m:
                    self.m.remove(idx)
                    self.mine_display += 1
                elif self.mine_display >= 1:
                    self.m.append(idx)
                    self.mine_display -= 1
                self.mark_click.emit(self.mine_display)
                self.update()

    def reset(self):
        self.mine_display = self.mine
        self.d = dict()  # swept
        self.m = list()  # flag
        self.b = None  # boom
        self.t = list()  # mine
        self.state = GameState.Ready
        self.update()

    def response(self, s, f):
        if isinstance(f, list):  # from check
            self.t = f
        elif isinstance(f, int): # from boom
            self.b = f
        else:  # from dig
            self.d.update(f)

        self.state = s
        self.update()

    def i2p(self, idx):
        return QtCore.QPoint(idx % self.shape[0] * self.csize, idx // self.shape[0] * self.csize)


class Controller(QtWidgets.QWidget):
    ready = QtCore.Signal()
    check = QtCore.Signal()

    def __init__(self, size, res, par=None):
        super().__init__(par)
        self.state = GameState.Ready
        self.res = res
        self.setFixedSize(size)

    def update_state(self, s):
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
