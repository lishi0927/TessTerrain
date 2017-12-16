import struct
import math

def toHex(val, nbits = 16):
    return (val + (1 << nbits)) % (1 << nbits)

def mix(a, b, r):
    assert r <= 1
    return a * (1 - r) + r * b

def updateHeights(w, h, heights):
    nW = w >> 1
    nH = h >> 1
    result = []
    ratio = float(w - 1) / (nW - 1)
    for i in range(0, nH):
        for j in range(0, nW):
            th = i * ratio
            tw = j * ratio
            h0 = int(math.floor(th))
            h1 = int(math.ceil(th - 1e-6))
            rh = th - h0
            w0 = int(math.floor(tw))
            w1 = int(math.ceil(tw - 1e-6))
            rw = tw - w0

            h_w0_h0 = heights[h0 * w + w0]
            h_w1_h0 = heights[h0 * w + w1]
            h_w0_h1 = heights[h1 * w + w0]
            h_w1_h1 = heights[h1 * w + w1]

            wh0 = mix(h_w0_h0, h_w1_h0, rw)
            wh1 = mix(h_w0_h1, h_w1_h1, rw)
            height = int(mix(wh0, wh1, rh))

            result.append(height)
    return nW, nH, result

def getError(heights, w, h, sHeights, sw, sh):
    ratio = float(sw - 1) / (w - 1)
    error = 0
    for i in range(0, sh):
        for j in range(0, sw):
            th = i / ratio
            tw = j / ratio
            h0 = int(math.floor(th))
            h1 = int(math.ceil(th - 1e-6))
            rh = th - h0
            w0 = int(math.floor(tw))
            w1 = int(math.ceil(tw - 1e-6))
            rw = tw - w0

            h_w0_h0 = heights[h0 * w + w0]
            h_w1_h0 = heights[h0 * w + w1]
            h_w0_h1 = heights[h1 * w + w0]
            h_w1_h1 = heights[h1 * w + w1]

            wh0 = mix(h_w0_h0, h_w1_h0, rw)
            wh1 = mix(h_w0_h1, h_w1_h1, rw)
            height = mix(wh0, wh1, rh)

            tError = math.fabs(sHeights[i * sw + j] - height)
            error = max(error, tError)

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
		fileName = '../data/' + dirName + '/dem'
		errorFileName = '../data/' + dirName + '/errList'
		
		w = h = chunkSize
		f = open(fileName, 'rb')
        # w, h = struct.unpack('LL', f.read(2 * 4))
		sHeights = heights = struct.unpack('H' * w * h, f.read(2 * w * h))
		f.close()
		sw = w
		sh = h

		errout = open(errorFileName, 'wb')
		
		err = 0
		for k in range(0, level):
			tFile = fileName + str(k)
			output = open(tFile, 'wb')
			output.write(struct.pack('LLf', w, h, err))
			output.write(struct.pack('H' * w * h, *heights))
			errout.write(struct.pack('f',err))
			# print err
			if k != level - 1:
				w, h, heights = updateHeights(w, h, heights)
				err = getError(heights, w, h, sHeights, sw, sh)
			output.close()
		errout.close()