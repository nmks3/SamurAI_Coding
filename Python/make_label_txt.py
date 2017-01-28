# -*- coding: utf-8 -*-
import re

for i in range(10):
	# 各入力ファイルの番号を制御する変数
	fileNum = str(i * 1000 + 1) + '-' + str((i + 1)*1000)
	plusFileNum = i*1000 + 1
	# ラベルを格納する配列
	label = []
	# 配列の添字を制御する変数
	labelNum = 1
	fileName = '/Users/g-2017/SamurAI_Coding/Python/label' + fileNum + '.txt'
	fw = open(fileName, 'w')

	for k in range(1000):
		# 読み込んだ行数を記録する変数
		lineNum2 = 0
		# 読み込むファイルを設定
		label_txt = '/Users/g-2017/SamurAI_Coding/matchlog/game' + fileNum + '/game' + str(k + plusFileNum) + '/log'
		fr2 = open(label_txt)
		lines2 = fr2.readlines()
		# 現在どのファイルを読み込んでいるか表示する
		#print('Now file is : game' + str(k + 1))
		for line2 in lines2:
			if line2[0] == '#':
				if line2[19] == '0' and line2[31] == '2':
					#改行の除去
					line2 = line2.rstrip()
					line2 = line2[43:]
					line2 = re.sub('10','9',line2)
					line2 = re.sub(' ','',line2)
					# 行動コストを設定
					cost = 7
					count = 0
					for i in line2:
						if i == '1' or i == '2' or i == '3' or i == '4':
							cost -= 4
						elif i == '5' or i == '6' or i == '7' or i == '8':
							cost -= 2
						elif i == '9':
							cost -= 1
						else: 
							break
						if cost > 0:
							count += 1
					line2 = line2[0:count]
					line2 = re.sub('9','',line2)
					line2 = re.sub('565','556',line2)
					line2 = re.sub('576','567',line2)
					line2 = re.sub('585','558',line2)
					line2 = re.sub('655','556',line2)
					line2 = re.sub('656','566',line2)
					line2 = re.sub('658','568',line2)
					line2 = re.sub('657','567',line2)
					line2 = re.sub('665','566',line2)
					line2 = re.sub('676','667',line2)
					line2 = re.sub('685','586',line2)
					line2 = re.sub('686','668',line2)
					line2 = re.sub('687','678',line2)
					line2 = re.sub('755','557',line2)
					line2 = re.sub('756','567',line2)
					line2 = re.sub('757','577',line2)
					line2 = re.sub('758','578',line2)
					line2 = re.sub('76','67',line2)
					line2 = re.sub('766','667',line2)
					line2 = re.sub('767','677',line2)
					line2 = re.sub('776','677',line2)
					line2 = re.sub('787','778',line2)
					line2 = re.sub('85','58',line2)
					line2 = re.sub('855','558',line2)
					line2 = re.sub('856','568',line2)
					line2 = re.sub('857','578',line2)
					line2 = re.sub('858','588',line2)
					line2 = re.sub('86','68',line2)
					line2 = re.sub('865','568',line2)
					line2 = re.sub('866','668',line2)
					line2 = re.sub('867','678',line2)
					line2 = re.sub('87','78',line2)
					line2 = re.sub('877','778',line2)
					line2 = re.sub('878','788',line2)
					line2 = re.sub('885','588',line2)
					line2 = re.sub('887','788',line2)
					fw.writelines(line2 + "\n")
					lineNum2 += 1
				elif line2[20] == '0'  and line2[32] == '2':
					#改行の除去
					line2 = line2.rstrip()
					line2 = line2[44:]
					line2 = re.sub('10','9',line2)
					line2 = re.sub(' ','',line2)
					for i in line2:
						if cost > 0:
							if i == '1' or i == '2' or i == '3' or i == '4':
								cost -= 4
							elif i == '5' or i == '6' or i == '7' or i == '8':
								cost -= 2
							elif i == '9':
								cost -= 1
							else: 
								break
							count += 1
					line2 = line2[0:count]
					line2 = re.sub('9','',line2)
					line2 = re.sub('565','556',line2)
					line2 = re.sub('576','567',line2)
					line2 = re.sub('585','558',line2)
					line2 = re.sub('655','556',line2)
					line2 = re.sub('656','566',line2)
					line2 = re.sub('658','568',line2)
					line2 = re.sub('657','567',line2)
					line2 = re.sub('665','566',line2)
					line2 = re.sub('676','667',line2)
					line2 = re.sub('687','678',line2)
					line2 = re.sub('755','557',line2)
					line2 = re.sub('756','567',line2)
					line2 = re.sub('757','577',line2)
					line2 = re.sub('758','578',line2)
					line2 = re.sub('76','67',line2)
					line2 = re.sub('766','667',line2)
					line2 = re.sub('767','677',line2)
					line2 = re.sub('776','677',line2)
					line2 = re.sub('787','778',line2)
					line2 = re.sub('85','58',line2)
					line2 = re.sub('855','558',line2)
					line2 = re.sub('856','568',line2)
					line2 = re.sub('857','578',line2)
					line2 = re.sub('858','588',line2)
					line2 = re.sub('86','68',line2)
					line2 = re.sub('865','568',line2)
					line2 = re.sub('866','668',line2)
					line2 = re.sub('867','678',line2)
					line2 = re.sub('87','78',line2)
					line2 = re.sub('877','778',line2)
					line2 = re.sub('878','788',line2)
					line2 = re.sub('885','588',line2)
					line2 = re.sub('887','788',line2)
					fw.writelines(line2 + "\n")
					lineNum2 += 1
	fw.close()
