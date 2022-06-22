from PySide6 import QtCore, QtWidgets, QtGui
from PySide6.QtCore import Qt
from help import Config


class BoardWidget(QtWidgets.QWidget):

    clickBoard =  QtCore.Signal(tuple)

    def __init__(self, cfg: Config):
        super().__init__()
        self.cfg = cfg
        self.boardSize = cfg.boardSize
        self.recordPoints = list()
        self.alive = True

    def size(self):
        return (self.boardSize - 1) * self.cfg.cellSize + 2 * self.cfg.padding
    
    def transCoordinate(self, pos, kind = None):
        """
        kind = 0: (int, int) -> QPoint, game board coordinate -> board widget coordinate
        kind = 1: QPoint -> (int, int), board widget coordinate -> game board coordinate
        """
        transpos = None
        if kind == None:
            kind = 1 if isinstance(pos, QtCore.QPoint) else \
                  (0 if isinstance(pos, tuple) else 2)

        if kind == 0:
            x, y = pos
            transpos =  QtCore.QPoint(
                self.cfg.padding + self.cfg.cellSize * (x - 1), 
                self.cfg.padding + self.cfg.cellSize * (self.boardSize - y)
            )
        elif kind == 1:
            x = int ((pos.x() - self.cfg.padding) / self.cfg.cellSize)
            y = int ((pos.y() - self.cfg.padding) / self.cfg.cellSize)
            for _x, _y in [(x, y),(x+1, y), (x,y+1), (x+1, y+1)]:
                _pos = QtCore.QPoint(
                    self.cfg.padding + self.cfg.cellSize * _x,
                    self.cfg.padding + self.cfg.cellSize * _y
                )
                if (pos - _pos).manhattanLength() < self.cfg.cellSize / 3:
                    transpos = (1 + _x, self.boardSize - _y)
                    break
        else:
            raise NotImplementedError
        
        return transpos
  
    def updateBoard(self, recordPoints):
        self.recordPoints = []
        for x, y in recordPoints:
            self.recordPoints.append(self.transCoordinate((x,y),0))
        self.update()

    def paintEvent(self, event: QtGui.QPaintEvent) -> None:

        painter = QtGui.QPainter(self)

        lattice = lambda i: self.cfg.cellSize*i + self.cfg.padding
        region = lambda p: QtCore.QRect(
            p.x() - self.cfg.cellSize / 3, p.y() - self.cfg.cellSize / 3,
            self.cfg.cellSize * 2/3, self.cfg.cellSize *2/3)

        # 1. frame = board + padding
        painter.setPen(QtGui.QPen(Qt.black, 2, Qt.SolidLine))
        painter.setBrush(QtGui.QBrush(QtGui.QColor(*self.cfg.bgColor)))
        _size = self.size()
        painter.drawRect(0,0,_size,_size)

        # 2. grid
        painter.setPen(QtGui.QPen(Qt.black, 1, Qt.SolidLine))
        
        for i in range(self.boardSize):
            painter.drawLine(lattice(0), lattice(i), lattice(self.boardSize-1), lattice(i))        
        for j in range(self.boardSize):
            painter.drawLine(lattice(j), lattice(0), lattice(j), lattice(self.boardSize-1))
        
        # 3. key points
        painter.setPen(Qt.NoPen)
        painter.setBrush(QtGui.QBrush(Qt.black))
        z1, z2, z3 = (lattice(3), lattice(int(self.boardSize/2)), lattice(self.boardSize - 4))
        plist = [QtCore.QPoint(z1,z1), QtCore.QPoint(z1,z3), QtCore.QPoint(z2,z2), 
            QtCore.QPoint(z3,z1), QtCore.QPoint(z3,z3)]
        for p in plist:
            painter.drawEllipse(p, 4, 4)
        
        # 4. ordinate
        if self.cfg.showOrdinate:
            painter.setPen(QtGui.QPen(Qt.black,1))
            spacing = self.cfg.padding / 2
            for i in range(self.boardSize):
                painter.drawText(lattice(i) - 5, lattice(self.boardSize - 1) + spacing, str(i+1))
            for i in range(self.boardSize):
                painter.drawText(lattice(0) - spacing, lattice(self.boardSize - 1 - i) + 5,  chr(65 + i))
            
        # update board
        if len(self.recordPoints) > 0:
            scale = 5 / 12
            painter.setPen(Qt.NoPen)
            painter.setBrush(QtGui.QBrush(Qt.black))
            for i in range(0, len(self.recordPoints), 2):
                painter.drawEllipse(self.recordPoints[i], self.cfg.cellSize*scale, self.cfg.cellSize*scale)
            
            painter.setBrush(QtGui.QBrush(Qt.white))
            for i in range(1, len(self.recordPoints), 2):
                painter.drawEllipse(self.recordPoints[i], self.cfg.cellSize*scale, self.cfg.cellSize*scale) 
            
            if self.cfg.showNumber:
                painter.setPen(QtGui.QPen(Qt.white,3))
                painter.setFont(QtGui.QFont("Courier", 18, QtGui.QFont.Bold))
               
                for i in range(0, len(self.recordPoints), 2):
                    painter.drawText(region(self.recordPoints[i]), Qt.AlignCenter, str(i+1))
                
                painter.setPen(QtGui.QPen(Qt.black,3))
                for i in range(1, len(self.recordPoints), 2):
                    painter.drawText(region(self.recordPoints[i]), Qt.AlignCenter, str(i+1))

            elif self.cfg.showCurrent:
                _curcolor = Qt.black if len(self.recordPoints) % 2 == 0 else Qt.white
                painter.setPen(QtGui.QPen(_curcolor,3))
                painter.setFont(QtGui.QFont("Courier", 34))
                painter.drawText(region(self.recordPoints[-1]), Qt.AlignCenter, '▽')
        
    def mousePressEvent(self, event: QtGui.QMouseEvent):
        if self.alive:
            pos = self.transCoordinate(event.pos(), 1)
            if pos is not None and self.transCoordinate(pos, 0) not in self.recordPoints:
                self.clickBoard.emit(pos)

        return super().mousePressEvent(event)


