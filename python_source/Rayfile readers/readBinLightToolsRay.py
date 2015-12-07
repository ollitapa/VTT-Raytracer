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
import struct


fname = 'lightOsram.RAY'
fname = 'Luxeon.ray'
fname = 'LightTools1_lighttools.ray'

with open(fname, "rb") as f:

    bytes = f.read(4)
    print(bytes)
    print('text len:', len(bytes))
    print(struct.unpack('i', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('i', f.read(4))[0])

    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('i', f.read(4))[0])

    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])

    rays = 1

    i = 0
    while True:

        if i >= rays:
            break

        print('#### Ray')
        print('location')
        print(struct.unpack('f', f.read(4))[0])
        print(struct.unpack('f', f.read(4))[0])
        print(struct.unpack('f', f.read(4))[0])
        print('direction')
        xi = struct.unpack('f', f.read(4))[0]
        yi = struct.unpack('f', f.read(4))[0]
        zi = struct.unpack('f', f.read(4))[0]
        print(xi, yi, zi, (xi ** 2 + yi ** 2 + zi ** 2))
        print('flux', struct.unpack('f', f.read(4))[0])
        # if file_type != 0:
        #    print('wave', struct.unpack('f', f.read(4))[0])
        '''
        try:
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
            struct.unpack('f', f.read(4))[0]
        except Exception:
            print(i)
            break
        '''

        i += 1

    print(i)
