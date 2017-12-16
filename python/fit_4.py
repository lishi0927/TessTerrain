import numpy as np
import math
import pylab as plt
from scipy.optimize import curve_fit
from scipy import asarray as ar,exp
from matplotlib import pyplot as py
from matplotlib.widgets import Slider

pi = math.pi
x_var = np.linspace(0.01,pi/2,100)

#def erf(x):
#	return 1 - 1/math.pow((1 + 0.278393 * x + 0.230389 * x * x + 0.000972 * x * x * x + 0.078108 * x * x * x * x),4.0)

def erf(x,c,b):
	x1 = np.abs(x-c)
	x2 = np.abs(min(x+c,pi/2))
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

def outputy(a,b):
	del y_var[:]
	for i in range(x_var.size):
		y = f(x_var[i], a, b)
		#print b
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
b0 = 4.0

outputy(a0,b0)
Approximate()

l1, = plt.plot(x_var, y_var, 'r', label= 'data')
l2, = plt.plot(x_var, y_Gaestimate, 'b', label = 'Gaussian fit')
l3, = plt.plot(x_var, y_SGestimate, 'g', label = 'SG fit')
l4, = plt.plot(x_var, y_Vmestimate, 'y', label = 'vMF fit')
plt.legend(loc= 'upper right')
plt.axis([0, 1.6, 0, 5.0])



axcolor = 'lightgoldenrodyellow'
avalue = plt.axes([0.15, 0.1, 0.65, 0.03], axisbg=axcolor)
bvalue = plt.axes([0.15, 0.05, 0.65, 0.03], axisbg=axcolor)

sa = Slider(avalue, 'a', 0.1, 1.6, valinit=a0)
sb = Slider(bvalue, 'b', 1.0, 30, valinit=b0)

def updatedata(a,b):
	outputy(a,b)
	Approximate()

def update(val):
	a = sa.val
	b = sb.val
	updatedata(a,b)
	l1.set_ydata(y_var)
	l2.set_ydata(y_Gaestimate)
	l3.set_ydata(y_SGestimate)
	l4.set_ydata(y_Vmestimate)
	fig.canvas.draw_idle()

sa.on_changed(update)
sb.on_changed(update)

plt.show()