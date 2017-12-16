import struct
import math
import datetime
from multiprocessing import Pool


chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 32

level = 7

now = 0
thetasize = 4
phisize = 16

def change(index):
	y = int(index / xEnd)
	x = index % xEnd
	dirName = str(x) + '_' + str(y)
	fileName = '../data/' + dirName + '/Vis'
	print fileName
	tFile1 = fileName + 'a'
	tFile2 = fileName + 'b'
	tFile3 = fileName + 'c'
	tFile4 = fileName + 'd'
	result1_1 = []
	result2_1 = []
	result3_1 = []
	result4_1 = []
	err = 0.0
	tfileName = fileName + str(0)
	f = open(tfileName, 'rb')
	w,h = struct.unpack('LL',f.read(8))
	heights = struct.unpack('H' * 4 * w * h, f.read(8 * w * h))
	f.close()
	for i in range(h):
			for j in range(w):
				for k in range(4):
					result = heights[4*(i*w+j)+k]
					ret = []
					for t in range(4):
						tmp = result % 16
						rtmp = 0
						for s in range(4):
							rtmp << 4
							if tmp % 2 == 1:
								rtmp += 15
						ret.append(rtmp)
						result << 4
					result1_1.append(ret[0])
					result2_1.append(ret[1])
					result3_1.append(ret[2])
					result4_1.append(ret[3])

	tfilename = tFile1 + str(0)
	f = open(tfilename, 'wb')
	f.write(struct.pack('LLf', w, h ,err))
	f.write(struct.pack('H' * 4 * w * h, *result1_1))
	f.close()

	tfilename = tFile2 + str(0)
	f = open(tfilename, 'wb')
	f.write(struct.pack('LLf', w, h,err))
	f.write(struct.pack('H' * 4 * w * h, *result2_1))
	f.close()

	tfilename = tFile3 + str(0)
	f = open(tfilename, 'wb')
	f.write(struct.pack('LLf', w, h,err))
	f.write(struct.pack('H' * 4 * w * h, *result3_1))
	f.close()

	tfilename = tFile4 + str(0)
	f = open(tfilename, 'wb')
	f.write(struct.pack('LLf', w, h,err))
	f.write(struct.pack('H'* 4 * w * h, *result4_1))
	f.close()

	
	for k in range(1, level):
		result1 = []
		result2 = []
		result3 = []
		result4 = []
		tfileName = fileName + str(k)
		f = open(tfileName, 'rb')
		w,h = struct.unpack('L'*2, f.read(8))
		w = chunkSize >> k
		h = w
		heights = struct.unpack('H' * 16 * w * h, f.read(32 * w * h))
		for i in range(h):
			for j in range(w):
				for t in range(4):
					result1.append(heights[((2*i)*w+j)*4 + t])
					result2.append(heights[((2*i)*w+j+1)*4 + t])
					result3.append(heights[((2*i+1)*w+j)*4 + t])
					result4.append(heights[((2*i+1)*w+j+1)*4 + t])
		f.close()
		tfilename = tFile1 + str(k)
		f = open(tfilename, 'wb')
		f.write(struct.pack('LLf', w, h,err))
		f.write(struct.pack('H' * 4 * w * h, *result1))
		f.close()

		tfilename = tFile2 + str(k)
		f = open(tfilename, 'wb')
		f.write(struct.pack('LLf', w, h,err))
		f.write(struct.pack('H'* 4 * w * h, *result2))
		f.close()

		tfilename = tFile3 + str(k)
		f = open(tfilename, 'wb')
		f.write(struct.pack('LLf', w, h,err))
		f.write(struct.pack('H'* 4 * w * h, *result3))
		f.close()

		tfilename = tFile4 + str(k)
		f = open(tfilename, 'wb')
		f.write(struct.pack('LLf', w, h,err))
		f.write(struct.pack('H'* 4* w * h, *result4))
		f.close()




if __name__ == '__main__':
	X = []
	for i in range(yBegin, yEnd):
		for j in range(xBegin, xEnd):
			X.append(i * xEnd + j)
	pool = Pool()
	pool.map(change,X)
	pool.close()
	pool.join()