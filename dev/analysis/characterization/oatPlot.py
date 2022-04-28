import numpy as np
import matplotlib.pyplot as plt

a = np.loadtxt('a_sr.csv')
g = np.loadtxt('g_sr.csv')
m = np.loadtxt('m_sr.csv')
r = np.loadtxt('r_sr.csv')

lower_bound = 0.9*np.array(list(range(5, 505, 5)))
upper_bound = 2.1*np.array(list(range(5, 505, 5)))

plt.plot(list(range(5, 505, 5)),a,'r')
plt.plot(list(range(5, 505, 5)),g,'b')
plt.plot(list(range(5, 505, 5)),m,'m')
plt.plot(list(range(5, 505, 5)),r,'c')
plt.plot(list(range(5, 505, 5)),lower_bound,'k-')
plt.plot(list(range(5, 505, 5)),upper_bound,'k-')

plt.xlabel("set sample rate (Hz)")
plt.ylabel("actual sample rate (Hz)")
plt.legend(['a', 'g', 'm', 'r'])
plt.grid(which='both', color='k',  linestyle=':', linewidth=0.5)
plt.show()