# VTT-Raytracer

This is a yet another ray tracer program. It may not be the fastest and may not be the easiest to understand.

# Installation

## Preinstalls for building source

### For Windows:

* Download MSYS2
https://msys2.github.io/

#### For all platforms:

Compiler must support C++11 -syntax!!! Also threading support is required:
http://en.cppreference.com/w/c/thread


Required packages:

   * make
   * g++
   * lapack
   * qhull (This is for STL-geometry creation)
   * libhdf5-devel
   * doxygen (for class documentation)
   * graphviz (for class documentation)


   * Download and install Armadillo
      * http://arma.sourceforge.net/download.html
   * Get the premake4 software: http://industriousone.com/premake Copy the executable to MSYS2 bin-directory `C:/msys64/usr/bin`

## Building Source

1. Build source. Go to ```Tracer/```

```bash
premake4 gmake
make config=release
sudo premake4 install
```
For faster executable without saving of the traced rays use:
```bash
premake4 gmake
make config=releasenorays
sudo premake4 install
```
For Windows MSYS2 use:
```bash
premake4 gmake
mingw32-make config=releasenorays
sudo premake4 --DESTDIR=/usr/bin install
```

If you want the debug parameters to print out use:

```bash
make config=debug_verbose
```

# Running the traycer

1. On Windows you should add the msys2 bin directories to system PATH. This enables the MMP API to run the tracer
   2. Go to System settings in Control Panel
   2. Open Edit Environment variables and add to PATH ```C:\msys64\usr\bin``` and ```C:\msys64\mingw64\bin``` (refer to your msys install location)
1. Create an input.json -file
1. Run tracer using the input file

```bash
tracer input.json
```
or
```bash
tracer-no-ray-save input.json
```

# Building Documentation

Check out the [wiki](https://github.com/ollitapa/VTT-Raytracer/wiki
).

For C++ class documentation we use doxygen.
On unix use command line, windows use MSYS2.

1. Goto `/doc` directory.
1. Run `doxygen`

Documentation is placed inside `/doc/html`. Open `index.html` to read it.


# Code style

For code contributions please use Google-style C++ code

https://google.github.io/styleguide/cppguide.html

https://github.com/google/styleguide/

For Python use PEP8-style. Formatter:
https://pypi.python.org/pypi/autopep8/1.1.1



# Acknowledgements

This research has received funding from the European Union's 7th Framework Programme (FP7-NMP) under NMP-2013-1.4-1 with grant agreement no. 604279