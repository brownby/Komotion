"""
   Simple plotter
   for AP10 sensing platform
   Parses data from CSV file,
   interpolates samples, applies smoothing, and plots
   written for python 3.8
   modified 1 October 2021
   by J. Evan Smith
   Active Learning Labs, Electrical Engineering
   https://www.seas.harvard.edu/active-learning-labs
 """

import numpy as np
import matplotlib.pyplot as plt

# import data from file
fileName = "data06.csv"
dimString = "agm"

data = np.genfromtxt(fileName, delimiter=',', filling_values=-1, usecols=np.arange(1, 6))
indexOfInterest = 4

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

plt.style.use('dark_background')
legendArr = []

fig, axs = plt.subplots(2,2)

if('a' in dimString):
    axs[0,0].plot(a[:,-1], a[:,0], 'r')
    axs[0,0].plot(a[:,-1], a[:,1], 'y')
    axs[0,0].plot(a[:,-1], a[:,2], 'b')
    axs[0,0].legend(["a_x", "a_y", "a_z"])
    axs[0,0].set_xlabel("time (s)")
    axs[0,0].set_ylabel("acceleration (m/s2)")

if('g' in dimString):
    axs[0,1].plot(g[:,-1], g[:,0], 'r')
    axs[0,1].plot(g[:,-1], g[:,1], 'y')
    axs[0,1].plot(g[:,-1], g[:,2], 'b')
    axs[0,1].legend(["g_x", "g_y", "g_z"])
    axs[0,1].set_xlabel("time (s)")
    axs[0,1].set_ylabel("angular velocity (rad/s)")

if('m' in dimString):
    axs[1,0].plot(a[:,-1], a[:,0], 'r')
    axs[1,0].plot(a[:,-1], a[:,1], 'y')
    axs[1,0].plot(a[:,-1], a[:,2], 'b')
    axs[1,0].legend(["m_x", "m_y", "m_z"])
    axs[1,0].set_xlabel("time (s)")
    axs[1,0].set_ylabel("magnetic field (uT)")

plt.show()
