import randomPlayer

class player:
    """Select Player"""
    def __init__(self, playername):
        self.player = randomPlayer.randomPlayer()
        if playername == 'random':
            self.player = randomPlayer.randomPlayer()

    def play(self, info):
        self.player.play(info)
