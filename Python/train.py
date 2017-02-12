#!/usr/bin/env python
#! -*- coding: utf-8 -*-


#複数のファイルを指定すれば片っ端から判別してくれる。
#  ./502_hanbetu__parts.py aaa.jpg bbb.jpg ...
#
#以下参照
#  http://kivantium.hateblo.jp/entry/2015/11/18/233834

import os
import sys
import numpy as np
import tensorflow as tf
import cv2


NUM_CLASSES = 70
IMAGE_SIZE = 28
IMAGE_PIXELS = IMAGE_SIZE*IMAGE_SIZE*3
datadir = '/Users/naohiro/SamurAI_Coding/traindata/'
side = 'first'
playerNum = 0
logNum = 7
stepNum = 50
batch_sizeNum = 20

flags = tf.app.flags
FLAGS = flags.FLAGS
flags.DEFINE_string('train', datadir + side + '/traintxt/train' + playerNum + '_2.txt', 'File name of train data')
flags.DEFINE_string('test', datadir + side + '/testtxt/test' + playerNum + '_2.txt', 'File name of train data')
flags.DEFINE_string('train_dir', datadir + side + '/learn_log' + playerNum + '/' + logNum + '/', 'Directory to put the training data.')
flags.DEFINE_integer('max_steps', stepNum, 'Number of steps to run trainer.')
flags.DEFINE_integer('batch_size', batch_sizeNum, 'Batch size'
                     'Must divide evenly into the dataset sizes.')
flags.DEFINE_float('learning_rate', 0.00001, 'Initial learning rate.')

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

def loss(logits, labels):
    """ lossを計算する関数

    引数:
      logits: ロジットのtensor, float - [batch_size, NUM_CLASSES]
      labels: ラベルのtensor, int32 - [batch_size, NUM_CLASSES]

    返り値:
      cross_entropy: 交差エントロピーのtensor, float

    """

    # 交差エントロピーの計算
    #aaaaa cross_entropy = -tf.reduce_sum(labels*tf.log(logits)) http://qiita.com/ikki8412/items/3846697668fc37e3b7e0
    cross_entropy = -tf.reduce_sum(labels*tf.log(tf.clip_by_value(logits,1e-10,1.0)))
    # TensorBoardで表示するよう指定
    tf.summary.scalar("cross_entropy", cross_entropy)
    return cross_entropy

def training(loss, learning_rate):
    """ 訓練のOpを定義する関数

    引数:
      loss: 損失のtensor, loss()の結果
      learning_rate: 学習係数

    返り値:
      train_step: 訓練のOp

    """

    train_step = tf.train.AdamOptimizer(learning_rate).minimize(loss)
    return train_step

def accuracy(logits, labels):
    """ 正解率(accuracy)を計算する関数

    引数:
      logits: inference()の結果
      labels: ラベルのtensor, int32 - [batch_size, NUM_CLASSES]

    返り値:
      accuracy: 正解率(float)

    """
    correct_prediction = tf.equal(tf.argmax(logits, 1), tf.argmax(labels, 1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, "float"))
    tf.summary.scalar("accuracy", accuracy)
    return accuracy

if __name__ == '__main__':
    # ファイルを開く
    f = open(FLAGS.train, 'r')
    # データを入れる配列
    train_image = []
    train_label = []
    for line in f:
        # 改行を除いてスペース区切りにする
        line = line.rstrip()
        l = line.split()
        # データを読み込んで28x28に縮小

        #print "1 line[%s] %s %s"%(line,l[0],l[1])

        img = cv2.imread(l[0])
        img = cv2.resize(img, (28, 28))
        # 一列にした後、0-1のfloat値にする
        train_image.append(img.flatten().astype(np.float32)/255.0)
        # ラベルを1-of-k方式で用意する
        tmp = np.zeros(NUM_CLASSES)
        tmp[int(l[1])] = 1
        train_label.append(tmp)
    # numpy形式に変換
    train_image = np.asarray(train_image)
    train_label = np.asarray(train_label)
    f.close()

    f = open(FLAGS.test, 'r')
    test_image = []
    test_label = []
    for line in f:
        line = line.rstrip()
        l = line.split()
        img = cv2.imread(l[0])
        img = cv2.resize(img, (28, 28))

        #print "2 line[%s] %s %s"%(line,l[0],l[1])

        test_image.append(img.flatten().astype(np.float32)/255.0)
        tmp = np.zeros(NUM_CLASSES)
        tmp[int(l[1])] = 1
        test_label.append(tmp)
    test_image = np.asarray(test_image)
    test_label = np.asarray(test_label)
    f.close()
    images_placeholder = tf.placeholder("float", shape=(None, IMAGE_PIXELS))
    labels_placeholder = tf.placeholder("float", shape=(None, NUM_CLASSES))
    keep_prob = tf.placeholder("float")

    logits = inference(images_placeholder, keep_prob)
    loss_value = loss(logits, labels_placeholder)
    train_op = training(loss_value, FLAGS.learning_rate)
    acc = accuracy(logits, labels_placeholder)
    sess = tf.InteractiveSession()

    saver = tf.train.Saver()
    sess.run(tf.global_variables_initializer())
    saver.restore(sess, "/Users/naohiro/SamurAI_Coding/traindata/first/learn_log0/4/model.ckpt")

	# TensorBoardで表示する値の設定
    summary_op = tf.summary.merge_all()
    summary_writer = tf.summary.FileWriter(FLAGS.train_dir, sess.graph)
    # 訓練の実行
    for step in range(FLAGS.max_steps):
        #print(step)
        for i in range(len(train_image)/FLAGS.batch_size):
            #print(1)
            # batch_size分の画像に対して訓練の実行
            batch = FLAGS.batch_size*i
            # feed_dictでplaceholderに入れるデータを指定する
            sess.run(train_op, feed_dict={
              images_placeholder: train_image[batch:batch+FLAGS.batch_size],
              labels_placeholder: train_label[batch:batch+FLAGS.batch_size],
              keep_prob: 0.5})

        # 1 step終わるたびに精度を計算する
        train_accuracy = sess.run(acc, feed_dict={
            images_placeholder: train_image,
            labels_placeholder: train_label,
            keep_prob: 1.0})
        train_loss = sess.run(loss_value, feed_dict={
            images_placeholder: train_image,
            labels_placeholder: train_label,
            keep_prob: 1.0})
        print "step %d, training accuracy %g, training cross entropy %g"%(step, train_accuracy, train_loss)

        # 1 step終わるたびにTensorBoardに表示する値を追加する
        summary_str = sess.run(summary_op, feed_dict={
            images_placeholder: train_image,
            labels_placeholder: train_label,
            keep_prob: 1.0})
        summary_writer.add_summary(summary_str, step)

    # 訓練が終了したらテストデータに対する精度を表示
    print "test accuracy %g"%sess.run(acc, feed_dict={
        images_placeholder: test_image,
        labels_placeholder: test_label,
        keep_prob: 1.0})
    # 最終的なモデルを保存
    save_path = saver.save(sess, datadir + side + "/learn_log" + playerNum + "/" + logNum + "/model.ckpt")
