# -*- coding: utf-8 -*-
import numpy as np
import cv2
import sys

args = sys.argv
img_name = args[1]
i = args[2]
tate = "/Users/g-2017/SamurAI_Coding/Python/tate.png"
yoko = "/Users/g-2017/SamurAI_Coding/Python/yoko.png"

#15x15
img1 = cv2.imread(img_name)
#15x15
img2 = cv2.imread(img_name)
#縦連結 15x15 15x15
img3 = cv2.vconcat([img1, img2])
#15x30
cv2.imwrite(img_name, img3)
#縦連結 15x15 15x30
img4 = cv2.vconcat([img1, img3])
#15X45
cv2.imwrite(img_name, img4)
#縦連結 15x45 15x45
img5 = cv2.vconcat([img4,img4])
#15x90
cv2.imwrite(img_name, img5)
#横連結 15x90 15x90
img6 = cv2.hconcat([img5,img5])
# 30x90
cv2.imwrite(img_name, img6)
#横連結 15x90 30x90
img7 = cv2.hconcat([img5,img6])
# 45x90
cv2.imwrite(img_name, img7)
#横連結 15x90 30x90
img8 = cv2.hconcat([img7,img7])
# 90x90
cv2.imwrite(img_name, img8)
# 横余白画像
img9 = cv2.imread(yoko)
#縦連結 90x90 90x3
img10 = cv2.vconcat([img8,img9])
#90x93
cv2.imwrite(img_name, img10)
#縦連結 90x3 90x93
img11 = cv2.vconcat([img9,img10])
#90x96
cv2.imwrite(img_name, img11)
# 縦余白画像
img12 = cv2.imread(tate)
#横連結 90x96 3x96
img13 = cv2.hconcat([img11,img12])
# 93x96
cv2.imwrite(img_name, img13)
#横連結 3x96 93x96
img14 = cv2.hconcat([img12,img13])
# 96x96
cv2.imwrite(img_name, img14)

