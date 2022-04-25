from ctypes import sizeof
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

fileName = 'data_a250_r100_v2.csv'
includeRotate = 1

# sweep parameters
sr_start = 10
sr_stop = 200
sr_step = 10
N = len(list(range(sr_start, sr_stop, sr_step)))  # number of runs

# import data from file

if includeRotate:
    data = np.genfromtxt(fileName, delimiter=',', filling_values=-1, usecols=np.arange(1, 6))
else:
    data = np.loadtxt(fileName, delimiter=',', usecols=np.arange(1, 5))

dimen = np.loadtxt(fileName, dtype=str, delimiter=',', usecols=0)
dimen_str = ['a', 'g', 'm', 'r']

# find indices (rows) for 9-axis dimensions + rotation

a_index = np.nonzero(np.char.count(dimen, dimen_str[0]))
g_index = np.nonzero(np.char.count(dimen, dimen_str[1]))
m_index = np.nonzero(np.char.count(dimen, dimen_str[2]))
r_index = np.nonzero(np.char.count(dimen, dimen_str[3]))

# extract dimensions + rotation into separate arrays
# with columns x, y, z, t or r, i, j, k, t

a = data[a_index, :]
g = data[g_index, :]
m = data[m_index, :]
r = data[r_index, :]

# fix this

a = a[0, :, :]
g = g[0, :, :]
m = m[0, :, :]
r = r[0, :, :]

# allocate arrays for runs (average sample rate)

a_sr = np.zeros(N)
g_sr = np.zeros(N)
m_sr = np.zeros(N)
r_sr = np.zeros(N)

i0 = 0
run = 0
flag = 1
for i, t in enumerate(a[:, 3]):
    if t < 0.2:
        i0 = i
        flag = 1
    if 5 - t < 0.2 and flag:
        regression = linregress(a[i0:i, 3], list(range(i0, i)))
        a_sr[run] = regression.slope
        run += 1
        i0 = 0
        flag = 0

i0 = 0
run = 0
flag = 1
for i, t in enumerate(g[:, 3]):
    if t < 0.2:
        i0 = i
        flag = 1
    if 5 - t < 0.2 and flag:
        regression = linregress(g[i0:i, 3], list(range(i0, i)))
        g_sr[run] = regression.slope
        run += 1
        i0 = 0
        flag = 0

i0 = 0
run = 0
flag = 1
for i, t in enumerate(m[:, 3]):
    if t < 0.1:
        i0 = i
        flag = 1
    if 5 - t < 0.1 and flag:
        regression = linregress(m[i0:i, 3], list(range(i0, i)))
        m_sr[run] = regression.slope
        run += 1
        i0 = 0
        flag = 0

i0 = 0
run = 0
flag = 1
for i, t in enumerate(r[:, 4]):
    if t < 0.1:
        i0 = i
        flag = 1
    if 5 - t < 0.1 and flag:
        regression = linregress(r[i0:i, 4], list(range(i0, i)))
        r_sr[run] = regression.slope
        run += 1
        i0 = 0
        flag = 0

lower_bound = 0.9 * np.array(list(range(sr_start, sr_stop, sr_step)))
upper_bound = 2.1 * np.array(list(range(sr_start, sr_stop, sr_step)))

plt.style.use('dark_background')

plt.plot(list(range(sr_start, sr_stop, sr_step)), a_sr, 'r')
plt.plot(list(range(sr_start, sr_stop, sr_step)), g_sr, 'b')
plt.plot(list(range(sr_start, sr_stop, sr_step)), m_sr, 'm')
plt.plot(list(range(sr_start, sr_stop, sr_step)), r_sr, 'c')
plt.plot(list(range(sr_start, sr_stop, sr_step)), lower_bound, 'w-')
plt.plot(list(range(sr_start, sr_stop, sr_step)), upper_bound, 'w-')

plt.xlabel("set sample rate (Hz)")
plt.ylabel("actual sample rate (Hz)")
plt.legend(['a', 'g', 'm', 'r'])
plt.grid(which='both', color='w', linestyle=':', linewidth=0.5)
plt.show()

# alternate plot

#plt.style.use('dark_background')
#plt.plot(a[:,3], list(range(0,len(a))),'r-')
#plt.xlabel("time (s)")
#plt.ylabel("samples")
#plt.show()