import struct
import math
import numpy

def toHex(val, nbits = 16):
    return (val + (1 << nbits)) % (1 << nbits)

def mix(a, b, r):
    assert r <= 1
    return a * (1 - r) + r * b


chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 1
xBegin = 0
xEnd = 1

Scale = 40

def clamp(a,b,c):
	if a < b:
		return b
	elif a > c:
		return c
	else:
		return a

now = 0
for i in range(yBegin, yEnd):
	for j in range(xBegin, xEnd):
		print now
		now = now + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/Normal'
		print fileName
		w = h = chunkSize
		f = open(fileName, 'rb')
		#w, h, err = struct.unpack('LLf', f.read(3 * 4))
		heights = struct.unpack('f' * 3 * w * h, f.read(12 * w * h))
		f.close()
		ret = []
		tFile = '../data/' + dirName + '/newnormal'
		for s in range(h):
			for t in range(w):
				normal_0 = heights[(s*w+t)*3]
				normal_1 = heights[(s*w+t)*3+1]
				normal_2 = heights[(s*w+t)*3+2]
				ret.append(0.5 * (normal_0 + 1.0))
				#print ret[-1]
				ret.append(0.5 * (normal_1 + 1.0))
				#print ret[-1]
				ret.append(0.5 * (normal_2 + 1.0))
				#print ret[-1]
		output = open(tFile, 'wb')
		#output.write(struct.pack('LLf', w, h, err))
		output.write(struct.pack('f' * w * h * 3, *ret))
		output.close()