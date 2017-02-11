# -*- coding: utf-8 -*-
import numpy as np
import cv2
import sys

args = sys.argv
img_name = args[1]
i = args[2]
tate = "/Users/g-2017/SamurAI_Coding/Python/tate_1.png"
yoko = "/Users/g-2017/SamurAI_Coding/Python/yoko_1.png"

#15x15
img1 = cv2.imread(img_name)
#15x15
img2 = cv2.imread(img_name)
#縦連結 15x15 15x15
img3 = cv2.vconcat([img1, img2])
#15x30
cv2.imwrite(img_name, img3)
#横連結
img4 = cv2.hconcat([img3, img3])
#30x30
cv2.imwrite(img_name, img3)
#横余白画像
img5 = cv2.imread(yoko)
#縦連結 30x30 30x1
img6 = cv2.vconcat([img4, img5])
#30x31
cv2.imwrite(img_name, img6)
#縦連結 30x31 30x1
img7 = cv2.vconcat([img5, img6])
#30x32
cv2.imwrite(img_name, img7)
#縦余白画像
img8 = cv2.imread(tate)
#横連結 30x32 1x32
img9 = cv2.hconcat([img7, img8])
#31x32
cv2.imwrite(img_name, img9)
#横連結 1x32 31x32
img10 = cv2.hconcat([img8, img9])
#31x32
cv2.imwrite(img_name, img10)
