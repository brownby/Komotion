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
from scipy.interpolate import interp1d
from scipy.signal import savgol_filter

plt.style.use('dark_background')
dimensions = 6  # number of columns in data less one (time)
window_size, poly_order = 11, 3 # window must be odd

with open('data00.csv') as f:
    lines = f.readlines()
    data_raw = np.zeros((len(lines), dimensions+1))
    for indx, line in enumerate(lines):
        tmp = np.asarray(line.replace(",\n", "").split(','))
        for indy, num in enumerate(tmp):
            data_raw[indx, indy] = float(num)  # TODO - must have empty final line

xinterp = np.linspace(data_raw[:, dimensions].min(), data_raw[:, dimensions].max(), len(lines))
data_interp = interp1d(xinterp, data_raw[:, 0:dimensions], kind='linear', axis=0)
data_smooth = np.zeros_like(data_interp.y)

for indy, column in enumerate(data_interp.y.T):
    data_smooth.T[indy, :] = savgol_filter(column, window_size, poly_order)

plt.plot(xinterp, data_smooth)
plt.xlabel('time (s)')
plt.ylabel('magnitude')
plt.legend(['a_x', 'a_y', 'a_z', 'g_x', 'g_y', 'g_z'], loc='upper right')
plt.title("6-axis inertial measurement: walking")
plt.show()