import numpy as np
import math
import pylab as plt
from scipy.optimize import curve_fit
import random

pi = math.pi

   
#A = -4.61345546605   
#B = 6.92328570533
#C = 0.652905385736
#D = 0.591273515636 
   
A = -0.565743313431
B = 1.52605337937 
C = 0.0240395073981
D = 0.2902444322

def Gaussian(x,u,v):
	return 1.0/math.sqrt(2*pi)/v * np.e**(-((x) -u)**2 / 2 / v**2)

def SGaussian(x,u,v):
	return v*np.e**(u*(x-1))

def f(l,h,c):
	u = A * c + B
	v = C * c + D
	x = np.vdot(l, h)
	return Gaussian(x,u,v)

h1 = [0,1,0]
c1 = 85.0 / 180 * pi
h2 = [0,math.sqrt(2) / 2, math.sqrt(2) / 2]
c2 = 45.0 / 180 * pi
h = np.add(h1,h2) / 2
length = np.linalg.norm(h)
h = h / length
#print h,length

x_var = []
x_h1 = []
x_h2 = []
y_var = []
y_h1 = []
y_h2 = []

for i in range(10000):
	theta = (random.random() - 0.5)* pi
	phi = math.acos(2 * random.random() - 1)
	l = [math.sin(theta)* math.cos(phi), math.cos(theta), math.sin(theta) * math.cos(phi)]
	x = np.dot(l,h)
	x_var.append(x)
	y1 = f(l,h1,c1)
	x_h1.append(np.dot(l,h1))
	y_h1.append(y1)
	y2 = f(l,h2,c2)
	x_h2.append(np.dot(l,h2))
	y_h2.append(y2)
	y = (y1 + y2) / 2 - f(l, h, (c1+c2)/2)
	y_var.append(y)

plt.plot(x_var, y_var, 'ro',label='estimate')
#plt.plot(x_h1, y_h1, 'bo', label = 'h1')
#plt.plot(x_h2, y_h2, 'go',label='h2')
plt.legend(loc= 'upper right')
plt.show()