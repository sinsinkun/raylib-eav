## Basic parameters for starting a raylib project in vscode on windows

## Installation (with MSYS2)
- Go through setup process to get mingw compiler working with VSCode (https://code.visualstudio.com/docs/languages/cpp)
- Search for raylib package in mingw repository `pacman -Ss raylib`
- Copy package name that matches g++ version (default is ucrt64)
- Install package with `pacman -S {...}`
- (FYI: `-lraylib` flag was added to tasks.json manually)

## Release
- This is for compiling a release version of the project, packaging dependencies into the .exe
- Will result in a longer compile time
- Download raylib sources files to default location (C:/raylib/raylib/src)
- Note: Can get raylib source code from https://www.raylib.com/
- Run `./release.bat`
