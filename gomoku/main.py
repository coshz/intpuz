import sys
from time import time
from PySide6 import QtCore, QtWidgets, QtGui
from PySide6.QtCore import Qt
from help import Config
from gui import BoardWidget, PlayControlWidget, RecorderWidget
from gomoku import Gomoku
from record import Record, UnRedoTree


class MainWindow(QtWidgets.QMainWindow):
    gameOver = QtCore.Signal()

    def __init__(self, cfg, **kwargs):
        super().__init__()
        
        self.mode = cfg.mode
        self.gameModel = Gomoku(cfg.boardSize, cfg.rule)
        self.recordModel = Record(cfg.variant, cfg.unfold)
        self.board = BoardWidget(cfg)
        self.recorder = RecorderWidget()
        self.controller = PlayControlWidget()

        self.setMinimumWidth(self.board.size() + 200)
        self.setFixedHeight(self.board.size()+ 30)
        self.setCentralWidget(self.board)
        dock = QtWidgets.QDockWidget("Record")
        dock.setWidget(self.recorder)
        self.addDockWidget(Qt.RightDockWidgetArea, dock)
        # self.layout.addWidget(self.control)

        self.setWindowTitle("Gomoku")
   
        # self.board.clickBoard.connect(self.responseToBoard)
        self.board.clickBoard.connect(self.response)
        self.recorder.jump.connect(self.response)
        self.recorder.choose.connect(self.response)
       
        # self.controller.undoButton.clicked.connect(self.undo)
        

        # self.startButton = QtWidgets.QPushButton("N",self)
        # self.settingButton = QtWidgets.QPushButton("⚙",self)
        # self.toolbar = QtWidgets.QToolBar(self)
        # self.toolbar.addAction('Peference')
        # self.toolbar.addAction('Help')
        # self.menubar = QtWidgets.QMenuBar()
        # # self.menubar.addAction('Peference')
        # self.menubar.addMenu('View')
        # self.menubar.addMenu('Help')
        # self.setMenuBar(self.menubar)   

        self.createStatusBar()
        self.createMenuBar()

        # TODO: add toolbar/menubar and setting panel

    def response(self, para):
        # 1. update backend: game, record
        s = QtCore.QObject.sender(self)
        if s == self.board:
            x, y = para
            self.recordModel.update((x, y))
            self.gameModel.go(x, y)   
            # tpoints_show = self.recordModel.getTimeline()

        else:
            index, kind = para
            if kind == 0: 
                self.recordModel.jump(index)
                self.gameModel.loadFrom(self.recordModel.timelineShow())
            else:
                choice = self.recordModel.choose(index)
                self.gameModel.go(*choice[index])

        # 2. update frontend 
        tpoints = [p.val for p in self.recordModel.timeline]
        tpoints_show = self.recordModel.timelineShow()
        isbranch = self.recordModel.isBranch()
        self.board.updateBoard(tpoints_show)
        # print(self.recorder.list.currentIndex())
        self.recorder.updateRecorder(tpoints, isbranch, self.recordModel.choices())

        # 3. check backend 
        if not self.gameModel.state:
            self.board.alive = False
            self.statusbar.showMessage(f"{'Black' if self.gameModel.winner == 1 else 'White'} Win !")

    def undo(self):
        self.recordModel.undo()
        # self.board.

        self.gameModel.loadFrom()

    def createMenuBar(self):
        self.menubar = QtWidgets.QMenuBar()
        self.menubar.addMenu('Help')
        self.setMenuBar(self.menubar)
        # self.menubar.addAction(QtWidgets.QAction('New',self.menubar))

    def createStatusBar(self):
        self.statusbar = QtWidgets.QStatusBar()
        self.statusbar.showMessage('Ready', 3000)
        self.setStatusBar(self.statusbar)


if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    cfg = Config(conf_file="config.yaml")
    w = MainWindow(cfg)
    w.show()

    sys.exit(app.exec())
