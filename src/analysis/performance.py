import numpy as np
import matplotlib.pyplot as plt

plt.style.use('dark_background')

ai = 0
gi = 0
mi = 0
ri = 0

with open('data_new_spi.txt') as f:
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

plt.plot(data[:, 3, 0])
plt.plot(data[:, 3, 1])
plt.plot(data[:, 3, 2])
plt.plot(data[:, 4, 3])
plt.plot(time)
plt.xlabel("sample count (N)")
plt.ylabel("time (s)")
plt.legend(['a', 'g', 'm', 'r', 'all'])
plt.show()
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