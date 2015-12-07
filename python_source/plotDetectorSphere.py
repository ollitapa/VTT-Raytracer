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
import matplotlib
# matplotlib.use('Qt4Agg')
# matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import os
import io
from scipy.interpolate import griddata
from matplotlib import cm
import pyraytracer.fileTools as ft

#from scipy.interpolate import griddata

orig = [0, 0, 0]
nPhi = 10
nTheta = 20


# Set axes equal
axesEqual = True

# Create figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
fpath = "AllDetector_1_LED.bin"

df = ft.loadDetectorData(fpath)


print(min(df['z']))
#df = df[df['z'] > 0]

# r of the sphere
r = np.sqrt(df['z'].iloc[2] ** 2 + df['y'].iloc[2] ** 2 + df['x'].iloc[2] ** 2)

# Spherical coords
df['theta'] = np.arccos(df['z'] / r)
df['phi'] = np.arctan2(df['y'], df['x']) + np.pi

df = df.dropna()


# Define grid
phig = np.linspace(0, 2 * np.pi, nPhi)
thetag = np.linspace(0, 2 * np.pi, nTheta)

wPhi = 2 * np.pi / nPhi
wTheta = 2 * np.pi / nTheta

# Digitize to pixels
df['ibinPhi'] = np.digitize(df['phi'], phig)
df['ibinTheta'] = np.digitize(df['theta'], thetag)

del df['x']
del df['y']
del df['z']
del df['wavelength']
del df['phi']
del df['theta']


# Sum pixels
df = df.groupby(['ibinPhi', 'ibinTheta']).sum().reset_index()

# Pixel coordinates
df['phi'] = df['ibinPhi'].apply(lambda x: phig[x] + wPhi / 2)
df['theta'] = df['ibinTheta'].apply(lambda x: thetag[x] + wTheta / 2)


x = df['theta'] * np.sin(df['phi'])
y = df['theta'] * np.cos(df['phi'])
z = df['radPower'].values


xi = np.linspace(min(x), max(x), 100)
yi = np.linspace(min(y), max(y), 100)
Z = griddata((x, y), z, (xi[None, :], yi[:, None]),
             method='linear', fill_value=0)

X, Y = np.meshgrid(xi, yi)


surf = ax.plot_surface(X, Y, Z, rstride=5, cstride=5,
                       cmap=plt.get_cmap('jet'),
                       linewidth=1, shade=True)


print(X[:, 5])
print(Y[:, 5])


cset = ax.contour(X, Y, Z, zdir='z', offset=-3, cmap=cm.coolwarm)

cset = ax.contour(X, Y, Z, zdir='y', offset=3, cmap=cm.coolwarm)
cset = ax.contour(X, Y, Z, zdir='x', offset=-3, cmap=cm.coolwarm)

plt.show()


plt.plot(Y[:, 5], Z[:, 5])
plt.show()
