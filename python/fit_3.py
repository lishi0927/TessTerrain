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

def vMf(l,n,k):
	x = np.vdot(l, n)
	t = k / (2*pi*(np.e**(k) - np.e**(-k)))
	return t*np.e**(k*x)

h1 = [0,1,0]
c1 = 85.0 / 180 * pi
h2 = [0,math.sqrt(2) / 2, math.sqrt(2) / 2]
c2 = 45.0 / 180 * pi


l_var = []
y_var = []
N = 1000
J = 4

for i in range(N):
	theta = (random.random() - 0.5)* pi
	phi = math.acos(2 * random.random() - 1)
	l = [math.sin(theta)* math.cos(phi), math.cos(theta), math.sin(theta) * math.cos(phi)]
	l_var.append(l)
	y = (f(l, h1, c1) + f(l, h2, c2) + f(l, h3, c3) + h(l, h4, c4)) / 4.0
	y_var.append(y)

z_var = [[0 for i in range(4)] for i in range(1000)]
n_var = [[0 for i in range(3)] for i in range(4)]
k_var = [0 for i in range(4)]
a_var = [0 for i in range(4)]
for iter in range(5):
	for i in range(N):
		sum = 0
		for j in range(J):
			sum = sum + vMf(l_var[i],n_var[j],k[j])
		z[i][j] = vMf(l_var[i],n_var[j],k[j]) / sum
	for j in range(J):
		sum = 0
		sum_r = [0,0,0]
		for i in range(N):
			sum = sum + z[i][j]
			sum_r = np.add(sum_r,np.multiply(z[i][j],l_var[i])
		a_var[j] = sum / N
		r = np.divide(sum_r, sum)
		length = np.linalg.norm(r)
		k_var[j] = (3*length - length**3) / (1-length**2)
		n_var[j] = np.divide(r, length)


plt.legend(loc= 'upper right')
plt.show()