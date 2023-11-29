# Hesiod

A desktop application for node-based procedural terrain generation.

https://github.com/otto-link/Hesiod/assets/121820229/da3b246e-18af-47d5-bfd2-197c44c670df

Color is coming!

![Screenshot_2023-11-12_23-01-20](https://github.com/otto-link/Hesiod/assets/121820229/1fb9311a-5b50-40a5-b8ad-7755b9e3571a)

## Warning 

Work in progress, use at your own risk!

## License

This project is licensed under the GNU General Public License v3.0.

## Getting started

### Windows

#### Standalone executable

Available here: https://sourceforge.net/projects/hesiod/files/Hesiod_win64_20231118_60ad80b.zip/download

#### Getting the sources

Use `PowerShell` to clone the repository (because Visual Studio won't clone the submodules) using `git` command lines:
``` bash
git clone git@github.com:otto-link/Hesiod.git
cd Hesiod
git submodule update --init --recursive
```

Install the missing OpenSource dependencies using `vcpkg`:
```
vcpkg install glfw3 opengl gsl glew freeglut eigen3 glm libpng boost-range boost-type-erasure boost-geometry glm
```

You should then be able to build the sources using Visual Studio.

### Linux

#### Getting the sources

Use `git` to retrieve the sources: 
``` bash
git clone git@github.com:otto-link/Hesiod.git
cd Hesiod
git submodule update --init --recursive
```

#### Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:
``` bash
mkdir build && cd build
cmake ..
make
```

Start the main executable:
```
bin/./hesiod
```

## Development roadmap

See https://github.com/otto-link/HighMap.

## Dependencies

- https://github.com/otto-link/HighMap / A C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.
- https://github.com/otto-link/GNode / A generic node-based data structure for node graph programming in C++

- https://github.com/ocornut/imgui / Dear ImGui: Bloat-free Graphical User interface for C++ with minimal dependencies
- https://github.com/aiekick/ImGuiFileDialog / File Dialog for Dear ImGui
- https://github.com/thedmd/imgui-node-editor / Node Editor built using Dear ImGui
- https://github.com/Raais/ImguiCandy / Color utils, Themes and other cool stuff for Dear ImGui
- https://github.com/dmcrodrigues/macro-logger / A simplified logging system using macros
- https://github.com/USCiLab/cereal / A C++11 library for serialization

