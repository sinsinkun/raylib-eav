@REM Compile standalone exe with raylib source code
g++ -O2 -o main.exe **.cpp -std=c++17 -IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -static -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows