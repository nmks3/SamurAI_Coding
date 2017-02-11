# -*- coding: utf-8 -*-
import numpy as np
import cv2
import sys

args = sys.argv
img_name = args[1]
i = args[2]
tate = "./tate_1.png"
yoko = "./yoko_1.png"

#15x15
img1 = cv2.imread(img_name)
#15x15
img2 = cv2.imread(img_name)
#縦連結 15x15 15x15
img3 = cv2.vconcat([img1, img2])
#15x30
#横連結 15x30 15x30
img4 = cv2.hconcat([img3,img3])
# 30x30
# 横余白画像
img5 = cv2.imread(yoko)
#縦連結 30x30 30x1
img6 = cv2.vconcat([img4,img5])
#90x93
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
cv2.imwrite(img_name, img10)

