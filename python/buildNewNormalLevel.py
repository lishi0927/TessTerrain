import struct
import math
import numpy

def toHex(val, nbits = 16):
    return (val + (1 << nbits)) % (1 << nbits)

def mix(a, b, r):
    assert r <= 1
    return a * (1 - r) + r * b

def updateHeights(w, h, heights):
	nW = w >> 1
	nH = h >> 1
	result = []
	error = 0
	for i in range(0, nH):
		for j in range(0, nW):
			h0 = i * 2
			h1 = i * 2 + 1
			w0 = j * 2
			w1 = j * 2 + 1
			for t in range(3):
				h_w0_h0 = heights[(h0 * w + w0)*3 + t]
				h_w1_h0 = heights[(h0 * w + w1)*3 + t]
				h_w0_h1 = heights[(h1 * w + w0)*3 + t]
				h_w1_h1 = heights[(h1 * w + w1)*3 + t]
				
				wh0 = mix(h_w0_h0, h_w1_h0, 0.5)
				wh1 = mix(h_w0_h1, h_w1_h1, 0.5)
				height = mix(wh0, wh1, 0.5)
				result.append(height)
	return nW, nH, result



chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 1

level = 7

now = 0


for i in range(yBegin, yEnd):
	for j in range(xBegin, xEnd):
		print now
		now = now + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/newnormal'
		normalfiletxt = "../data/" + dirName + "/newnormal.txt"
		w = h = chunkSize
		f = open(fileName, 'rb')
        # w, h = struct.unpack('LL', f.read(2 * 4))
		heights = struct.unpack('f' * 3 * w * h, f.read(12 * w * h))
		f.close()
		sw = w
		sh = h
		err = 0
		for k in range(0, level):
			tFile = fileName + str(k)
			normalfiletxt_o = normalfiletxt + str(k)
			output = open(tFile, 'wb')
			file_normal_o = open(normalfiletxt_o, 'w')
			output.write(struct.pack('LLf', w, h, err))
			output.write(struct.pack('f' * 3 * w * h, *heights))
			for t in range(h):
				for s in range(w):
					file_normal_o.write("%f %f %f " %(heights[(t * w + s)*3],heights[(t * w + s)*3+1],heights[(t * w + s)*3+2]))
				file_normal_o.write("\n")
			if k != level - 1:
				w, h, heights = updateHeights(w, h, heights)
			output.close()
			file_normal_o