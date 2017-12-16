import math
import numpy
import sys

#angle_vec1 = [30.0,0.0]
#angle_vec2 = [15,45]

#angle_o = [80,120]

PI = math.pi
def transformangle(angle):
	ret  = [math.sin(angle[0] / 180.0 * PI)*math.cos(angle[1] /180.0 * PI),math.cos(angle[0] / 180.0 * PI),math.sin(angle[0] /180.0 * PI)*math.sin(angle[1] /180.0 *  PI)]
	return ret

def dot(vec1,vec2):
	return vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2]

def halfangle(vec1, vec2):
	scale = 2 * dot(vec1,vec2)
	ret = [scale * vec1[0], scale * vec1[1],scale * vec1[2]]
	ret = [ret[0] - vec2[0],ret[1] - vec2[1],ret[2] - vec2[2]]
	return ret

def lengthvec(vec):
	return math.sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2])

def halfvector(vec1,vec2):
	return [(vec1[0] + vec2[0]) / 2,(vec1[1] + vec2[1]) / 2,(vec1[2] + vec2[2]) / 2]




def compute(angle_o,angle_vec1,angle_vec2):
	vec1 = transformangle(angle_vec1)
	vec2 = transformangle(angle_vec2)
	vec = halfvector(vec1, vec2)
	o = transformangle(angle_o)
	new_vec1 = halfangle(vec1,o)
	new_vec2 = halfangle(vec2,o)
	if new_vec1[1] < 0:
		return 0
	if new_vec2[1]< 0:
		return 0
	new_vec = halfvector(new_vec1, new_vec2)
	ret = abs(lengthvec(new_vec)-lengthvec(vec))
	if ret > 0.5:
		print dot(vec1,vec2),dot(new_vec1,new_vec2),lengthvec(vec),dot(o,vec)/lengthvec(vec)
	return ret 

max = 0.0

for t in range(10):
	for s in range(40):
		angle_o = [s*(90 / 10),t * (360 / 40)]
		for a in range(10):
			for b in range(40):
				angle_vec1 = [a*(90 / 10),b * (360 / 40)] 
				for c in range(10):
					for d in range(40):
						angle_vec2 = [c*(90 / 10),d * (360 / 40)]
						result = compute(angle_o,angle_vec1,angle_vec2)
						if result > max:
							max = result
							print max

angle_vec1 = [36,198]
angle_vec2 = [45,0]
angle_o = [9,0]
result = compute(angle_o,angle_vec1,angle_vec2)
print result