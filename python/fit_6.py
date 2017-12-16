import numpy as np
import math
import pylab as plt
from scipy.optimize import curve_fit
from scipy import asarray as ar,exp
from matplotlib import pyplot as py
from mpl_toolkits.mplot3d import Axes3D

pi = math.pi
x_var = np.linspace(0.05, pi/2 ,100)

a_var = np.linspace(0.05, pi/2, 100)

b_var = np.linspace(1.0,100.0,100)

def erf(x,c,b):
	x1 = np.abs(x-c)
	x2 = np.abs(min(x+c,pi/2))
	if x1 == 0:
		x1 = 0.01
	a = np.linspace(x1,x2,100)
	t = (math.cos(c) - np.cos(a)* math.cos(x)) / (math.sin(x) * np.sin(a))
	for i in range(t.size):
		if t[i] >1.0:
			t[i] = 1.0
		elif t[i] < -1.0:
			t[i] = -1.0
	y = 2 * np.arccos(t) * np.e**(b*(np.cos(a) - 1)) * np.sin(a)
	return np.trapz(y,a)

def f(x, a, b):
	if x > a:
		t = erf(x,a,b)
	else:
		t = 2 * pi / b * (1 - np.e**(b*(math.cos(a-x) - 1))) + erf(x,a,b)
	return t

def SphericalGaussian(x,u,v):
	return v*np.e**(u*(np.cos(x) - 1))

u_var = []
v_var = []
y_var = []

for t in range(b_var.size):
	print b_var[t]
	for k in range(a_var.size):
		del y_var[:]
		for i in range(x_var.size):
			y = f(x_var[i], a_var[k], b_var[t])
			y_var.append(y)
		popt,pcov = curve_fit(SphericalGaussian,x_var,y_var)
		u = popt[0]
		v = popt[1]
		u_var.append(u)
		v_var.append(v)
	
ax=plt.subplot(111,projection='3d')

color = ('aliceblue', 'black', 'coral', 'darkgreen', 'darkred')

tempb = []
for t in range(b_var.size):
	for j in range(a_var.size):
		tempb.append(b_var[t])


for t in range(b_var.size):
	ax.scatter(a_var[:], tempb[t*a_var.size:(t+1)*a_var.size], u_var[t*a_var.size:(t+1)*a_var.size],color[t%5])

ax.set_zlabel('Z')
ax.set_ylabel('Y')
ax.set_xlabel('X')
plt.show()