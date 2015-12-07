#
# Copyright 2015 VTT Technical Research Center of Finland
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import matplotlib.pylab as plt
import pandas as pd
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import os
import io
import pyraytracer.fileTools as ft


# Set axes equal
axesEqual = False

# Create figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
fpath = "AllDetector_1_LED.bin"

df = ft.loadDetectorData(fpath)


# plot
plt.plot(df['x'], df['y'], df['z'], 'x')

# Equalize axes (approximately, maybe)
if axesEqual:
    axLim = ax.get_w_lims()
    MAX = np.max(axLim)
    for direction in (-1, 1):
        for point in np.diag(direction * MAX * np.array([1, 1, 1])):
            ax.plot([point[0]], [point[1]], [np.abs(point[2])], 'w')


plt.show()
