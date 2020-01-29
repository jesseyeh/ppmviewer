ppmviewer
=========

**ppmviewer** is a minimal image viewer that parses and displays PPM images using the SDL2 library. At the moment, only the P3 image format is supported.

Installation
------------

### Dependencies

* [SDL2](https://www.libsdl.org/download-2.0.php)

### Windows

1. Run `call "file\path\to\vcvarsall.bat" x64` to initialize the environment. Alternatively, to automate this step, put `call "path\to\vcvarsall.bat" x64` in a `.bat` file and add `/k path\to\your.bat` to the target field in the Command Prompt properties.
1. Replace the `SDL2_LIB` path in `build.bat`
1. Run `build.bat`
1. Ensure `SDL2.dll` is in the `build` directory

Usage
-----

`ppmviewer path\to\your\image.ppm`
