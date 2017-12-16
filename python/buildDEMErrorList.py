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

dx = [-1, 0, 1, 0]
dy = [0, -1, 0, 1]

ans = []

hTop = 40000.0
ep = 60.0

def getEle(ret, x, y, w):
    if x < 0 or y < 0:
        return 0
    return ret[y * w + x]

def transform(x):
    return pow(x / hTop, ep)

def restore(x, tot):
    return pow(x / tot, 1.0 / ep) * hTop

tot = 0
level = 7

for i in range(yBegin, yEnd):
    for j in range(xBegin, xEnd):
		print tot
		tot = tot + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/dem'
		
		for t in range(level):
			tFile = fileName + str(t)
			f = open(tFile, 'rb')
			w, h,error = struct.unpack('LLf', f.read(12))
			heights = struct.unpack('H' * w * h, f.read(2 * w * h))
			f.close()
			ret = []
			maxDiff = 0
			for y in range(0, h):
				for x in range(0, w):
					md = 0
					for k in range(0, 4):
						tx = x + dx[k]
						ty = y + dy[k]
						if tx < 0 or ty < 0 or tx >= w or ty >= h:
							continue
						md = max(md, abs(heights[y * w + x] - heights[ty * w + tx]))
					ret.append(md)
					maxDiff = max(maxDiff, md)
			# print maxDiff
			ans.append(maxDiff)
			SATfileName = '../data/' + dirName + '/maxDiffSAT'
			for y in range(0, h):
				for x in range(0, w):
					d = transform(ret[y * w + x])
					ret[y * w + x] = getEle(ret, x - 1, y, w) + \
					getEle(ret, x, y - 1, w) - \
					getEle(ret, x - 1, y - 1, w) + \
					d
			satFile = SATfileName + str(t)
			f = open(satFile, 'wb')
			f.write(struct.pack("L" * 5, w, h, hTop, 0, ep))
			f.write(struct.pack('d' * len(ret), *ret))
			f.close()
			# print restore(ret[len(ret) - 1], 1)


output = sys.stdout
nOutput = open('../data/maxDiff.txt', 'w')
sys.stdout = nOutput
for i in range(0, len(ans)):
    print ans[i]
sys.stdout = output
nOutput.close()
