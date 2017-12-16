import struct
import math
import numpy
from multiprocessing import Pool, Array, Process
import mymodule
import datetime

chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 1
xBegin = 0
xEnd = 1

level = 7

now = 0
thetasize = 4
phisize = 16

#sHeights = numpy.zeros((chunkSize * chunkSize * 4))
#newVis = numpy.zeros((thetasize*phisize*chunkSize * chunkSize))


def initProcess(share):
	mymodule.toShare = share

def ComputeOffset(index):
	w = chunkSize
	s = index % w
	t = (index - s) / w
	t0 =  mymodule.toShare[4*(s*w+t)]
	t1 =  mymodule.toShare[4*(s*w+t)+1]
	t2 =  mymodule.toShare[4*(s*w+t)+2]
	t3 =  mymodule.toShare[4*(s*w+t)+3]
	ret = numpy.zeros((thetasize*phisize))
	for i in range(16):
		ret[i] = (t0 & 1)
		t0 = t0 >> 1
	for i in range(16):
		ret[i + phisize] = (t1 & 1)
		t1 = t1 >> 1
	for i in range(16):
		ret[i + 2 * phisize] = (t2 & 1)
		t2 = t2 >> 1
	for i in range(16):
		ret[i + 3 * phisize] = (t3 & 1)
		t3 = t3 >> 1
	return ret


def updateVis(newVis,k):
	w = chunkSize >> k
	h = chunkSize >> k
	nw = w >> 1
	nh = h >> 1
	width = 1 << (k+1)

	resultVis = numpy.zeros((w * h * 4))
	result = []
	for i in range(0, nh):
		for j in range(0, nw):
			ystart = i * 2
			xstart = j * 2
			height = numpy.zeros((thetasize*phisize))
			for s in range(width):
				for t in range(width):
					for k in range(thetasize*phisize):
						height[k] = height[k] + newVis[nw*(ystart+s)+(xstart+j)][k]
			for k in range(thetasize*phisize):
				ret = height[k]/ width / width
				result.append(ret)

	for i in range(0, nh):
		for j in range(0, nw):
			retbit = 0
			retvalue = 0
			retnumx = 0
			retnumy = 0
			for k in range(thetasize*phisize):
				ret = result[thetasize*phisize*(i * nw + j) + k]
				intret = int(ret * 15)
				retvalue = retvalue + intret
				retbit = retbit + 1
				if retbit == 4:
					resultVis[(2 * i + retnumy) * 8 * nw + 8 * j + retnumx] = retvalue
					retnumx = retnumx + 1
					if retnumx == 8:
						retnumx = 0
						retnumy = 1
					retvalue = 0
					retbit = 0
				else:
					retvalue << 4

	return w,h,resultVis


if __name__ == '__main__':
	starttime = datetime.datetime.now()
	for i in range(yBegin, yEnd):
		for j in range(xBegin, xEnd):
			print now
			now = now + 1
			dirName = str(j) + '_' + str(i)
			fileName = '../data/' + dirName + '/Vis'
			fileNametxt = '../data/' + dirName + '/Vis.txt'
			print fileName
			w = h = chunkSize
			f = open(fileName, 'rb')
			sHeights = heights = struct.unpack('H' * 4 * w * h, f.read(8 * w * h))
			f.close()

		
			X=[]
			for s in range(h):
				for t in range(w):
					X.append(t * w + s)
			toShare = Array('H', w*h*4, lock = False)
			pool = Pool(initializer=initProcess,initargs=(toShare,))
			toShare[:len(heights)] = sHeights
			newVis = pool.map(ComputeOffset,X)
			pool.close()
			pool.join()
	

			sw = w
			sh = h
			
			for k in range(0, level):
				tFile = fileName + str(k)
				tFileName = fileNametxt + str(k)
				output = open(tFile, 'wb')
				file_object = open(tFileName, 'w')
				print w,h
				output.write(struct.pack('LL', w, h))
				output.write(struct.pack('H' * 4 * w * h, *heights))
				for s in range(h):
					for t in range(w):
						file_object.write("%d %d %d %d " %(heights[4*(s*w+t)],heights[4*(s*w+t)+1],heights[4*(s*w+t)+2],heights[4*(s*w+t)+3]))
					file_object.write("\n")
				if k != level - 1:
					w, h, heights = updateVis(newVis, k)
				output.close()
				file_object.close()
			endtime = datetime.datetime.now()
			print(endtime - starttime).seconds