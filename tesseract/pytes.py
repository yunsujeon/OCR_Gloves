import cv2
import numpy as np
try:
	import Image
except ImportError:
	from PIL import Image
import pytesseract
import re
import serial


cap = cv2.VideoCapture(0)
m = 0
a = 0
b = 0
c = 0
d = 0
index = 0
PORT = '/dev/ttyACM0'
BaudRate = 9600
ARD= serial.Serial(PORT,BaudRate)
img_cut = 0
img_crop = 0

print index
while True:
    
	ret,img_color = cap.read()
	if ret == False:
		continue
	img_gray = cv2.cvtColor(img_color, cv2.COLOR_BGR2GRAY)
	
	rst,img_binary = cv2.threshold(img_gray,255, 255,cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)
	
	kernel = np.ones((1,1), np.uint8)
	img_removenoize = cv2.morphologyEx(img_binary, cv2.MORPH_OPEN, kernel)
	kernel2 = np.ones((7, 7), np.uint8)
	img_dilated = cv2.dilate(img_removenoize, kernel2, iterations = 2)
	

	nlabels, labels, stats, centroids = cv2.connectedComponentsWithStats(img_dilated)
	for i in range(nlabels):
		if i < 2:
			continue
		left = stats[i, cv2.CC_STAT_LEFT]
		top = stats[i, cv2.CC_STAT_TOP]
		width = stats[i, cv2.CC_STAT_WIDTH]
		height = stats[i, cv2.CC_STAT_HEIGHT]
		
		cv2.rectangle(img_color, (left, top), (left + width, top + height),(0, 0, 255), 1)
		cv2.line(img_color, (350,250),(350,280),(0,255,0),1)
		cv2.putText(img_color, str(i), (left + 20, top + 20),cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
		#rst,img_binary = cv2.threshold(img_gray,100, 255,cv2.THRESH_BINARY | cv2.THRESH_OTSU)
		if m == 0 and top + height < 280 and top + height > 250 and left < 350 and left + width > 350 :
			a = left
			b = top
			c = width
			d = height
			m = 1
		img_cut=img_dilated[230:270,350:353]
		no_white = cv2.countNonZero(img_cut)
		
		if m == 2 and top + height < 280 and top + height > 250 and no_white < 1:
			m=0
	
	

	if m == 1 and b + d < 280 and b + d > 250 and a < 350 and a + c > 350 :
		img_crop = img_binary[b : b + d, a : a + c]
		buf = "/home/dotheart/OCR_Gloves/tesseract/img_saves/tesimg%05d.jpg" % (index)
		cv2.imwrite(buf,img_crop)
		tess = pytesseract.image_to_string(Image.open('/home/dotheart/OCR_Gloves/tesseract/img_saves/tesimg%05d.jpg' % (index)), config=' --psm 7')
		tess = tess.encode('utf-8')
		print tess
		#ARD.write(tess)

		m = 2
		index +=1
		print tess
		print buf
		if index == 99 :
			index = 0

	
	cv2.imshow("Color", img_color)
	cv2.imshow("crop", img_crop)
	cv2.imshow("Cut", img_cut)

	if cv2.waitKey(1)&0xFF == 27:
		
		break


cap.release()
cv2.destroyAllWindows()

