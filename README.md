# SCGL · an OpenGL 3 renderer for SimCity 4

[![MSBuild](https://github.com/nsgomez/scgl/actions/workflows/msbuild.yml/badge.svg)](https://github.com/nsgomez/scgl/actions/workflows/msbuild.yml)

**SCGL** (pronounced *sigil*) replaces SimCity 4's OpenGL hardware rendering engine with an implementation targeting
OpenGL 3.0. Its goal is to fix incompatibility with modern graphics drivers and Windows versions. It also addresses
performance shortcomings in the original OpenGL driver.

This project is not affiliated with or endorsed by EA Games.

## Building and installing

Install [Visual Studio 2022](https://visualstudio.microsoft.com/#vs-section) or later with the desktop C++ components.
Then use the solution and project files in the repository root to build SCGL. The default target is Debug x86, which
will create a Debug folder in the repository containing a `SCGL.dll` build.

Note: Debug builds will run slower due to additional logging and error checking. 

## Third-party components

SCGL depends on these third-party projects, which are already included in the `vendor` folder:

* [gzcom-dll](https://github.com/nsgomez/gzcom-dll) (Expat License) to register the graphics implementation with the game.
* [Scion](https://github.com/nsgomez/scion) (LGPLv2.1) for compatibility with game components.

## License

This project is licensed under the [GNU Lesser General Public License, version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).
You may dynamically link it with proprietary software such as SimCity 4, but changes you make to SCGL must also be
shared under the LGPLv2.1 license.
