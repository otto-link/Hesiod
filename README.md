# Hesiod

A desktop application for node-based procedural terrain generation.
- binaries: https://sourceforge.net/projects/hesiod
- Discord: https://discord.gg/HBgFCjdV8W
- tutorial videos (WIP): https://www.youtube.com/playlist?list=PLvla2FXp5tDxbPypf_Mp66gWzX_Lga3DK
- documentation (WIP): https://hesioddoc.readthedocs.io

>[!WARNING] 
> Work in progress, use at your own risk!

https://github.com/user-attachments/assets/cc51e870-8d21-49a0-84f4-18758285e16c

<img width="1920" height="1054" alt="Screenshot_2025-09-22_09-17-19" src="https://github.com/user-attachments/assets/402a6509-3432-405f-9cda-2ac9f318c969" />

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