class PlayControlWidget(QtWidgets.QWidget):
    # start = QtCore.Signal()
    # undo = QtCore.Signal()
    # redo = QtCore.Signal()

    def __init__(self):
        super().__init__()
        
        self.undoButton = QtWidgets.QPushButton("<")
        self.redoButton = QtWidgets.QPushButton(">")
        self.toStartButton = QtWidgets.QPushButton("<<")
        self.toEndButton = QtWidgets.QPushButton(">>")
        
        layout = QtWidgets.QHBoxLayout()
        
        layout.setSpacing(2)
        # layout.addWidget(self.startButton, Qt.AlignCenter)
        # layout.addWidget(self.showForbiddenButton)
        layout.addWidget(self.toStartButton)
        layout.addWidget(self.undoButton)
        layout.addWidget(self.redoButton)
        layout.addWidget(self.toEndButton)
        
        self.setLayout(layout)


class RecorderWidget(QtWidgets.QWidget):
    # clickRecoder = QtCore.QSignal()
    jump = QtCore.Signal(tuple)
    choose = QtCore.Signal(tuple)

    def __init__(self):
        super().__init__()

        self.list = QtWidgets.QListWidget()
        self.variant = QtWidgets.QListWidget()

        self.setFixedWidth(200)
        spliter = QtWidgets.QSplitter(Qt.Vertical)
        spliter.addWidget(self.list)
        spliter.addWidget(self.variant)
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(spliter)
        self.setLayout(layout)
        self.list.setFont(QtGui.QFont("Courier", 14))
        self.variant.setFont(QtGui.QFont("Courier", 14))

        self.list.itemClicked.connect(self.response)
        self.variant.itemClicked.connect(self.response)

        self.currentSelect = tuple()
        self.currentIndice = tuple()

        # TODO: add UP-DOWN key event to control backword and forword
        
    def response(self, item):
        s = QtCore.QObject.sender(self)

        if s == self.list:
            index = self.list.indexFromItem(item).row()
            self.currentIndice = (index, -1)
            self.jump.emit((index,0))
        else:
            index_list = self.list.currentRow()
            index_variant = self.variant.indexFromItem(item).row()
            # self.currentSelect = (index_variant, 1)
            self.currentIndice = (index_list, index_variant)
            # self.choose.emit(self.currentIndice)
            self.choose.emit((index_variant,1))

    def updateRecorder(self, timeline, isbranch, variant):
        self.list.clear()
        self.variant.clear()
        for i, (it, flag) in enumerate(zip(timeline, isbranch)):
            x, y = it
            _flag = '*' if flag else ''
            _text = f"{i + 1:03},\t{x}{chr(y + 96)}{_flag}"
            self.list.addItem(_text)
        
        for i, it in enumerate(variant):
            x, y = it
            _text = f"{i + 1:03},\t{x}{chr(y + 96)}"
            self.variant.addItem(_text)
        
        if self.currentIndice:
            _index1, _index2 = self.currentIndice 
            self.list.setCurrentRow(_index1)
            if _index2 >= 0:
                self.variant.setCurrentRow(_index2)
                self.list.setCurrentRow(_index1 + 1)



            


    