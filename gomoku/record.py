
class UnRedoNode:
    def __init__(self, value = None, parent = None):
        self.val = value
        self.suc = list()
        self.par = parent
    
    def numSuc(self):
        return len(self.suc)

    def isLeaf(self):
        return 0 == self.numSuc()


class UnRedoTree:
    def __init__(self, root = UnRedoNode(), max_variance = -1):
        self.variance = max_variance
        self.root = root
        # update `self.at` when forward, backward and jump
        self.at = self.root
    
    def createChild(self, value):
        for suc in self.at.suc:
            if suc.val == value:
                return suc
        else:
            _node = UnRedoNode(value, self.at)
            if self.variance == -1 or self.at.numSuc() < self.variance:
                self.at.suc.append(_node)
            else:
                self.at.suc.pop(0)
                self.at.suc.append(_node)
        return _node
    
    def foreward(self, value = None, index = 0):
        if value is not None:
            self.at = self.createChild(value)
        elif index < self.at.numSuc():
            self.at = self.at.suc[index]      

    def backward(self):
        if self.at.par is not None:
            self.at = self.at.par

    def tolist(self):
        _list = []
        p = self.root
        if p.isLeaf():
            _list.append(p.val)
        else:
            _list.append(p.val)
            for suc in p.suc:
                _list.append(UnRedoTree(suc).tolist())

        return _list


class Record(UnRedoTree):
    def __init__(self, max_variance = -1, unfold = False):
        super().__init__(max_variance = max_variance)

        # udpate `self.timeline` when jump
        self.timeline = []
        self.bpoint = []
        self.unfold = unfold
     
    def timelineShow(self):
        _timeline = []
        p = self.at
        while p.par != None:
            _timeline.append(p.val)
            p = p.par
        _timeline.reverse()

        return _timeline

    def isBranch(self):
        return [p.numSuc() > 1 for p in self.timeline]

    def update(self, value):
        # update unredotree, timeline and bpoint
        self.foreward(value)
        self.updateTimeline()
    
    def jump(self, index):
        self.at = self.timeline[index]
        self.updateTimeline()
        # self.timeline = self.timeline[:index+1]
        # self.unfoldTimeline()

    def choose(self, index):
        choice = []
        if self.at.numSuc() >= 1 and index < self.at.numSuc():
            choice = [p.val for p in self.at.suc]
            self.at = self.at.suc[index]
            self.updateTimeline()
        else:
            choice = [self.at.suc[0].val]
        
        return choice   
            # self.unfoldTimeline()

    # def fromStart(self):
    #     _list = []
    #     _bpoint = []
    #     p = self.at
    #     while p.par != None:
    #         _list.append(p.val)
    #         _bpoint.append((p.numSuc() > 1))
    #         p = p.par
    #     _list.reverse()
    #     _bpoint.reverse()

    #     return _list, _bpoint
        
    # def unfoldTimeline(self):
    #     p = self.at
    #     while not p.isLeaf() and (p.numSuc() == 1 or self.unfold):
    #         p = p.suc[0]
    #         self.timeline.append(p)
            
    def updateTimeline(self):
        self.timeline = []
        self.bpoint = []

        p = self.at
        while p.par != None:
            self.timeline.append(p)
            p = p.par
        
        self.timeline.reverse()

        
        p = self.at
        if self.unfold: # auto unfold first branch
            while p.numSuc() >= 1:
                p = p.suc[0]
                self.timeline.append(p) 
        else: # only unfold the only branch
            while p.numSuc() == 1:
                p = p.suc[0]
                self.timeline.append(p) 

    def choices(self):
        return [p.val for p in self.at.suc]

    
          
    # def undo(self):
    #     if self.at > 0:
    #         self.at -= 1

    # def redo(self):
    #     if self.at < len(self.history):
    #         self.at += 1

    @classmethod    
    def load(cls, record_str):
        data = list()
        for it in record_str.split():
            a, b = it
            x = ord(a)
            y = int(b)
            data.append((x,y))
        return data
    
    @classmethod
    def dump(cls, record_list):
        record = [chr(65 + it[0])+str(it[1]) for it in record_list].join()
        return record
