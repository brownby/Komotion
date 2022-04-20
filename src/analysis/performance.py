import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

plt.style.use('dark_background')

ai = 0
gi = 0
mi = 0
ri = 0

with open('data_noFlush.csv') as f:
    lines = f.readlines()
    time = np.zeros((len(lines), 1))
    data = np.zeros((len(lines), 5, 4))
    for i, line in enumerate(lines):
        tmp = np.asarray(line.replace("\n", "").split(','))
        if 'a' in tmp:
            data[ai, 0:4, 0] = tmp[1:5]
            time[i] = tmp[4]
            ai += 1
        if 'g' in tmp:
            data[gi, 0:4, 1] = tmp[1:5]
            time[i] = tmp[4]
            gi += 1
        if 'm' in tmp:
            data[mi, 0:4, 2] = tmp[1:5]
            time[i] = tmp[4]
            mi += 1
        if 'rv' in tmp:
            data[ri, :, 3] = tmp[1:6]
            time[i] = tmp[5]
            ri += 1


i_start = 0
indx = 0
slopes_a = np.zeros(50)
slopes_g = np.zeros(50)

for i in range(len(data)):
    if data[i,3,0] > 4.98:
        result = linregress(list(range(i_start,i)),data[i_start:i,3,0])
        slope = 1/result.slope

        if not np.isnan(slope):
            slopes_a[indx] = slope
            indx = indx + 1
        i_start = i

i_start = 0
indx = 0

for i in range(len(data)):
    if data[i,3,1] > 4.97:
        result = linregress(list(range(i_start,i)),data[i_start:i,3,1])
        slope = 1/result.slope

        if not np.isnan(slope):
            slopes_g[indx] = slope
            indx = indx + 1
        i_start = i

np.savetxt("magnetometer_ag_a.csv", slopes_a, delimiter=",")
np.savetxt("magnetometer_ag_g.csv", slopes_g, delimiter=",")

lower_bound = 0.9*np.array(list(range(10,510,10)))
upper_bound = 2.1*np.array(list(range(10,510,10)))

plt.plot(list(range(10,510,10)),slopes_a,'r')
plt.plot(list(range(10,510,10)),slopes_g,'b')
plt.plot(list(range(10,510,10)),lower_bound,'w-')
plt.plot(list(range(10,510,10)),upper_bound,'w-')

plt.xlabel("set sample rate (Hz)")
plt.ylabel("actual sample rate (Hz)")
plt.legend(['a', 'g'])
plt.grid(which='both', color='w',  linestyle=':', linewidth=0.5)
plt.show()

#plt.plot(data[:, 3, 1])
#plt.plot(data[:, 3, 2])
#plt.plot(data[:, 4, 3])
#plt.plot(time)
#plt.legend(['a', 'g', 'm', 'r', 'all'])

#
# freq = np.zeros_like(data_raw)
#
# for indy, num in enumerate(data_raw):
#     if indy < len(lines)-2:
#         freq_tmp = 1/(data_raw[indy+1]-data_raw[indy])
#         if freq_tmp != float("inf"):
#             freq[indy] = 1/(data_raw[indy+1]-data_raw[indy])
#         else:
#             freq[indy] = 2000
#
# print(np.mean(freq))
#
# N = len(lines)
# bins = 50
# plt.hlines(100, 0, 3000);
# plt.plot(freq)
# #plt.hist(freq, bins)
# plt.show()