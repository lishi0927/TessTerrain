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
			error0 = 0
			error1 = 0
			error2 = 0
			error3 = 0
			length = 0
			for t in range(3):
				h_w0_h0 = heights[(h0 * w + w0)*3 + t]
				h_w1_h0 = heights[(h0 * w + w1)*3 + t]
				h_w0_h1 = heights[(h1 * w + w0)*3 + t]
				h_w1_h1 = heights[(h1 * w + w1)*3 + t]
				
				wh0 = mix(h_w0_h0, h_w1_h0, 0.5)
				wh1 = mix(h_w0_h1, h_w1_h1, 0.5)
				height = mix(wh0, wh1, 0.5)
				error0 = error0 + h_w0_h0 * height
				error1 = error1 + h_w1_h0 * height
				error2 = error2 + h_w0_h1 * height
				error3 = error3 + h_w1_h1 * height
				length = length + height  * height
				result.append(height)
			error0 = error0 / math.sqrt(length)
			error1 = error1 / math.sqrt(length)
			error2 = error2 / math.sqrt(length)
			error3 = error3 / math.sqrt(length)
			error = min(error,error0)
			error = min(error,error1)
			error = min(error,error2)
			error = min(error,error3)
	return nW, nH, result,error

def getError(heights, w, h, sHeights, sw, sh):
	ratio = sw / w
	error = 1
	lasterror = 1
	hindex = 0
	windex = 0
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
			if(error < lasterror):
				
				#print error
				#print height_0, height_1, height_2, oheight_0, oheight_1, oheight_2
				#print oheightlength
				lasterror = error
				hindex = h0
				windex = w0
	print hindex,windex
	return error

normald = []
normal_phi = 8
normal_theta = 16

def getNormalDis(heights,sw,sh):
	normalflag = numpy.zeros((normal_phi * normal_theta / 32), dtype = numpy.int)
	for i in range(0, sh):
		for j in range(0, sw):
			oheight_0 = sHeights[(i * sw + j) * 3]
			oheight_1 = sHeights[(i * sw + j) * 3 + 1]
			oheight_2 = sHeights[(i * sw + j) * 3 + 2]
			oheightlength = math.sqrt(oheight_0 * oheight_0 + oheight_1 * oheight_1 + oheight_2 * oheight_2)
			
			if oheightlength != 0:
				oheight_0 = oheight_0 / oheightlength
				oheight_1 = oheight_1 / oheightlength
				oheight_2 = oheight_2 / oheightlength
			
			phi = math.acos(oheight_1)
			theta = math.atan2(oheight_2,oheight_0)
			phi = phi / math.pi * 180
			theta = theta/ math.pi * 180
			result1 = int(phi / 180 * normal_phi)
			result2 = int(theta / 360 * normal_theta)
			result = result1 * normal_theta + result2
			index1 = int(result / 32)
			index2 = result % 32
			normalflag[index1] = normalflag[index1] | (1 << index2)
	
	for i in range(normal_phi * normal_theta / 32):
		normald.append(normalflag[i])


chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 32

level = 7

now = 0
normalfile = '../data/normaldistribution'



for i in range(yBegin, yEnd):
	for j in range(xBegin, xEnd):
		print now
		now = now + 1
		dirName = str(j) + '_' + str(i)
		fileName = '../data/' + dirName + '/Normal'
		errorFileName = '../data/' + dirName + '/NormalerrList'
		print fileName
		w = h = chunkSize
		f = open(fileName, 'rb')
        # w, h = struct.unpack('LL', f.read(2 * 4))
		sHeights = heights = struct.unpack('f' * 3 * w * h, f.read(12 * w * h))
		f.close()
		sw = w
		sh = h

		getNormalDis(sHeights,sw,sh)
		errout = open(errorFileName, 'wb')
		
		err = 0
		for k in range(0, level):
			tFile = fileName + str(k)
			output = open(tFile, 'wb')
			output.write(struct.pack('LLf', w, h, err))
			output.write(struct.pack('f' * 3 * w * h, *heights))
			errout.write(struct.pack('f',err))
			if k != level - 1:
				w, h, heights,err = updateHeights(w, h, heights)
				print err
			output.close()
		errout.close()

normalout = open(normalfile, 'wb')
normalout.write(struct.pack('i' * (normal_phi * normal_theta / 32 * yEnd * xEnd), *normald))
normalout.close()