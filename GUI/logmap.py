def tuplemap(f,tup):
    return tuple(map(f,list(tup)))

class Child:
    def __init__(self, value, rect):
        self.value = value
        self.rect = rect
    def __repr__(self):
        return str(self.value)+'*'

def rect_overlap(rect1, rect2):
    if (rect1[0] > rect2[0] + rect2[2] or
        rect1[0] + rect1[2] < rect2[0] or
        rect1[1] > rect2[1] + rect2[3] or
        rect1[1] + rect1[3] < rect2[1]):
        return None
    x = max(rect1[0], rect2[0])
    y = max(rect1[1], rect2[1])
    w = min(rect1[0] + rect1[2], rect2[0] + rect2[2]) - x
    h = min(rect1[1] + rect1[3], rect2[1] + rect2[3]) - y
    if w == 0 or h == 0:
        return None
    return (x,y,w,h)

def inrect(x,y,rect):
    if x>= rect[0] and x <= rect[0] + rect[2] and y >= rect[1] and y <= rect[1] + rect[3]:
        return True
    return False

class AdvancedLogMap:
    w_min, h_min = 8.0, 8.0

    def __init__(self, w, h):
        from math import ceil
        self.w, self.h = w, h
        self.sw, self.sh = float(w)/2, float(h)/2
        
        self.subrects = []
        
        self.children = []
        self.leaf = True

    def split(self):
        sw, sh = self.sw, self.sh
        NW = AdvancedLogMap(self.sw, self.sh)
        SW = AdvancedLogMap(self.sw, self.sh)
        NE = AdvancedLogMap(self.sw, self.sh)
        SE = AdvancedLogMap(self.sw, self.sh)
        self.subrects = [(NW,(0   ,0   ,sw,sh)),
                         (SW,(0   ,sh  ,sw,sh)),
                         (NE,(sw  ,0   ,sw,sh)),
                         (SE,(sw  ,sh  ,sw,sh))]
        self.leaf = False

    def add(self, obj, rect):
        if self.leaf and (self. w <= AdvancedLogMap.w_min or 
                          self. h <= AdvancedLogMap.h_min or 
                          len(self.children) == 0):
            self.children.append(Child(obj,rect))
        elif self.leaf:
            self.split()
            for child in self.children:
                self.add(child.value, child.rect)
            self.children = []
            self.add(obj,rect)
        else:
            laps = False
            for lm, r in self.subrects:
                overlap = rect_overlap(rect,r)
                if overlap:
                    overlap_new = (overlap[0] - r[0], overlap[1] - r[1], overlap[2], overlap[3])
                    lm.add(obj,overlap_new)
                    laps = True
            if not laps:
                print rect,"Wut??????????????????"

    def get(self, rect):
        if self.leaf:
            ret = []
            for child in self.children:
                if rect_overlap(child.rect,rect):
                    ret.append(child)
            return ret

        ret = []
        for lm, r in self.subrects:
            overlap = rect_overlap(rect,r)
            if overlap:
                overlap_new = (overlap[0] - r[0], overlap[1] - r[1], overlap[2], overlap[3])
                ret += lm.get(overlap_new)
        return ret

    def get_pos(self, x, y):
        if self.leaf:
            ret = []
            for child in self.children:
                if inrect(x,y,child.rect):
                    ret.append(child)
            return ret

        ret = []
        for lm, r in self.subrects:
            if inrect(x,y,r):
                xn ,yn = x - r[0], y - r[1]
                return lm.get_pos(xn,yn)
        return []

    def __repr__(self):
        return '(%.1f,%.1f)<' % (self.w,self.h) + str(self.children) + ':' + '\n| '.join(map(lambda x: str(x[0]),self.subrects)) + '>'
        
class LogMap:
    def __init__(self,w,h):
        self.w = w
        self.h = h

        self.sw = 0
        self.sh = 0

        self.child_x = -1
        self.child_y = -1
        self.child = None

        self.leaf = True

    def add(self, obj, x, y):
        x = int(x)
        y = int(y)

        if self.leaf and not self.child:
            self.child = [obj]
            self.child_x = x
            self.child_y = y

        elif self.leaf and self.child:
            if x == self.child_x and y == self.child_y:
                self.child.append(obj)
                return
            old_child = self.child
            self.leaf = False

            sw,sh = self.w/2, self.h/2
            self.child = [ # clockwise submaps
                LogMap(sw,sh), # NW 
                LogMap(sw,sh), # NE
                LogMap(sw,sh), # SE
                LogMap(sw,sh)  # SW
            ]
            self.sw = sw
            self.sh = sh

            map(lambda x: self.add(x, self.child_x, self.child_y), old_child)
            self.add(obj,x,y)
        else:
            sw, sh = self.sw, self.sh
            if x < sw:
                if y < sh: # NW
                    self.child[0].add(obj, x, y)
                else:                 # SW
                    self.child[3].add(obj, x, y - sh)
            else:
                if y < sh: # NE
                    self.child[1].add(obj, x - sw, y)
                else:                 # SE
                    self.child[2].add(obj, x - sw, y - sh)
            
    def get(self, x, y):
        x = int(x)
        y = int(y)

        if self.leaf:
            return self.child
            
        sw, sh = self.sw, self.sh
        if x < sw:
            if y < sh:      # NW
                return self.child[0].get(x, y)
            else:           # SW
                return self.child[3].get(x, y - sh)
        else:
            if y < sh: # NE
                return self.child[1].get(x - sw, y)
            else:                 # SE
                return self.child[2].get(x - sw, y - sh)

    def __repr__(self):
        if self.leaf and self.child:
            return "<" +', '.join(map(str,self.child))+'>'
        elif self.leaf:
            return "<>"
        return "[" + ', '.join(map(str,self.child))+']'
        
if __name__ == "__main__":
    lmap = AdvancedLogMap(100.0,100.0)
    try:
        lmap.add("kek",(10.0,10.0,10.0,10.0))
        lmap.add("lel",(12.0,10.0,30.0,13.0))
        print lmap
    except RuntimeError:
        pass
