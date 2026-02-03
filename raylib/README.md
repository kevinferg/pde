# Raylib Setup

1. Download raylib from [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib).
2. Extract the `include` and `lib` directories from the raylib repository into this folder (`raylib/`).
3. Build the project using the `mingw32-make` or `make` command. Raylib for Windows requires the most recent version of MinGW-w64.


Structure should match the following:

```
\---raylib
    |   README.md
    |   
    +---include
    |       raylib.h
    |       raymath.h
    |       rlgl.h
    |       
    \---lib
            libraylib.a
            libraylibdll.a  <-- optional
            raylib.dll      <-- optional
```