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

def getError(heights, w, h, sHeights, sw, sh):
	ratio = sw / w
	error = 1
	for i in range(0, sh):
		for j in range(0, sw):
			h0 = int(i / ratio)
			w0 = int(j / ratio)

			height_0 = heights[(h0 * w + w0) * 3]
			height_1 = heights[(h0 * w + w0) * 3 + 1]
			height_2 = heights[(h0 * w + w0) * 3 + 2]

			oheight_0 = sHeights[(i * sw + j) * 3]
			oheight_1 = sHeights[(i * sw + j) * 3 + 1]
			oheight_2 = sHeights[(i * sw + j) * 3 + 2]

			heightlength = math.sqrt(height_0 * height_0 + height_1 * height_1 + height_2 * height_2)
			oheightlength = math.sqrt(oheight_0 * oheight_0 + oheight_1 * oheight_1 + oheight_2 * oheight_2)
			#print heightlength, oheightlength
			if heightlength != 0:
				height_0 = height_0 / heightlength
				height_1 = height_1 / heightlength
				height_2 = height_2 / heightlength
			
			if oheightlength != 0:
				oheight_0 = oheight_0 / oheightlength
				oheight_1 = oheight_1 / oheightlength
				oheight_2 = oheight_2 / oheightlength
			

			tError = height_0 * oheight_0 + height_1 * oheight_1 + height_2 * oheight_2

			error = min(error, tError)
	return error



chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 32

level = 7

now = 0


for i in range(yBegin, yEnd):
	for j in range(xBegin, xEnd):
		print now
		now = now + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/Lean'
		print fileName
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
			output = open(tFile, 'wb')
			output.write(struct.pack('LLf', w, h, err))
			output.write(struct.pack('f' * 3 * w * h, *heights))
			if k != level - 1:
				w, h, heights = updateHeights(w, h, heights)
			output.close()