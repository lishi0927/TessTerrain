import struct
import os
import os.path
import sys
from sys import stdin
import numpy
import random
import math
import threading
import multiprocessing
import datetime
from multiprocessing import Pool, Array, Process
import mymodule

chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 32

level = 7

w = h = chunkSize

thetasize = 4
phisize = 16

PI = math.pi

threads = []



heights = numpy.zeros(((w+20)*(h+20)))


def ComputeVisbility(offsetx, offsety, phi, theta):
	height = mymodule.toShare[offsety * (512 + 20) + offsetx]
	#height = heights[offsety * (512 + 20) + offsetx]
	#print offsetx, offsety
	maxdistance = 6.0
	ray_visibility = 1
	origin = numpy.array([1.0 * offsetx, 1.0 * height,1.0 * offsety])
	vec = [math.sin(1.0 * theta / thetasize * PI / 2)* math.cos(1.0 *phi / phisize * PI * 2),math.cos(1.0 *theta / thetasize * PI / 2),math.sin(1.0 *theta / thetasize * PI / 2) * math.sin(1.0 *phi / phisize * PI * 2)]
	#print vec
	for samples in range(8):
		new_pos = numpy.array([0.0,0.0,0.0])
		new_pos[0] = origin[0] + (samples+1)*(maxdistance / 8.0) * vec[0]
		new_pos[1] = origin[1] + (samples+1)*(maxdistance / 8.0) * vec[1]
		new_pos[2] = origin[2] + (samples+1)*(maxdistance / 8.0) * vec[2]
		new_height = mymodule.toShare[(int(new_pos[2])) * (512+20)+ (int(new_pos[0]))]
		#new_height = heights[(int(new_pos[2])) * (512 + 20)+ (int(new_pos[0]))]
		if new_height >= new_pos[1]:
			ray_visibility *= 0
		else:
			ray_visibility *= 1
	return ray_visibility



def ComputeTexelVisibility(index):
	texelvisibility = 0
	texelsize = 0
	s = index % w
	t = (index - s) / w
	#print t,s
	xwidth = s + 10
	ywidth = t + 10
	rtheta = PI / 2 / thetasize
	rphi = PI * 2 / phisize
	pixelbit = 0
	pervis = numpy.zeros(((thetasize + 1) * (phisize + 1)))
	retv =  numpy.zeros((4))
	for theta in range(thetasize + 1):
		for phi in range(phisize + 1):
			pervis[theta * (phisize + 1) + phi] = ComputeVisbility(xwidth, ywidth, phi, theta)
	for theta in range(thetasize):
		for phi in range(phisize):
			visibility = 0
			for k in range(0,2):
				for l in range(0,2):
					visibility += pervis[(theta + k) * (phisize + 1) + (phi + l)]
			if visibility < 4:
				visibility = 0
			else:
				visibility = 1

			if texelsize < 16:
				texelvisibility =  texelvisibility * 2
				texelvisibility += visibility
				texelsize += 1
				#print visibility,texelsize
			else:
				retv[pixelbit] = texelvisibility
				texelsize = 0
				texelvisibility = 0
				pixelbit += 1
	return retv

def initProcess(share):
	mymodule.toShare = share

lowerborder = [502, 0, 0]
biggerborder = [512, 512, 10]
hoffsetx = [0, 10, 512]

if __name__ == '__main__':
	starttime = datetime.datetime.now()
	for i in range(yBegin, yEnd):
		for j in range(xBegin, xEnd):
			tot = 0
			for t in range(-1,2):
				for s in range (-1,2):
					if( i+t < 0 or i+t > 31 or j+s < 0 or j+s > 31):
						height = numpy.zeros((w*h))
					else:
						dirName = str(j+s) + '_' + str(i+t)
						fileName = '../data/' + dirName + '/dem'
						f = open(fileName, 'rb')
						height = struct.unpack('H' * w * h, f.read(2 * w * h))
					for k in range(lowerborder[t+1],biggerborder[t+1]):
						for l in range(lowerborder[s+1],biggerborder[s+1]):
							heights[(hoffsetx[t+1]+ k - lowerborder[t+1]) * (512 + 20)+ hoffsetx[s+1] + l-lowerborder[s+1]] = height[k*w+l]
					tot = tot + 1
			tt = 0
			#tasks = [(x, y) for x in range(0,1) for y in range(0, h)]
			X=[]
			#visibilityvalue =  numpy.zeros((w*h,4))
			for t in range(h):
				for s in range(w):
					X.append(t * w + s)
					#visibilityvalue[t * w + s] = ComputeTexelVisibility(t * w + s)
					#threads.append(threading.Thread(target = ComputeTexelVisibility,args=(t * w + s,)))
					#threads[-1].start()
			toShare = Array('f', (w+20)*(h+20), lock = False)
			#print len(heights)
			pool = Pool(initializer=initProcess,initargs=(toShare,))
			toShare[:len(heights)] = heights
			visibilityvalue = pool.map(ComputeTexelVisibility,X)
			pool.close()
			pool.join()

			endtime = datetime.datetime.now()
			print(endtime - starttime).seconds
		
			visibilityvalueobject = numpy.zeros((w*h*4))	
			dirname = str(j)+ '_' +str(i)
			Visfile = "../data/" + dirname + "/Vis"
			Visfiletxt = "../data/" + dirname + "/Vis.txt"
			print Visfile
			file_object = open(Visfiletxt, 'w')
			file_binary = open(Visfile, 'wb')
			for t in range(h):
				for s in range(w):
					file_object.write("%d %d %d %d " %(visibilityvalue[t*w+s][0], visibilityvalue[t*w+s][1], visibilityvalue[t*w+s][2], visibilityvalue[t*w+s][3]))
					for k in range(4):
						visibilityvalueobject[4*(t*w+s)+k] = visibilityvalue[t*w+s][k]
				file_object.write("\n")
			file_binary.write(struct.pack('H'*4*w*h, *visibilityvalueobject))
			file_object.close()
			file_binary.close()
