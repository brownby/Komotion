import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
from scipy.signal import savgol_filter

plt.style.use('dark_background')

with open('jumpShot_agm.txt') as f:
    lines = f.readlines()
    data_raw = np.zeros((len(lines), 1))
    for indx, line in enumerate(lines):
        tmp = np.asarray(line.replace(",\n", "").split(','))
        for indy, num in enumerate(tmp):
            if indy == 9:  # 3, 6, 9
                data_raw[indx] = float(num)  # TODO - must have empty final line (?)

freq = np.zeros_like(data_raw)

for indy, num in enumerate(data_raw):
    if indy < len(lines)-1:
        freq_tmp = 1/(data_raw[indy+1]-data_raw[indy])
        if freq_tmp != float("inf"):
            freq[indy] = 1/(data_raw[indy+1]-data_raw[indy])

N = len(lines)
bins = 200

plt.hist(freq, bins)
plt.show()