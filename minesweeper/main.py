from PySide2.QtWidgets import QApplication, QMainWindow
from PySide2 import QtWidgets, QtCore
from gui import Frame
from config import *
from res import Res
from model import MinesweeperModel


class MainWindow(QMainWindow):
    def __init__(self, cfg=None):
        super().__init__()
        self.cfg = cfg
        self.game = Minesweeper(cfg, self)
        self.statusbar = QtWidgets.QStatusBar(self)
        self.setStatusBar(self.statusbar)
        self.setWindowTitle("Minesweeper")

        self.menubar = QtWidgets.QMenuBar(self)

        game = QtWidgets.QMenu("Game")
        new_action = QtWidgets.QAction("New", game)
        new_action.triggered.connect(self.new)
        level = QtWidgets.QMenu("Leval", game)
        level.addAction("Beginner")
        level.addAction("Intermediate")
        level.addAction("Expert")
        level.addAction("Customized")

        # print(level.actions()[0].text())
        level.triggered.connect(self.choose_level)
        game.addAction(new_action)
        game.addMenu(level)

        view = QtWidgets.QMenu("View")
        sb_action = QtWidgets.QAction("Status bar", view)
        sb_action.setCheckable(True)
        sb_action.setChecked(True)
        sb_action.toggled.connect(self.toggle_statusbar)
        view.addAction(sb_action )

        self.menubar.addMenu(game)
        self.menubar.addMenu(view)
        self.setMenuBar(self.menubar)

        self.game.ready_sig.connect(self.ready)
        self.game.start_sig.connect(self.start)
        self.game.end_sig.connect(self.end)

        self.setFixedWidth(self.game.width())
        self.height_1 = self.game.frame.height()
        self.height_2 = self.height_1 + self.statusbar.height()
        self.setFixedHeight(self.height_2)

        self.toggle_statusbar(False)

    def choose_level(self, l):
        if l.text() == "Beginner":
            self.cfg.shape = (9, 9)
            self.cfg.mines = 10
        elif l.text() == "Intermediate":
            self.cfg.shape = (16, 16)
            self.cfg.mines = 40
        else:  # Expert
            self.cfg.shape = (30, 16)
            self.cfg.mines = 99
     
        self.game.register(self.cfg)
        self.setFixedWidth(self.game.width())
        self.setFixedHeight(self.game.frame.height() if not self.statusbar.isVisible() else \
                            self.game.frame.height() + self.statusbar.height())

    def ready(self):
        self.statusbar.showMessage("Ready", 0)

    def start(self):
        self.statusbar.showMessage("Running", 0)

    def end(self, s):
        self.statusbar.showMessage(str(s).capitalize(), 0)

    def new(self):
        self.game.ready()

    def toggle_statusbar(self, visible):
        self.statusbar.setVisible(visible)
        self.setFixedHeight(self.height_2 if visible else self.height_1)
        self.update()


class Minesweeper(QtWidgets.QWidget):
    ready_sig = QtCore.Signal()
    start_sig = QtCore.Signal()
    end_sig = QtCore.Signal(GameState)

    def __init__(self, cfg, par=None):
        super().__init__(par)
        self.cfg = cfg
        # self.state = GameState.Ready
        self.frame = Frame(cfg, self)

        self.model = MinesweeperModel(cfg.shape, cfg.mines)

        self.frame.board.start.connect(self.start)
        self.frame.board.dig_click.connect(self.dig)
        self.frame.controller.ready.connect(self.ready)
        self.frame.controller.check.connect(self.check)

        self.setFixedSize(self.frame.size())

    def register(self, cfg):
        self.frame.register(cfg)
        self.model.register(cfg)
        self.setFixedSize(self.frame.size())

    def ready(self):
        self.frame.ready()
        self.ready_sig.emit()

    def start(self, sp):
        self.model.start(sp)
        self.frame.start(sp)
        self.start_sig.emit()

    def dig(self, pos):
        # assert(self.state == GameState.Running)
        f = self.model.dig(pos)
        s = self.model.state

        self.frame.board.response(s, f)
        if s != GameState.Running:
            self.end(s)

    def check(self):
        if len(self.frame.board.m) != self.model.n_mine:
            return
        feedback = self.model.peek()
        state = GameState.Win if set(feedback) == set(self.frame.board.m) else GameState.Lose
        self.frame.board.response(state, feedback)
        self.end(state)

    def end(self, s):
        self.frame.end(s)
        self.end_sig.emit(s)


def main():
    app = QApplication()
    cfg = Config().then(Res)
    w = MainWindow(cfg)
    w.show()
    app.exec_()


if __name__ == '__main__':
    main()

