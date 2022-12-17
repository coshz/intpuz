from PySide2.QtGui import QPixmap


class Res:
    """designed for skin/classic.bmp"""
    def __init__(self, pixmap):
        self.data = QPixmap(pixmap)
        self.number = dict.fromkeys(range(9))
        self.timer = dict.fromkeys(range(10))
        self.state = {'ready': 0, 'running':0, 'check': 1, 'loss': 2, 'win': 3}
        self.mark = {'default': 0, 'clean': 1, 'flag': 3, 'mine_1': 2, 'mine_2': 4, 'boom': 5, 'unk': 6}
        self.wall = dict()
        self.timeboard = None
        # self.mark = dict.fromkeys(['default', 'clean', 'unk', 'flag', 'mine', 'boom'])

        self.parser()

    def parser(self):
        for k in self.number.keys():
            self.number[k] = self.data.copy(16 * k, 0, 16, 16)

        for k in self.timer.keys():
            self.timer[k] = self.data.copy(12 * k, 33, 11, 21)

        for k in self.mark.keys():
            idx = self.mark[k]
            self.mark[k] = self.data.copy(16 * idx, 16, 16, 16)

        for k in self.state.keys():
            idx = self.state[k]
            self.state[k] = self.data.copy(idx * 27, 55, 26, 26)

        self.timeboard = self.data.copy(28, 82, 25, 41)
        self.wall['lu'] = self.data.copy(0, 82, 12, 11)
        self.wall['lm'] = self.data.copy(0, 96, 12, 11)
        self.wall['ld'] = self.data.copy(0, 110, 12, 12)
        self.wall['ru'] = self.data.copy(15, 82, 12, 11)
        self.wall['rm'] = self.data.copy(15, 96, 12, 11)
        self.wall['rd'] = self.data.copy(15, 110, 12, 12)
        self.wall['xu'] = self.data.copy(13, 82, 1, 11)
        self.wall['xm'] = self.data.copy(13, 96, 1, 11)
        self.wall['xd'] = self.data.copy(13, 110, 1, 12)
        self.wall['yul'] = self.data.copy(0, 94, 12, 1)
        self.wall['yur'] = self.data.copy(15, 94, 12, 1)
        self.wall['ydl'] = self.data.copy(0, 108, 12, 1)
        self.wall['ydr'] = self.data.copy(15, 108, 12, 1)
