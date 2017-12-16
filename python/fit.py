import numpy as np
import math
import pylab as plt
from scipy.optimize import curve_fit
from scipy import asarray as ar,exp

pi = math.pi
x_var = np.linspace(0, pi/2 ,50)

a_var = np.linspace(0.05,1.6, 100)

#def erf(x):
#	return 1 - 1/math.pow((1 + 0.278393 * x + 0.230389 * x * x + 0.000972 * x * x * x + 0.078108 * x * x * x * x),4.0)

def erf(x1,x2):
	a = np.linspace(x1,x2,100)
	b = np.e**(-a*a)
	return np.trapz(b,a)

def f(x, a, b):
	x_real_1 = math.tan(min(x+a,pi/2-0.1)) / b
	x_real_2 = math.tan(max(x-a,-pi/2+0.1)) / b
	t = (erf(x_real_2,x_real_1))*(2+b*b)/(2*b*pi) + b / 2/ pi * ( -x_real_1 * np.e**(-x_real_1**2) + x_real_2 * np.e**(-x_real_2**2))
	return t

def vmf(x,k):
	t = k / (2*pi*(np.e**(k) - np.e**(-k)))
	return t*np.e**(k*np.cos(x))

def Gaussian(x,u,v):
	return 1.0/math.sqrt(2*pi)/v * np.e**(-(np.cos(x) -u)**2 / 2 / v**2)

def SphericalGaussian(x,u,v):
	return v*np.e**(u*(np.cos(x) - 1))

def t(x, A, B):  
    return A * x + B

u_var = []
v_var = []

for k in range(a_var.size):
	y_var = []
	for i in range(x_var.size):
		y = f(x_var[i], a_var[k], 0.5)
		y_var.append(y)
	#popt,pcov = curve_fit(Gaussian,x_var,y_var)
	popt,pcov = curve_fit(SphericalGaussian,x_var,y_var)
	u = popt[0]
	v = popt[1]
	u_var.append(u)
	v_var.append(v)

popt,pcov = curve_fit(t,a_var,u_var)
A = popt[0]
B = popt[1]
popt,pcov = curve_fit(t,a_var,v_var)
C = popt[0]
D = popt[1]

u_estimate = A * a_var + B
v_estimate = C * a_var + D

print A,B,C,D

plt.plot(a_var, u_var, 'r',label='u data')
plt.plot(a_var, u_estimate, 'b', label = 'fit')
plt.plot(a_var, v_var, 'g',label='v data')
plt.plot(a_var, v_estimate, 'y', label = 'fit')
plt.show()

#y_estimate = []
#for i in range(x_var.size):
#	y = Gaussian(np.cos(x_var[i]),u,v)
#	y = SphericalGaussian(x_var[i],u,v)
#	y_estimate.append(y)
#plt.plot(x_var, y_var, 'r',label='data')
#plt.plot(x_var, y_estimate, 'b', label = 'fit')
#plt.show()