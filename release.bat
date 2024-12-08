@REM Compile standalone exe with raylib source code
g++ -O2 -o build\rayeav.exe **.cpp src\**.cpp -std=c++17 -IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -static -lraylib -lsqlite3 -lopengl32 -lgdi32 -lwinmm -mwindows