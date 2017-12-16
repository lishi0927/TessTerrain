import struct
import os
import os.path
import sys
import numpy
import random
import math

chunkSize = 512
chunkNumber = 32
yBegin = 0
yEnd = 32
xBegin = 0
xEnd = 32

def normalize(vector3):
	sum = math.sqrt(vector3[0] * vector3[0] + vector3[1] * vector3[1] + vector3[2] * vector3[2])
	if(sum == 0):
		return [0.0,0.0,0.0]
	vector3[0] = vector3[0] / sum
	vector3[1] = vector3[1] / sum
	vector3[2] = vector3[2] / sum
	return vector3


def GenerateAO(offsetx, offsety, heights):
	#print offsetx, offsety
	height = heights[offsetx * 512 * 3 + offsety]
	height01 = heights[offsetx * 512 * 3+ offsety - 1]
	height21 = heights[offsetx * 512 * 3+ offsety + 1]
	height10 = heights[(offsetx - 1) * 512 * 3+ offsety]
	height12 = heights[(offsetx + 1) * 512 * 3+ offsety]
	x = [2.0,0.0,height21-height01]
	y = [0.0,2.0,height12-height10]
	normal = numpy.cross(x,y)
	normal = normalize(normal)
	#print normal

	if math.fabs(normal[0]) > math.fabs(normal[1]):
		binormal = [normal[2], 0.0, -normal[0]]
	else:
		binormal = [0.0, -normal[2], normal[1]]
	binormal = normalize(binormal)
	tangent = numpy.cross(binormal, normal)

	visibility = 0
	maxdistance = 6.0

	for i in range(4):
		for j in range(4):
			u0 = (i + random.random()) / 4
			v0 = (j + random.random()) / 4
			r = math.sqrt(u0)
			phi = 2.0 * math.pi * v0
			p = numpy.array([0.0,0.0,0.0])
			p[0] = r * math.cos(phi)
			p[1] = r * math.sin(phi)
			p[2] = math.sqrt(max(0.0, 1.0 - p[0] * p[0] - p[1]* p[1]))
			q =  numpy.array([0.0,0.0,0.0])
			q[0] = p[0] * tangent[0] + p[1] * binormal[0] + p[2] * normal[0]
			q[1] = p[0] * tangent[1] + p[1] * binormal[1] + p[2] * normal[1]
			q[2] = p[0] * tangent[2] + p[1] * binormal[2] + p[2] * normal[2]
			p = normalize(q)
			origin = numpy.array([1.0 * offsety, 1.0 * offsetx,1.0 * height])
			ray_visibility = 1.0;
			for samples in range(8):
				new_pos = numpy.array([0.0,0.0,0.0])
				new_pos[0] = origin[0] + (samples+1)*(maxdistance / 8.0) * p[0]
				new_pos[1] = origin[1] + (samples+1)*(maxdistance / 8.0) * p[1]
				new_pos[2] = origin[2] + (samples+1)*(maxdistance / 8.0) * p[2]
				new_height = heights[(int(new_pos[1])) * 512 * 3 + (int(new_pos[0]))]
				if new_height >= new_pos[2]:
					ray_visibility *= 0
				else:
					ray_visibility *= 1
			visibility += ray_visibility
	return 1.0 - float(visibility / 16.0),normal[0],normal[1],normal[2]

w = h = chunkSize
for i in range(yBegin, yEnd):
	for j in range(xBegin, xEnd):
		heights = numpy.zeros((3*w*3*h))
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
				for k in range(h):
					for l in range(w):
						heights[((tot-tot%3)/3 * h + k) * w * 3 + l + (tot % 3 ) * w] = height[k*w+l]
				tot = tot + 1
		dirname = str(j)+ '_' +str(i)
		AOfile = "../data/" + dirname + "/AO"
		AOfiletxt = "../data/" + dirname + "/AO.txt"
		normalfile = "../data/" + dirname + "/normal"
		normalfiletxt = "../data/" + dirname + "/normal.txt"
		print AOfile
		aovalue = numpy.zeros((w*h))
		normalvalue = numpy.zeros((w*h*3))
		file_object = open(AOfiletxt, 'w')
		file_binary = open(AOfile, 'wb')
		file_normal_o = open(normalfiletxt, 'w')
		file_normal_b = open(normalfile, 'wb')
		for t in range(h):
			for s in range(w):
				aovalue[t*w+s],normalvalue[(t * w + s)*3],normalvalue[(t * w + s)*3+1],normalvalue[(t * w + s)*3+2] = GenerateAO(t+h, w+s, heights)
				file_object.write("%f " % aovalue[t*w+s])
				file_normal_o.write("%f %f %f " %(normalvalue[(t * w + s)*3],normalvalue[(t * w + s)*3+1],normalvalue[(t * w + s)*3+2]))
				#print normalvalue[i,j,:]
			file_object.write("\n")
			file_normal_o.write("\n")
		file_binary.write(struct.pack('f'*w*h,*aovalue))
		file_normal_b.write(struct.pack('f'*w*h*3,*normalvalue))
		file_object.close()
		file_binary.close()
		file_normal_o.close()
		file_normal_b.close()