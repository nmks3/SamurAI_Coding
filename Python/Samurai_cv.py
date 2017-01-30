# -*- coding: utf-8 -*-
import numpy as np
import cv2
import re

# 列を設定
cols = 15
# 行を設定
rows = 15
# プレイヤー情報を指定
side = "first"
arm = "2"
# 作業ディレクトリを指定
dir_name = "/Users/install/ML_inoue/SamurAI_Coding/"

for i in range(10):
	# イメージ生成
	image = np.zeros((rows, cols, 3), np.uint8)
	# 出力画像の名前を制御する変数
	imageNum = i*15000
	# 各入力ファイルの番号を制御する変数
	fileNum = str(i * 1000 + 1) + '-' + str((i + 1)*1000)
	plusFileNum = i*1000 + 1
	# ラベルを格納する配列
	label = []
	# 配列の添字を制御する変数
	labelNum = 1
	# 読み込むラベルファイルを設定
	label_txt = dir_name + 'traindata/' + side + '/labels' + arm + '/label' + fileNum + '.txt'
	fr2 = open(label_txt)
	lines2 = fr2.readlines()
	# 事前に準備してあるラベルファイルを読み取り、配列に格納
	for line2 in lines2:
		# 改行の除去
		line2 = line2.rstrip()
		# 配列に追加
		label.append(line2)
	fr2.close()
	#print(label)
	# 出力ファイル名を設定
	# 正解ラベルがある場合のみ、当該フォルダへ画像を保存
	if label[0] == '':
		outimg = dir_name + 'traindata/' + side + '/' + arm + '/' + str(imageNum) + '.png'
	elif label[0] == 'amurai1.0disqualified':
		outimg = dir_name + 'traindata/' + side + '/' + arm + '/' + str(imageNum) + '.png'
	else:
		outimg = dir_name + 'traindata/' + side + '/' + arm + '/' + label[0] + '/' +str(imageNum) + '.png'

	# プレイヤーの状態を記録する配列	
	playerState = []

	# 画像を白にする
	def drawWhite():
		image[0:15, 0:15] = [255, 255, 255]

	# 各プレイヤーの状況を画像に記録
	# プレイヤーの番号、プレイヤーの座標
	def drawPlayerState(playerNum, playerX, playerY):
		if playerNum == 0:
			image[playerY, playerX] = [0, 255, 0]
		elif playerNum == 1:
			image[playerY, playerX] = [0, 170, 0]
		else:
			image[playerY, playerX] = [0, 85, 0] 

	# 現在のステージの状況を画像に記録
	def drawStage(state, row, col):
		x = col
		y = row
		if state == 0:
			image[y, x] = [0, 0, 255]
		elif state == 1:
			image[y, x] = [0, 0, 170]
		elif state == 2:
			image[y, x] = [0, 0, 85]
		elif state == 3:
			image[y, x] = [255, 0, 0]
		elif state == 4:
			image[y, x] = [170, 0, 0]
		elif state == 5:
			image[y, x] = [85, 0, 0]
		elif state == 8:
			image[y, x] = [0,0,0]

	# 画像の初期化
	drawWhite()
	for k in range(1000):
		# 読み込んだ行数を記録する変数
		lineNum = 0
		lineNum2 = 0
		# 読み込むファイルを設定
		input_txt = dir_name + 'matchlog/game' + fileNum + '/game' + str(k + plusFileNum) + '/input' + arm 
		fr = open(input_txt)
		lines = fr.readlines()
		# 現在どのファイルを読み込んでいるか表示する
		print('Now file is : game' + str(k + plusFileNum))
		# 1行ずつ読み込む
		for line in lines:
			if lineNum >= 16:
				if (lineNum - 16)%28 > 5 and (lineNum - 16)%28 < 9:
					#改行の除去
					line = line.rstrip()
					# 空白の除去
					line = re.split(" +",line)
					playerState.append(line)
				# プレイヤー状況の記録
				if (lineNum - 16)%28 > 12 and (lineNum - 16)%28 < 28:
					#改行の除去
					line = line.rstrip()
					# 空白の除去
					line = re.split(" +",line)

					for i in range(16):
						if i >= 1:
							drawStage(int(line[i]),(lineNum - 16)%28-13, i - 1)
				# 次	の画像に記録する
				if (lineNum - 16) != 0 and (lineNum-16)%28 == 0:
					for j in range(3):
						drawPlayerState(j-1,int(playerState[j-1][0]),int(playerState[j-1][1]))
					del playerState[:]
					# 画像の出力
					cv2.imwrite(outimg, image)
					# 新しいイメージの生成
					image = np.zeros((rows, cols, 3), np.uint8)
					# 出力ファイル名を設定
					imageNum += 1
					# 正解ラベルがある場合のみ、当該フォルダへ画像を保存
					if label[labelNum] == '':
						outimg = dir_name + 'traindata/' + side + '/other' + arm + '/' + str(imageNum) + '.png'
					elif label[labelNum] == 'amurai1.0disqualified':
						outimg = dir_name + 'traindata/' + side + '/other' + arm + '/' + str(imageNum) + '.png'
					else:
						outimg = dir_name + 'traindata/' + side + '/' + arm + '/' + label[labelNum] + '/' +str(imageNum) + '.png'
					drawWhite()
					# ラベルの添字をカウント
					labelNum += 1
			# 行数のカウント
			lineNum += 1
