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

from distutils.core import setup
import os
import sys


if sys.version_info <= (2,):
    print('Python 2.x required!')
    sys.exit()


def is_package(path):
    return (
        os.path.isdir(path) and
        os.path.isfile(os.path.join(path, '__init__.py'))
    )


def find_packages(path, base=""):
    """ Find all packages in path """
    packages = {}
    for item in os.listdir(path):
        dirWD = os.path.join(path, item)
        if is_package(dirWD):
            if base:
                module_name = "%(base)s.%(item)s" % vars()
            else:
                module_name = item
            packages[module_name] = dirWD
            packages.update(find_packages(dirWD, module_name))
    return packages


packages = find_packages(".")

setup(name='pyraytracer',
      version='1.0',
      description='Plotting tools for MMPRaytracer',
      author='Olli Tapaninen',
      author_email='olli.tapaninen@gmail.com',
      package_dir=packages,
      packages=packages.keys(),
      requires=['numpy', 'pandas', 'matplotlib', 'colorTools'],
      scripts=[]
      )
