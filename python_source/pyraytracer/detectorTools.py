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
import pycolortools
import numpy as np
import pandas as pd

from . import fileTools as ft
from . import signalSmoothing as sm

cie = pycolortools.CIEXYZ()


def groupSpectrum(df, bin_w=1.0):

    if df.empty:
        return(df)

    #print(df.wavelength.min(), df.wavelength.max())

    bin_n = np.floor((df.wavelength.max() - df.wavelength.min()) / bin_w)
    # Bin the data frame
    bins = np.linspace(df.wavelength.min(),
                       df.wavelength.max(),
                       bin_n)
    # print(bins)
    groups = df.groupby(np.digitize(df.wavelength, bins)).sum() / bin_w
    if(bins.size != 0):
        groups['wavelength'] = bins[groups.index.values - 1] + bin_w / 2
    else:
        groups['wavelength'] = df.wavelength.iloc[0]

    return(groups)


def sumSpectrum(df, smooth=False, bin_w=1):
    gAll = groupSpectrum(df, bin_w=bin_w)

    x = gAll['wavelength'].values
    y = gAll['radPower'].values
    if smooth:
        y = sm.savitzky_golay(y, window_size=11, order=3, deriv=0)
        # y = sm.smooth(y, window_len=11, window='hanning')

    return(x, y)


def sumSphereHalfs(fpath, orig=[0, 0, 0]):
    '''
    Sum radiant power of the both sphere halves
    '''

    # Load data
    df = ft.loadDetectorData(fpath, orig)

    dfUp = df[df['z'] > 0]
    dfDown = df[df['z'] <= 0]

    gAll = groupSpectrum(df)
    gFor = groupSpectrum(dfUp)
    gBack = groupSpectrum(dfDown)

    allpower = gAll['radPower'].sum()
    # absorbed = 100 - escaped
    forward = gFor['radPower'].sum()
    backward = gBack['radPower'].sum()

    return(allpower, forward, backward)


def convertToSphere(df, half='All', axis='z', useTrapz=True, bin_w=1):
    '''
    This function converts spherical detector data to
    that of integrating sphere.
    One can specify which half of the sphere is integrated.
    'All': Whole sphere is integrated
    'up': Upper part is integrated
    'down': Lower part is integrated

    axis will specify the axis which is considered the up direction.
    Can be x,y,z

    '''

    if half == 'All':
        (w, I) = sumSpectrum(df, bin_w=bin_w)
        (ws, Is) = sumSpectrum(df, smooth=True, bin_w=bin_w)
    elif half == 'up':
        df2 = df[df[axis] >= 0]
        (w, I) = sumSpectrum(df2, bin_w=bin_w)
        (ws, Is) = sumSpectrum(df2, smooth=True, bin_w=bin_w)
    else:
        df2 = df[df[axis] < 0]
        (w, I) = sumSpectrum(df2, bin_w=bin_w)
        (ws, Is) = sumSpectrum(df2, smooth=True, bin_w=bin_w)

    colors = cie.ciexyzFromSpectrum(w, I)
    T = cie.calculateCCT(colors)
    if useTrapz:
        radP = np.trapz(I, w)
    else:
        radP = np.sum(I)
    if(len(I) == 1):
        radP = I[0]

    out = {'radPower': radP,
           'color_x': colors[0],
           'color_y': colors[1],
           'CCT': T,
           'wavelengths': w,
           'intensities': I,
           'intensities_smooth': Is}
    return(out)


def convertToSliceHL(df,  detDistance, detDiameter, nPhi=50, sliceAx='y'):
    '''
    Converts sphere detector to slice detector. Detector at distance
    detDistance and has diameter detDiameter.
    '''
    detAng = np.degrees(np.arctan(detDiameter / 2 / detDistance) * 2)
    return(convertToSlice(df, nPhi, sliceAx, detAng))


def convertToSlice(df, nPhi=50, sliceAx='y', detAng=10.0):
    '''
    Converts sphere detector to slice detector. Sees detAng-angle of rays
    from the sphere center.
    '''
    # r of the sphere
    r = np.sqrt(
        df['z'].iloc[0] ** 2 + df['y'].iloc[0] ** 2 + df['x'].iloc[0] ** 2)

    h = r - r * np.cos(np.radians(detAng / 2))
    A = 2 * np.pi * r * h
    solidAngle = A / r ** 2

    # Select only the slice
    axLim = r * np.sin(np.radians(detAng / 2))
    df = df[df[sliceAx] >= -axLim]
    df = df[df[sliceAx] <= axLim]
    # print(df.info())

    # Spherical coords where

    if sliceAx == 'x':
        df['phi'] = np.degrees(np.arctan2(-df['y'], df['z']))
        df['theta'] = np.degrees(np.arccos(df['x'] / r)) - 90

    elif sliceAx == 'y':
        df['phi'] = np.degrees(np.arctan2(df['x'], df['z']))
        df['theta'] = np.degrees(np.arccos(df['y'] / r)) - 90
    else:
        df['phi'] = np.degrees(np.arctan2(df['y'], df['x']))
        df['theta'] = np.degrees(np.arccos(df['z'] / r)) - 90

    # Check for nans
    # print('Nans: %d' % (df.shape[0] - df['phi'].count()))
    df = df.dropna()
    # print(df['phi'].max(), df['phi'].min())
    # print(df['theta'].max(), df['theta'].min())

    # Define grid
    phig = np.linspace(-180, 180, nPhi)

    def cap(ph, th):
        rr = np.radians(detAng / 2)
        ph = np.radians(ph)
        th = np.radians(th)
        t = np.cos(ph) >= np.cos(rr)

        in_cap = np.sin(ph) ** 2 + np.sin(th) ** 2 <= np.sin(rr) ** 2

        return(np.logical_and(in_cap, t))

    dfN = pd.DataFrame(index=phig)
    radP = []
    specW = []
    specI = []
    colorX = []
    colorY = []
    CCT = []
    # Digitize to pixels
    for p in phig:
        truth = cap(df['phi'] - p, df['theta'])
        radP.extend([df['radPower'][truth].sum()])
        if(radP[-1] == 0):
            specW.extend([np.array([0])])
            specI.extend([np.array([0])])
            colorX.extend([0.0])
            colorY.extend([0.0])
            CCT.extend([0.0])
        else:
            specG = sumSpectrum(df[truth])

            w = specG[0]
            I = specG[1]
            #print(w, I)

            colors = cie.ciexyzFromSpectrum(w, I)
            T = cie.calculateCCT(colors)

            specW.extend([w])
            specI.extend([I])
            colorX.extend([colors[0]])
            colorY.extend([colors[1]])
            CCT.extend([T])

    dfN['radPower'] = radP / solidAngle
    dfN['wavelengths'] = specW
    dfN['intensities'] = specI / solidAngle
    dfN['colorX'] = colorX
    dfN['colorY'] = colorY
    dfN['CCT'] = CCT

    return(dfN)
