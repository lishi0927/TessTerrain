import numpy as np
import math
import pylab as plt
from scipy.optimize import curve_fit
from scipy import asarray as ar,exp
from matplotlib import pyplot as py
from matplotlib.widgets import Slider

pi = math.pi
x_var = np.linspace(0.05, pi/2 ,100)

a_var = np.linspace(0.05, pi/2, 100)

b_var = np.linspace(1.0,100.0,1)

#def erf(x):
#	return 1 - 1/math.pow((1 + 0.278393 * x + 0.230389 * x * x + 0.000972 * x * x * x + 0.078108 * x * x * x * x),4.0)

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
y_var = []

def outputy(b):
	del u_var[:]
	del v_var[:]
	for k in range(a_var.size):
		del y_var[:]
		for i in range(x_var.size):
			y = f(x_var[i], a_var[k], b)
			#print b
			y_var.append(y)
		popt,pcov = curve_fit(SphericalGaussian,x_var,y_var)
		u = popt[0]
		v = popt[1]
		u_var.append(u)
		v_var.append(v)

fig = plt.figure()
ax = fig.add_subplot(111)
plt.subplots_adjust(left=0.15, bottom=0.25)
#a0 = 0.5
b0 = 1.0

outputy(b0)

l1, = plt.plot(a_var, u_var, 'r', label = 'u_var')
l2, = plt.plot(a_var, v_var, 'b', label = 'v_var')
plt.legend(loc= 'upper right')
plt.axis([0, 1.6, 0, 100.0])

axcolor = 'lightgoldenrodyellow'
#avalue = plt.axes([0.15, 0.1, 0.65, 0.03], axisbg=axcolor)
bvalue = plt.axes([0.15, 0.1, 0.65, 0.03], axisbg=axcolor)

#sa = Slider(avalue, 'a', 0.1, 1.6, valinit=a0)
sb = Slider(bvalue, 'b', 0.0, 100, valinit=b0)

def updatedata(b):
	outputy(b)

def update(val):
	b = sb.val
	updatedata(b)
	l1.set_ydata(u_var)
	l2.set_ydata(v_var)
	fig.canvas.draw_idle()

sb.on_changed(update)

plt.show()



#for j in range(b_var.size):
#	del u_var[:]
#	del v_var[:]
#	for k in range(a_var.size):
#		y_var = []
#		for i in range(x_var.size):
#			y = f(x_var[i], a_var[k], 1)
#			y_var.append(y)
		#popt,pcov = curve_fit(Gaussian,x_var,y_var)
#		popt,pcov = curve_fit(SphericalGaussian,x_var,y_var)
#		u = popt[0]
#		v = popt[1]
#		u_var.append(u)
#		v_var.append(v)

	#popt,pcov = curve_fit(t,a_var,u_var)
	#A = popt[0]
	#B = popt[1]
	#popt,pcov = curve_fit(t,a_var,v_var)
	#C = popt[0]
	#D = popt[1]

	#A_var.append(A)
	#B_var.append(B)
	#C_var.append(C)
	#D_var.append(D)

	#u_estimate = A * a_var + B
	#v_estimate = C * a_var + D

	#print A,B,C,D

#plt.plot(a_var, u_var, 'r',label='u data')
#plt.plot(a_var, u_estimate, 'b', label = 'fit')
#plt.plot(a_var, v_var, 'g',label='v data')
#plt.plot(a_var, v_estimate, 'y', label = 'fit')
#plt.show()

#plt.plot(b_var, A_var, 'r',label='A data')
#plt.plot(b_var, B_var, 'b',label='B data')
#plt.plot(b_var, C_var, 'g',label='C data')
#plt.plot(b_var, D_var, 'y',label='D data')
#plt.legend(loc= 'upper right')
#plt.show()

#y_estimate = []
#for i in range(x_var.size):
#	y = Gaussian(np.cos(x_var[i]),u,v)
#	y = SphericalGaussian(x_var[i],u,v)
#	y_estimate.append(y)
#plt.plot(x_var, y_var, 'r',label='data')
#plt.plot(x_var, y_estimate, 'b', label = 'fit')
#plt.show()