import numpy as np
import matplotlib.pyplot as plt

a = np.loadtxt('a400_5s.csv')
g = np.loadtxt('g250_5s.csv')
m = np.loadtxt('m100_5s.csv')
r = np.loadtxt('r250_5s.csv')

print(np.corrcoef(a))
print(np.corrcoef(g))
print(np.corrcoef(m))
print(np.corrcoef(r))

plt.plot(a, list(range(1, len(a)+1, 1)), 'r')
plt.plot(g, list(range(1, len(g)+1, 1)), 'b')
plt.plot(m, list(range(1, len(m)+1, 1)), 'm')
plt.plot(r, list(range(1, len(r)+1, 1)), 'c')

plt.xlabel("time (seconds)")
plt.ylabel("sample count")
plt.legend(['a', 'g', 'm', 'r'])
plt.grid(which='both', color='k',  linestyle=':', linewidth=0.5)
plt.show()