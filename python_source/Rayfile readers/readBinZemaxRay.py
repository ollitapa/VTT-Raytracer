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

fname = "LXW8.dat"
fname = 'ZemaxRays1_zemax.dat'
#fname = 'osram.dat'
fname = 'ZemaxRays1_LEDOnlySilicone.dat'
fname = 'lightOsram.RAY'

with open(fname, "rb") as f:
    byte = f.read(4)
    print('identifier', int.from_bytes(byte, byteorder='little', signed=False))
    byte = f.read(4)
    print('rays: ', int.from_bytes(byte, byteorder='little', signed=False))
    bytes = f.read(100)
    print(bytes)
    print('text len:', len(bytes))

    print('flux', struct.unpack('f', f.read(4))[0])
    print('set flux', struct.unpack('f', f.read(4))[0])
    print('wavelen[um]', struct.unpack('f', f.read(4))[0])
    print('Deg start:', struct.unpack('f', f.read(4))[0])
    print('Deg end:', struct.unpack('f', f.read(4))[0])
    print('Az star:', struct.unpack('f', f.read(4))[0])
    print('Az end:', struct.unpack('f', f.read(4))[0])
    print('Dim:', struct.unpack('i', f.read(4))[0])
    print('translation')
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print('rotation')
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print('scale')
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print('unused')
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    print(struct.unpack('f', f.read(4))[0])
    file_type = struct.unpack('i', f.read(4))[0]
    print('ray format', file_type)
    print('flux type', struct.unpack('i', f.read(4))[0])
    print('reserved', struct.unpack('i', f.read(4))[0])
    print('reserved', struct.unpack('i', f.read(4))[0])
    #print(struct.unpack('f', f.read(4))[0])
    #print(struct.unpack('f', f.read(4))[0])

    rays = 0

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
        print(struct.unpack('f', f.read(4))[0])
        print(struct.unpack('f', f.read(4))[0])
        print(struct.unpack('f', f.read(4))[0])
        print('flux', struct.unpack('f', f.read(4))[0])
        if file_type != 0:
            print('wave', struct.unpack('f', f.read(4))[0])
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
