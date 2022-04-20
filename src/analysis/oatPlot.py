import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

plt.style.use('dark_background')

a = np.loadtxt('accelerometer_slope.csv')
g = np.loadtxt('gyroscope_slope.csv')
m = np.loadtxt('magnetometer_slope.csv')
r = np.loadtxt('rotation_slope.csv')

lower_bound = 0.9*np.array(list(range(10,510,10)))
upper_bound = 2.1*np.array(list(range(10,510,10)))

plt.plot(list(range(10,510,10)),a,'r')
plt.plot(list(range(10,510,10)),g,'b')
plt.plot(list(range(10,510,10)),m,'m')
plt.plot(list(range(10,510,10)),r,'c')
plt.plot(list(range(10,510,10)),lower_bound,'w-')
plt.plot(list(range(10,510,10)),upper_bound,'w-')

plt.xlabel("set sample rate (Hz)")
plt.ylabel("actual sample rate (Hz)")
plt.legend(['a', 'g', 'm', 'r'])
plt.grid(which='both', color='w',  linestyle=':', linewidth=0.5)
plt.show()