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

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import glob
from matplotlib import rcParams
import pyraytracer.detectorTools as dt
import pyraytracer.fileTools as ft
pd.options.display.mpl_style = 'default'
from brewer2mpl import qualitative

rcParams['figure.figsize'] = (10, 6)
rcParams['figure.dpi'] = 150
rcParams['lines.linewidth'] = 2
#rcParams['axes.facecolor'] = 'white'
rcParams['font.size'] = 14
#rcParams['patch.facecolor'] = dark2_colors[0]
rcParams['axes.color_cycle'] = qualitative.Paired[12].mpl_colors


######################################
# Options

# Sphere origin
orig = [0, 0, 0]
# Gridding, number of pixels in slice
nPhi = 50
# Detector filename
fname = "AllDetector_1_LED.bin"
# Axis of the slice
sliceAx = 'y'
# Detector width, anlge that the detector sees. [deg]
detAng = 5.0
######################################

df = ft.loadDetectorData(fname)

gp = dt.convertToSlice(df, nPhi=nPhi, detAng=detAng)


fig = plt.figure()
ax = fig.add_subplot(111)
# Sum pixels
gg = gp['radPower']
gg.plot()

plt.xlabel('Angle on slice, deg. (Axis %s)' % sliceAx)
plt.ylabel('W/sr')
plt.legend()
plt.show(block=True)
