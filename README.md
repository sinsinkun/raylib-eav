## Raylib EAV

EAV db system setup with sqlite3, and raylib for GUI.

Also running a custom UI library I'm implementing for raylib,
though its not quite modular enough (yet) to fully separate from the project.

The advantage of sqlite is that it's designed to be embedded into applications,
removing the need for running a separate MySQL instance in the background.

<img src="assets/screenshot.png" width="600x" />

## Installation (with MSYS2)
- Go through setup process to get mingw compiler working with VSCode (https://code.visualstudio.com/docs/languages/cpp)
- Search for raylib package in mingw repository `pacman -Ss raylib`
- Copy package name that matches g++ version (default is ucrt64)
- Install package with `pacman -S {...}`
- create an empty "main.db" file in the root folder
- (FYI: `-lraylib -lsqlite3` flag was added to tasks.json manually)

## Release
- This is for compiling a release version of the project, packaging dependencies into the .exe
- Will result in a longer compile time
- Download raylib sources files to default location (C:/raylib/raylib/src)
- Note: Can get raylib source code from https://www.raylib.com/
- Run `./release.bat`
