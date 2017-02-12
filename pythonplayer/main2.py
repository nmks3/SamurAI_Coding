# -*- coding: utf-8 -*-
import sys
import string
import cv2
import numpy as np
import tensorflow as tf
import subprocess
# import player
import players.playerlist as player

# 列を設定
cols = 15
# 行を設定
rows = 15
# プレイヤー
num_players = 6
# クラス数
NUM_CLASSES = 70
# 画像サイズ
IMAGE_SIZE = 28
# pixel計算
IMAGE_PIXELS = IMAGE_SIZE*IMAGE_SIZE*3
images_placeholder = tf.placeholder("float", shape=(None, IMAGE_PIXELS))
labels_placeholder = tf.placeholder("float", shape=(None, NUM_CLASSES))
keep_prob = tf.placeholder("float")
# 余白画像
tate = "./tate_1.png"
yoko = "./yoko_1.png"
# ラベル
label = ["1","1 5","1 6","1 7","1 8","2","2 5","2 6","2 7","2 8","3","3 5","3 6","3 7","3 8","4","4 5","4 6","4 7","4 8","5","5 1","5 2","5 3","5 4","5 5","5 5 5","5 5 6","5","5 5 8","5 6","5 6 6","6","5","5 7","7","8","5 8","5 8 8","6","6 1","6 2","6 3","6 4","6 6","6 6 6","6 6 7","6","6 7","6 7 7","7","6 8","8","7","7 1","7 2","7 3","7 4","7 7","7 7 7","7 7 8","7 8","7 8 8","8","8 1","8 2","8 3","8 4","8 8","8 8 8"]

def ReadCommentedIStream(cis):
    l = cis.readline()
    while l.startswith('#'):
        l = cis.readline()
    return string.strip(l).split()

def inference(images_placeholder, keep_prob):
    """ モデルを作成する関数

    引数:
      images_placeholder: inputs()で作成した画像のplaceholder
      keep_prob: dropout率のplace_holder

    返り値:
      cross_entropy: モデルの計算結果
    """
    def weight_variable(shape):
      initial = tf.truncated_normal(shape, stddev=0.1)
      return tf.Variable(initial)

    def bias_variable(shape):
      initial = tf.constant(0.1, shape=shape)
      return tf.Variable(initial)

    def conv2d(x, W):
      return tf.nn.conv2d(x, W, strides=[1, 1, 1, 1], padding='SAME')

    def max_pool_2x2(x):
      return tf.nn.max_pool(x, ksize=[1, 2, 2, 1],
                            strides=[1, 2, 2, 1], padding='SAME')

    x_image = tf.reshape(images_placeholder, [-1, 28, 28, 3])

    with tf.name_scope('conv1') as scope:
        W_conv1 = weight_variable([5, 5, 3, 32])
        b_conv1 = bias_variable([32])
        h_conv1 = tf.nn.relu(conv2d(x_image, W_conv1) + b_conv1)

    with tf.name_scope('pool1') as scope:
        h_pool1 = max_pool_2x2(h_conv1)

    with tf.name_scope('conv2') as scope:
        W_conv2 = weight_variable([5, 5, 32, 64])
        b_conv2 = bias_variable([64])
        h_conv2 = tf.nn.relu(conv2d(h_pool1, W_conv2) + b_conv2)

    with tf.name_scope('pool2') as scope:
        h_pool2 = max_pool_2x2(h_conv2)

    with tf.name_scope('fc1') as scope:
        W_fc1 = weight_variable([7*7*64, 1024])
        b_fc1 = bias_variable([1024])
        h_pool2_flat = tf.reshape(h_pool2, [-1, 7*7*64])
        h_fc1 = tf.nn.relu(tf.matmul(h_pool2_flat, W_fc1) + b_fc1)
        h_fc1_drop = tf.nn.dropout(h_fc1, keep_prob)

    with tf.name_scope('fc2') as scope:
        W_fc2 = weight_variable([1024, NUM_CLASSES])
        b_fc2 = bias_variable([NUM_CLASSES])

    with tf.name_scope('softmax') as scope:
        y_conv=tf.nn.softmax(tf.matmul(h_fc1_drop, W_fc2) + b_fc2)

    return y_conv

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
        self.logits = inference(images_placeholder, keep_prob)
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
        print '0'

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
        # イメージ生成
        image = np.zeros((rows, cols, 3), np.uint8)
        outimg = "/Users/g-2017/Samuraicoding/" + str(self.turn) + "out.png"
        # 画像の初期化
        image[0:15, 0:15] = [255, 255, 255]
        
        for y in range(self.size[1]):
            ary = ReadCommentedIStream(cis)
            for x in range(self.size[0]):
                self.field[x][y] = int(ary[x])
                if int(ary[x]) == 0:
                    image[y, x] = [0, 0, 255]
                elif int(ary[x]) == 1:
                    image[y, x] = [0, 0, 170]
                elif int(ary[x]) == 2:
                    image[y, x] = [0, 0, 85]
                elif int(ary[x]) == 3:
                    image[y, x] = [255, 0, 0]
                elif int(ary[x]) == 4:
                    image[y, x] = [170, 0, 0]
                elif int(ary[x]) == 5:
                    image[y, x] = [85, 0, 0]
                elif int(ary[x]) == 8:
                    image[y, x] = [0,0,0]

        #縦連結 15x15 15x15
        img3 = cv2.vconcat([image, image])
        #15x30
        #横連結 15x30 15x30
        img4 = cv2.hconcat([img3,img3])
        # 30x30
        # 横余白画像
        img5 = cv2.imread(yoko)
        #縦連結 30x30 30x1
        img6 = cv2.vconcat([img4,img5])
        #30x31
        #縦連結 30x1 30x31
        img7 = cv2.vconcat([img5,img6])
        #30x32
        # 縦余白画像
        img8 = cv2.imread(tate)
        #横連結 30x32 1x32
        img9 = cv2.hconcat([img7,img8])
        # 31x32
        #横連結 1x32 31x32
        img10 = cv2.hconcat([img8,img9])
        # 32x32
        # 画像の出力
        self.img = cv2.resize(img10, (28, 28))
        #cv2.imwrite(outimg, self.img)

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
        print action,

    def selectAction(self):
        test_image = self.img.flatten().astype(np.float32)/255.0
        sess = tf.InteractiveSession()
        saver = tf.train.Saver()
        sess.run(tf.global_variables_initializer())
        saver.restore(sess, "/Users/g-2017/SamuraiCoding/traindata/first_1/learn_log2/2/model.ckpt")
        pred = np.argmax(self.logits.eval(feed_dict={images_placeholder: [test_image],keep_prob: 1.0 })[0])
        return label[pred]

if __name__ == '__main__':
    info = GameInfo(sys.stdin)
    player = player.player('random')
    argvs = sys.argv
    argc = len(argvs)

    if argc < 2:
        print 'Usage: python main.py playername'
        exit(0)

    while True:
        sys.stdout.flush()
        info.readTurnInfo(sys.stdin)
        print '# Turn', info.turn
        if info.curePeriod != 0:
            print 0
        else:
            player.play(info)
            print 0

