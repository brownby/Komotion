import numpy as np
import matplotlib.pyplot as plt

plt.style.use('dark_background')

with open('data.txt') as f:
    lines = f.readlines()
    data_raw = np.zeros((len(lines), 1))
    for indx, line in enumerate(lines):
        tmp = np.asarray(line.replace(",\n", "").split(','))
        for indy, num in enumerate(tmp):
            if indy == 6:  # 3, 6, 9
                data_raw[indx] = float(num)  # TODO - must have empty final line (?)

freq = np.zeros_like(data_raw)

for indy, num in enumerate(data_raw):
    if indy < len(lines)-2:
        freq_tmp = 1/(data_raw[indy+1]-data_raw[indy])
        if freq_tmp != float("inf"):
            freq[indy] = 1/(data_raw[indy+1]-data_raw[indy])
        else:
            freq[indy] = 2000

print(np.mean(freq))

N = len(lines)
bins = 50
plt.hlines(100, 0, 3000);
plt.plot(freq)
#plt.hist(freq, bins)
plt.show()