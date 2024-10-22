# Hesiod

A desktop application for node-based procedural terrain generation.
- documentation: https://hesioddoc.readthedocs.io
- binaries: https://sourceforge.net/projects/hesiod

>[!WARNING] 
> Work in progress, use at your own risk!

![screenshot2_v0 2 0](https://github.com/user-attachments/assets/644263e9-425b-45f6-9ffe-349e3c7f95a3)

## License

This project is licensed under the GNU General Public License v3.0.

## Getting started

### Standalone executables for Windows and Linux

Available here: https://sourceforge.net/projects/hesiod

### Windows

#### Getting the sources

Use `PowerShell` to clone the repository (because Visual Studio won't clone the submodules) using `git` command lines:
``` bash
git clone --recurse-submodules git@github.com:otto-link/Hesiod.git
```

To get the submodules, you will need a GitHub account with its SSH key configured.

Install [Qt6](https://doc.qt.io/qt-6/windows.html) and install the missing OpenSource dependencies using `vcpkg`:
```
vcpkg install glfw3 opengl gsl glew freeglut glm libpng glm opencl assimp spdlog opencv
```

You should then be able to build the sources using Visual Studio.

### Linux

There are also required external dependencies for ubuntu you can execute:
```
sudo apt-get -y install libglfw3 libglfw3-dev libglew-dev libopengl-dev freeglut3-dev libboost-all-dev libeigen3-dev libglm-dev fuse libfuse2 ocl-icd-opencl-dev libassimp-dev
```

#### Getting the sources

Use `git` to retrieve the sources: 
``` bash
git clone git@github.com:otto-link/Hesiod.git
cd Hesiod
git submodule update --init --recursive
```

To get the submodules, you will need a GitHub account with its SSH key configured.

#### Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:
``` bash
mkdir build && cd build
cmake ..
make hesiod
```

Start the main executable:
```
bin/./hesiod
```

## Development roadmap

See https://github.com/otto-link/HighMap.

## Error: Failed to initialize_window OpenGL loader!

Try to install the package `glfw` or `glfw-devel`!

## Dependencies

- https://github.com/otto-link/HighMap / A C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.
- https://github.com/otto-link/GNode / A generic node-based data structure for node graph programming in C++.
- https://github.com/otto-link/GNodeGUI / a C++ graphical node editor library aimed at providing an interface for building and manipulating nodes in a graphical context.
- https://github.com/otto-link/Attributes / A C++ library that provides a collection of standard attributes, each paired with a corresponding Qt widget for editing their values.

- https://github.com/mgaillard/Noise / Dendry: A Procedural Model for Dendritic Patterns
