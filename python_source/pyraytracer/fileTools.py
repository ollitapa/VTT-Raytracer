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
import pandas as pd
import numpy as np
import os
import io


def genArray(text):
    # Function for generating np arrays from [1 1 2 3] type text.
    t = text[1:-1].replace('\n', ' ').replace('\r', ' ').replace('\r\n', ' ')
    return np.genfromtxt(io.BytesIO(t.encode()))


def loadDetectorDataBin(fpath):

    f = open(fpath, "rb")
    f.seek(314, os.SEEK_SET)  # seek past header

    dtypes = np.dtype([('radPower', 'f8'),
                       ('wavelength', 'f8'),
                       ('location', 'f8', (3,)),
                       ('direction', 'f8', (3,)),
                       ('surface', 'S8')
                       ])
    data = np.fromfile(f, dtype=dtypes)
    f.close()
    df = pd.DataFrame(data.tolist(), columns=data.dtype.names)
    return(df)


def loadDetectorData(filepath, orig=[0, 0, 0]):
    if(filepath[-4:] == ".bin"):
        df = loadDetectorDataBin(filepath)
    else:
        df = pd.read_csv(filepath,
                         converters={'location': genArray,
                                     'direction': genArray})

    points = np.array(list(df['location'].values))
    # print(points)
    # Center coordinates
    df['x'] = points[:, 0] - orig[0]
    df['y'] = points[:, 1] - orig[1]
    df['z'] = points[:, 2] - orig[2]
    del df['location']

    return(df)
