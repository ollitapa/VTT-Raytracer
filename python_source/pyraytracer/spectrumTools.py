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
import numpy as np
from scipy.stats import norm


def generateLEDspectrum(domWave, fwhm, wavelengths, power=1.0, cut=True):
    '''
    Genarate gaussian shape LED spectrum.

    if cut=True, all 0 intensities are cut out.

    '''
    variance = fwhm / (2 * np.sqrt(2 * np.log(2)))

    rv = norm(loc=domWave, scale=variance)
    intens = rv.pdf(wavelengths) * power
    intens[intens < 1e-3] = 0

    if cut:
        truth = intens != 0

        wavelengths = wavelengths[truth]
        intens = intens[truth]

    return(wavelengths, intens)


def generateAttennuationSpectrum(transmissivity, thickness):
    '''
    Function that generated attenuation coefficients of Beer-Lambert medium.

    transmissivity : array (or single number) of transmissivity [0,1] at
                     different wavelengths.
    thickness      : thickness of the sample that the transmission was measured.

    '''

    att = -np.log(transmissivity) / thickness
    return(att)
