import struct

def toHex(val, nbits = 16):
    return (val + (1 << nbits)) % (1 << nbits)

def getEle(ret, i, j, w):
    if (i < 0 or j < 0):
        return 0
    return ret[i * w + j]

chunkSize = 50
chunkNumber = 32
yBegin = 15
yEnd = yBegin + chunkNumber
xBegin = -24
xEnd = xBegin + chunkNumber

hTop = 200.0
ep = 60.0

def transform(x):
    return pow(x / hTop, ep)

def transform1(x):
    return pow(1 - x / hTop, ep)

def restore(x, tot):
    return pow(x, 1.0 / ep) * hTop

def restore1(x, tot):
    return (1 - pow(x / tot, 1.0 / ep)) * hTop

def writeMat(dirname, maxOrMin, data, w, h):
    if (maxOrMin == 0):
        tFile = "data/" + dirname + "/maxDEMSAT"
    else:
        tFile = "data/" + dirname + "/minDEMSAT"

    output = open(tFile, "wb")
    # output.write(struct.pack("L" * 5, w, h, htop, hoff, ep))
    output.write(struct.pack("L" * 5, w, h, hTop, 0, ep))
    output.write(struct.pack("d" * len(data), *data))

for i in range(yBegin, yEnd):
    for j in range(xBegin, xEnd):
        lStr = '%04x' % toHex(i)
        rStr = '%04x' % toHex(j)
        dirName = lStr + rStr
        fileName = 'data/' + dirName + '/dem'

        f = open(fileName, 'rb')
        w, h = struct.unpack('LL', f.read(4 * 2))
        heights = struct.unpack('f' * w * h, f.read(4 * w * h))
        f.close()

        # ret = []
        # ret1 = []

        ma = -100000
        mi = -ma
        for k1 in range(0, h):
            for k2 in range(0, w):
                ma = max(ma, heights[k1 * w + k2])
                mi = min(mi, heights[k1 * w + k2])
        print ma
        #         height = transform(heights[k1 * w + k2])
        #         height1 = transform1(heights[k1 * w + k2])
        #         ret.append(getEle(ret, k1 - 1, k2, w) + \
        #                    getEle(ret, k1, k2 - 1, w) - \
        #                    getEle(ret, k1 - 1, k2 - 1, w) + \
        #                    height)
        #         ret1.append(getEle(ret1, k1 - 1, k2, w) + \
        #                     getEle(ret1, k1, k2 - 1, w) - \
        #                     getEle(ret1, k1 - 1, k2 - 1, w) + \
        #                     height1)
        #
        # writeMat(dirName, 0, ret, w, h)
        # writeMat(dirName, 1, ret1, w, h)
