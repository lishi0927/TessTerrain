import struct
import sys
import math

def toHex(val, nbits=16):
    return (val + (1 << nbits)) % (1 << nbits)

chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = yBegin + chunkNumber
xBegin = 0
xEnd = xBegin + chunkNumber

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
                sum += heights[p]
            sum /= w * h
            averageH.append(sum)
        
        fileName = '../data/' + dirName + '/averageHeight'
        f = open(fileName, 'wb')
        f.write(struct.pack('f' * len(averageH), *averageH))
        f.close()
