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

from pyraytracer import mieGenerator as mie
from pyraytracer import scatteringTools as sc
from scipy.stats import lognorm

if __name__ == '__main__':

    # Wavelengths (nm) and particle sizes (um)
    w = np.linspace(100, 1100, 501)
    p = np.linspace(3, 35, 50)

    # Log norm parameters from D10, D50 and D90 parameters
    particle_mu, particle_sigma = sc.fitLogNormParticleDistribution(
        D10=7.3, D50=12.0, D90=18.3)

    print('mu: %.6f\nsigma: %.6f' % (particle_mu, particle_sigma))

    # Calculate particle distribution
    N = lognorm(particle_sigma, scale=np.exp(particle_mu))

    # Weight factors of each particle size
    pdf = N.pdf(p)
    pdf /= pdf.sum()
    p_weights = dict(zip(p, pdf))

    # Generate mie
    df = mie.generateMieDataEffective(wavelengths=w,
                                      p_normed_weights_dict=p_weights,
                                      number_of_rvs=1000,
                                      number_of_theta_angles=90,
                                      n_particle=1.83,
                                      n_silicone=1.55,
                                      p_diameters=p)

    # Save mie
    mie.saveMieDataToHDF5(df_list=[df],
                          particle_diameters=[1],
                          wavelengths=w,
                          out_fname="mieData.hdf5")
