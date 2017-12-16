import struct
import sys
import math


chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 32

level = 7

tot = 0
for i in range(yBegin, yEnd):
	for j in range(xBegin, xEnd):
		print tot
		tot = tot + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/dem'
		averageH = []
		for k in range(level):
			tFile = fileName + str(k)
			f = open(tFile, 'rb')
			w, h, err = struct.unpack('LLf', f.read(12))
			heights = struct.unpack('H' * w * h, f.read(2 * w * h))
			f.close()
			sum = 0
			for p in range(w * h):
				sum = 1.0 * sum * p / (p+1) + 1.0 * heights[p] / (p+1)
			#print w,h,sum
			averageH.append(sum)
        
		fileName = '../data/' + dirName + '/averageHeight'
		f = open(fileName, 'wb')
		f.write(struct.pack('f' * len(averageH), *averageH))
		f.close()