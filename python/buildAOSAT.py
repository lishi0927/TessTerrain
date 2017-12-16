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

tot = 0
level = 7

for i in range(yBegin, yEnd):
    for j in range(xBegin, xEnd):
		print tot
		tot = tot + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/AO'
		
		SATfileName = '../data/' + dirName + '/maxAOdiff'
		f1 = open(SATfileName, 'wb')
		maxDiff = 0.0
		minDiff = 1.0
		ret = []
		for t in range(level):
			tFile = fileName + str(t)
			f = open(tFile, 'rb')
			w, h,error = struct.unpack('LLf', f.read(12))
			heights = struct.unpack('f' * w * h, f.read(4 * w * h))
			f.close()
			md = 0
			for y in range(0, h):
				for x in range(0, w):
					maxDiff = max(maxDiff, heights[y * w + x])
					minDiff = min(minDiff, heights[y * w + x])
					for k in range(0, 4):
						tx = x + dx[k]
						ty = y + dy[k]
						if tx < 0 or ty < 0 or tx >= w or ty >= h:
							continue
						md = max(md, abs(heights[y * w + x] - heights[ty * w + tx]))
			ret.append(md)
			print md
		
		ans.append(maxDiff - minDiff)
		f1.write(struct.pack('f' * len(ret), *ret))
		f1.close()
			# print restore(ret[len(ret) - 1], 1)


output = sys.stdout
nOutput = open('../data/maxAO.txt', 'w')
sys.stdout = nOutput
for i in range(0, len(ans)):
    print ans[i]
sys.stdout = output
nOutput.close()