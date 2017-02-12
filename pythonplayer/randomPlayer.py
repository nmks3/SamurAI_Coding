import random
import sys
import string


required = [0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1]
randomplay = False

class randomPlayer:
    """Random Player"""
    def __init__(self):
        self.max_power = 7
        self.randomplay = False

    def play(self, info):
        power = self.max_power
        action=info.selectAction()
        if action == "6 8" or action == "5 7":
            self.randomplay = True
        if action == "5" or action == "6" or action == "7" or action == "8":
            action = action + " " + str(random.randint(1, 2))
        command=action.split()
        i=0
        if self.randomplay == True:
            while power >= 2:
                action = random.randint(1, 10)
                if required[action] <= power and info.isValid(action):
                    power -= required[action]
                    info.doAction(action)
            self.randomplay = False
        else:
            while power >= 2 and i < len(command):
                if required[int(command[i])] <= power and info.isValid(int(command[i])):
                    power -= required[int(command[i])]
                    info.doAction(int(command[i]))
                i += 1
        #if power != 0:
         #   action = 10 if info.samuraiInfo[info.weapon].hidden else 9
          #  if info.isValid(action):
           #     info.doAction(action)
