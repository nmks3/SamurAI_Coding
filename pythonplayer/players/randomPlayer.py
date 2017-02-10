import random
import sys

required = [0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1]

class randomPlayer:
    """Random Player"""
    def __init__(self):
        self.max_power = 7

    def play(self, info):
        power = self.max_power
        while power >= 2:
            action = random.randint(1, 10)
            if required[action] <= power and info.isValid(action):
                power -= required[action]
                info.doAction(action)
        if power != 0:
            if random.randint(0, 10) > 8:
                action = 10 if info.samuraiInfo[info.weapon].hidden else 9
                if info.isValid(action):
                    info.doAction(action)
