"""
   Simple plotter
   for ES20r sensing platform
   last modified 11 May 2022

   Active Learning Labs, Electrical Engineering
   https://www.seas.harvard.edu/active-learning-labs
 """

import numpy as np
import matplotlib.pyplot as plt
import math

# import data from file
fileName = "sample_data.csv"
config = "agr"

def euler_from_quaternion(w, i, j, k):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise)
        yaw is rotation around z in radians (counterclockwise)
        """
        t0 = +2.0 * (w * i + j * k)
        t1 = +1.0 - 2.0 * (i * i + j * j)
        roll_x = math.atan2(t0, t1)
        roll_x = roll_x * 180/math.pi
     
        t2 = +2.0 * (w * j - k * i)
        t2 = +1.0 if t2 > +1.0 else t2
        t2 = -1.0 if t2 < -1.0 else t2
        pitch_y = math.asin(t2)
        pitch_y = pitch_y * 180/math.pi
     
        t3 = +2.0 * (w * k + i * j)
        t4 = +1.0 - 2.0 * (j * j + k * k)
        yaw_z = math.atan2(t3, t4)
        yaw_z = yaw_z * 180/math.pi
     
        return roll_x, pitch_y, yaw_z # in radians

data = np.genfromtxt(fileName, delimiter=',', filling_values=-1, usecols=np.arange(1, 6))

dimen = np.loadtxt(fileName, dtype=str, delimiter=',', usecols=0)
dimen_str = ['a', 'g', 'm', 'r', 'l']

# find indices (rows) for 9-axis dimensions + rotation

a_index = np.nonzero(np.char.count(dimen, dimen_str[0]))
g_index = np.nonzero(np.char.count(dimen, dimen_str[1]))
m_index = np.nonzero(np.char.count(dimen, dimen_str[2]))
r_index = np.nonzero(np.char.count(dimen, dimen_str[3]))
l_index = np.nonzero(np.char.count(dimen, dimen_str[4]))

# extract dimensions + rotation into separate arrays
# with columns x, y, z, t or r, i, j, k, t

a = data[a_index, :]
g = data[g_index, :]
m = data[m_index, :]
r = data[r_index, :]
l = data[l_index, :]

# fix this

a = a[0, :, :]
g = g[0, :, :]
m = m[0, :, :]
r = r[0, :, :]
l = l[0, :, :]

r_euler = r
for i in range(len(r[:, 0])):
    r_euler[i, 0], r_euler[i, 1], r_euler[i, 2] = euler_from_quaternion(r[i, 0], r[i, 1], r[i, 2], r[i, 3])
    r_euler[i, 3] = 0

plt.style.use('dark_background')
rows = 0
cols = 0
numPlots = len(config) + 1

if (numPlots == 1):
    rows = 1
    cols = 1
elif (numPlots == 2):
    rows = 1
    cols = 2
elif (numPlots == 4 or numPlots == 3):
    rows = 2
    cols = 2
elif (numPlots == 5 or numPlots == 6):
    rows = 3
    cols = 2

fig, axs = plt.subplots(rows, cols)
curRow = 0
curCol = 0

if('a' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(a[:,-1], a[:,0], 'r')
        axs[curRow, curCol].plot(a[:,-1], a[:,1], 'y')
        axs[curRow, curCol].plot(a[:,-1], a[:,2], 'b')
        axs[curRow, curCol].legend(["a_x", "a_y", "a_z"])
        axs[curRow, curCol].set_xlabel("time (s)")
        axs[curRow, curCol].set_ylabel("acceleration (m/s2)")
    else:
        axs[curCol].plot(a[:,-1], a[:,0], 'r')
        axs[curCol].plot(a[:,-1], a[:,1], 'y')
        axs[curCol].plot(a[:,-1], a[:,2], 'b')
        axs[curCol].legend(["a_x", "a_y", "a_z"])
        axs[curCol].set_xlabel("time (s)")
        axs[curCol].set_ylabel("acceleration (m/s2)")
    curCol += 1
    if(curCol > cols - 1):
        curCol = 0
        curRow +=1

if('l' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(l[:,-1], l[:,0], 'r')
        axs[curRow, curCol].plot(l[:,-1], l[:,1], 'y')
        axs[curRow, curCol].plot(l[:,-1], l[:,2], 'b')
        axs[curRow, curCol].legend(["l_x", "l_y", "l_z"])
        axs[curRow, curCol].set_xlabel("time (s)")
        axs[curRow, curCol].set_ylabel("acceleration (m/s2)")
    else:
        axs[curCol].plot(l[:,-1], l[:,0], 'r')
        axs[curCol].plot(l[:,-1], l[:,1], 'y')
        axs[curCol].plot(l[:,-1], l[:,2], 'b')
        axs[curCol].legend(["l_x", "l_y", "l_z"])
        axs[curCol].set_xlabel("time (s)")
        axs[curCol].set_ylabel("acceleration (m/s2)")
    curCol += 1
    if(curCol > cols - 1):
        curCol = 0
        curRow +=1

if('g' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(g[:,-1], g[:,0], 'r')
        axs[curRow, curCol].plot(g[:,-1], g[:,1], 'y')
        axs[curRow, curCol].plot(g[:,-1], g[:,2], 'b')
        axs[curRow, curCol].legend(["g_x", "g_y", "g_z"])
        axs[curRow, curCol].set_xlabel("time (s)")
        axs[curRow, curCol].set_ylabel("angular velocity (rad/s)")
    else:
        axs[curCol].plot(g[:,-1], g[:,0], 'r')
        axs[curCol].plot(g[:,-1], g[:,1], 'y')
        axs[curCol].plot(g[:,-1], g[:,2], 'b')
        axs[curCol].legend(["g_x", "g_y", "g_z"])
        axs[curCol].set_xlabel("time (s)")
        axs[curCol].set_ylabel("angular velocity (rad/s)")
    curCol += 1
    if(curCol > cols - 1):
        curCol = 0
        curRow +=1

if('m' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(m[:,-1], m[:,0], 'r')
        axs[curRow, curCol].plot(m[:,-1], m[:,1], 'y')
        axs[curRow, curCol].plot(m[:,-1], m[:,2], 'b')
        axs[curRow, curCol].legend(["m_x", "m_y", "m_z"])
        axs[curRow, curCol].set_xlabel("time (s)")
        axs[curRow, curCol].set_ylabel("magnetic field (uT)")
    else:
        axs[curCol].plot(m[:,-1], m[:,0], 'r')
        axs[curCol].plot(m[:,-1], m[:,1], 'y')
        axs[curCol].plot(m[:,-1], m[:,2], 'b')
        axs[curCol].legend(["m_x", "m_y", "m_z"])
        axs[curCol].set_xlabel("time (s)")
        axs[curCol].set_ylabel("magnetic field (uT)")
    curCol += 1
    if(curCol > cols - 1):
        curCol = 0
        curRow +=1

if('r' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(r_euler[:,-1], r_euler[:,0], 'r')
        axs[curRow, curCol].plot(r_euler[:,-1], r_euler[:,1], 'y')
        axs[curRow, curCol].plot(r_euler[:,-1], r_euler[:,2], 'b')
        axs[curRow, curCol].legend(["roll", "pitch", "yaw"])
        axs[curRow, curCol].set_xlabel("time (s)")
        axs[curRow, curCol].set_ylabel("Euler angles (deg)")
    else:
        axs[curCol].plot(r_euler[:,-1], r_euler[:,0], 'r')
        axs[curCol].plot(r_euler[:,-1], r_euler[:,1], 'y')
        axs[curCol].plot(r_euler[:,-1], r_euler[:,2], 'b')
        axs[curCol].legend(["roll", "pitch", "yaw"])
        axs[curCol].set_xlabel("time (s)")
        axs[curCol].set_ylabel("Euler angles (deg)")
    curCol += 1
    if(curCol > cols - 1):
        curCol = 0
        curRow +=1

# Plot sample linearity
legendArr = []
if('a' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(a[:,-1], list(range(0, len(a), 1)), 'r')
    else:
        axs[curCol].plot(a[:,-1], list(range(0, len(a), 1)), 'r')
    legendArr.append('a')
if('g' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(g[:,-1], list(range(0, len(g), 1)), 'y')
    else:
        axs[curCol].plot(g[:,-1], list(range(0, len(g), 1)), 'y')
    legendArr.append('g')
if('m' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(m[:,-1], list(range(0, len(m), 1)), 'b')
    else:
        axs[curCol].plot(m[:,-1], list(range(0, len(m), 1)), 'b')
    legendArr.append('m')
if('r' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(r[:,-1], list(range(0, len(r), 1)), 'g')
    else:
        axs[curCol].plot(r[:,-1], list(range(0, len(r), 1)), 'g')
    legendArr.append('r')
if('l' in config):
    if (numPlots > 2):
        axs[curRow, curCol].plot(l[:,-1], list(range(0, len(l), 1)), 'w')
    else:
        axs[curCol].plot(l[:,-1], list(range(0, len(l), 1)), 'w')
    legendArr.append('l')
if (numPlots > 2):
    axs[curRow, curCol].legend(legendArr)
    axs[curRow, curCol].set_xlabel("time (s)")
    axs[curRow, curCol].set_ylabel("sample count")
else:
    axs[curCol].legend(legendArr)
    axs[curCol].set_xlabel("time (s)")
    axs[curCol].set_ylabel("sample count")

plt.show()
