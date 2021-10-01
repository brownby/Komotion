"""
   Parser
   for AP10 sensing platform

   Parses data from CSV file

   written for python 3.8
   modified 1 October 2021
   by J. Evan Smith

   Active Learning Labs, Electrical Engineering
   https://www.seas.harvard.edu/active-learning-labs
 """

import numpy as np
import matplotlib.pyplot as plt

dimensions = 6  # number of columns in data less one (time)

with open('data_005.txt') as f:
    lines = f.readlines()
    data_raw = np.zeros((len(lines), dimensions+1))
    for indx, line in enumerate(lines):
        tmp = np.asarray(line.replace(",\n", "").split(','))
        for indy, num in enumerate(tmp):
            data_raw[indx, indy] = float(num)  # TODO - must have empty final line