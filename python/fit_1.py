import numpy as np
import math
import pylab as plt
from scipy.optimize import curve_fit
from scipy import asarray as ar,exp
from matplotlib import pyplot as py
from matplotlib.widgets import Slider

pi = math.pi
x_var = np.linspace(0, pi/2 ,50)

#def erf(x):
#	return 1 - 1/math.pow((1 + 0.278393 * x + 0.230389 * x * x + 0.000972 * x * x * x + 0.078108 * x * x * x * x),4.0)

def erf(x1,x2):
	a = np.linspace(x1,x2,100)
	b = np.e**(-a*a)
	return np.trapz(b,a)

def f(x, a, b):
	x_real_1 = math.tan(min(x+a,pi/2-0.1)) / b
	x_real_2 = math.tan(max(x-a,-pi/2+0.1)) / b
	t = (erf(x_real_2,x_real_1))*(2+b*b)/(2*b*pi) + b / 2 / pi * ( -x_real_1 * np.e**(-x_real_1**2) + x_real_2 * np.e**(-x_real_2**2))
	t = t * (2 * b * math.sqrt(pi)) / (2+b*b)
	return t

def vmf(x,k):
	t = k / (2*pi*(np.e**(k) - np.e**(-k)))
	return t*np.e**(k*np.cos(x))

def Gaussian(x,u,v):
	return 1.0/math.sqrt(2*pi)/ v * np.e**(-(np.cos(x)-u)**2 / 2 / v**2)

def SphericalGaussian(x,u,v):
	return v*np.e**(u*(np.cos(x) -1))

y_var = []
y_Gaestimate = []
y_SGestimate = []
y_Vmestimate = []

def outputy(a):
	del y_var[:]
	for i in range(x_var.size):
		y = f(x_var[i], a, 0.5)
		y_var.append(y)


def Approximate():
	popt,pcov = curve_fit(Gaussian,x_var,y_var)
	gu = popt[0]
	gv = popt[1]
	popt,pcov = curve_fit(SphericalGaussian,x_var,y_var)
	sgu = popt[0]
	sgv = popt[1]
	popt,pcov = curve_fit(vmf,x_var,y_var)
	ku = popt[0]
	#kv = popt[1]
	#print u,v,pcov
	del y_Gaestimate[:]
	del y_SGestimate[:]
	del y_Vmestimate[:]
	for i in range(x_var.size):
		y = Gaussian(x_var[i],gu,gv)
		y_Gaestimate.append(y)
		y = SphericalGaussian(x_var[i],sgu,sgv)
		y_SGestimate.append(y)
		y = vmf(x_var[i], ku)
		y_Vmestimate.append(y)

fig = plt.figure()
ax = fig.add_subplot(111)
plt.subplots_adjust(left=0.15, bottom=0.25)
a0 = 0.5

outputy(a0)
Approximate()

l1, = plt.plot(x_var, y_var, 'r', label= 'data')
l2, = plt.plot(x_var, y_Gaestimate, 'b', label = 'Gaussian fit')
l3, = plt.plot(x_var, y_SGestimate, 'g', label = 'SG fit')
l4, = plt.plot(x_var, y_Vmestimate, 'y', label = 'vMF fit')
plt.legend(loc= 'upper right')
plt.axis([0, 1, 0, 1.6])



axcolor = 'lightgoldenrodyellow'
avalue = plt.axes([0.15, 0.1, 0.65, 0.03], axisbg=axcolor)

sa = Slider(avalue, 'a', 0.1, 1.6, valinit=a0)

def updatedata(a):
	outputy(a)
	Approximate()

def update(val):
	a = sa.val
	updatedata(a)
	l1.set_ydata(y_var)
	l2.set_ydata(y_Gaestimate)
	l3.set_ydata(y_SGestimate)
	l4.set_ydata(y_Vmestimate)
	fig.canvas.draw_idle()

sa.on_changed(update)

plt.show()