import sys
import string

# import player
import players.playerlist as player

num_players = 6

def ReadCommentedIStream(cis):
    l = cis.readline()
    while l.startswith('#'):
        l = cis.readline()
    return l.split(' ')

class SamuraiInfo:
    """Samurai Information"""
    def __init__(self):
        self.home = (0, 0)
        self.cur = (0, 0)
        self.rank = 0
        self.score = 0
        self.hidden = False

class GameInfo:
    """Game Information"""
    def __init__(self, cis):
        ary = ReadCommentedIStream(cis)

        self.turns = int(ary[0])
        self.side = int(ary[1])
        self.weapon = int(ary[2])
        self.size = (int(ary[3]), int(ary[4]))
        self.maxCure = int(ary[5])

        self.samuraiInfo = [SamuraiInfo() for i in range(num_players)]
        for i in range(num_players):
            ary = ReadCommentedIStream(cis)
            self.samuraiInfo[i].home = (int(ary[0]), int(ary[1]))
        for i in range(num_players):
            ary = ReadCommentedIStream(cis)
            self.samuraiInfo[i].rank = int(ary[0])
            self.samuraiInfo[i].score = int(ary[1])

        self.turn = 0
        self.curePeriod = 0
        self.field = 0
        print('0')

    def readTurnInfo(self, cis):
        ary = ReadCommentedIStream(cis)
        if len(ary) == 0:
            exit(0)
        self.turn = int(ary[0])
        if self.turn < 0:
            exit(0)
        ary = ReadCommentedIStream(cis)
        self.curePeriod = int(ary[0])
        for i in range(num_players):
            ary = ReadCommentedIStream(cis)
            self.samuraiInfo[i].cur = (int(ary[0]), int(ary[1]))
            self.samuraiInfo[i].hidden = True if int(ary[2]) == 1 else False

        self.field = [[0 for i in range(self.size[1])] for j in range(self.size[0])]
        for y in range(self.size[1]):
            ary = ReadCommentedIStream(cis)
            for x in range(self.size[0]):
                self.field[x][y] = int(ary[x+1])

    def isValid(self, action):
        myself = self.samuraiInfo[self.weapon]
        cur = myself.cur
        if action >= 0 and action <= 4:
            if not(myself.hidden):
                return True
            return False
        if action >= 5 and action <= 8:
            if action == 5:
                cur = (cur[0], cur[1]+1)
            if action == 6:
                cur = (cur[0]+1, cur[1])
            if action == 7:
                cur = (cur[0], cur[1]-1)
            if action == 8:
                cur = (cur[0]-1, cur[1])
            if cur[0] < 0 or self.size[0] <= cur[0] or cur[1] < 0 or self.size[1] <= cur[1]:
                return False
            if myself.hidden and self.field[cur[0]][cur[1]] >= 3:
                return False
            for s in range(num_players):
                if cur == self.samuraiInfo[s].cur:
                    return False
                if s != self.weapon and cur == self.samuraiInfo[s].home:
                    return False
            return True
        if action == 9:
            if myself.hidden:
                return False
            if self.field[cur[0]][cur[1]] >= 3:
                return False
            return True
        if action == 10:
            if not(myself.hidden):
                return False
            for s in range(num_players):
                other = self.samuraiInfo[s]
                if not(other.hidden) and other.cur == cur:
                    return False
            return True
        exit(1)

    def rotate(self, direction, x0, y0):
        if direction == 0:
            x = x0
            y = y0
        if direction == 1:
            x = y0
            y = -x0
        if direction == 2:
            x = -x0
            y = -y0
        if direction == 3:
            x = -y0
            y = x0
        return (x, y)

    def occupy(self, direction):
        myself = self.samuraiInfo[self.weapon]
        cur = myself.cur
        size = [4, 5, 7]
        ox = [[0, 0, 0, 0],
              [0, 0, 1, 1, 2],
              [-1, -1, -1, 0, 1, 1, 1]]
        oy = [[1, 2, 3, 4],
              [1, 2, 0, 1, 0],
              [-1, -1, 1, 1, 1, -1, 0]]
        for k in range(size[self.weapon]):
            pos = self.rotate(direction, ox[self.weapon][k], oy[self.weapon][k])
            pos = (cur[0] + pos[0], cur[1] + pos[1])
            if 0 <= pos[0] and pos[0] < self.size[0] and 0 <= pos[1] and pos[1] < self.size[1]:
                isHome = False
                for ss in range(num_players):
                    if self.samuraiInfo[ss].home == pos:
                        isHome = True
                if isHome:
                    self.field[pos[0]][pos[1]] = self.weapon
                    for s in range(3, num_players):
                        si = self.samuraiInfo[s]
                        if si.cur == pos:
                            si.cur = si.home
                            si.hidden = False
                            self.samuraiInfo[s] = si

    def doAction(self, action):
        myself = self.samuraiInfo[self.weapon]
        cur = myself.cur
        if action >= 1 and action <= 4:
            self.occupy(action - 1)
        if action == 5:
            cur = (cur[0], cur[1]+1)
        if action == 6:
            cur = (cur[0]+1, cur[1])
        if action == 7:
            cur = (cur[0], cur[1]-1)
        if action == 8:
            cur = (cur[0]-1, cur[1])
        if action == 9:
            myself.hidden = True
        if action == 10:
            myself.hidden = False
        myself.cur = cur
        self.samuraiInfo[self.weapon] = myself
        print(action),

if __name__ == '__main__':
    info = GameInfo(sys.stdin)
    player = player.player('random')

    argvs = sys.argv
    argc = len(argvs)

    if argc < 2:
        print('Usage: python main.py playername')
        exit(0)

    while True:
        sys.stdout.flush()
        info.readTurnInfo(sys.stdin)
        print('# Turn', info.turn)
        if info.curePeriod != 0:
            print(0)
        else:
            player.play(info)
            print(0)

