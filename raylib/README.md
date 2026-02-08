# Raylib Setup

### Requirements:
- [MinGW-w64](https://github.com/skeeto/w64devkit/releases) (for Windows)
- [raylib](https://github.com/raysan5/raylib/releases)
- [emscripten SDK](https://github.com/emscripten-core/emsdk) (for Web)  

### Instructions

1. Download raylib from [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib).
2. Extract the `include` and `lib` directories from the raylib repository into this folder (`raylib/`).
3. Build the project using the `mingw32-make` or `make` command. Raylib for Windows requires the most recent version of MinGW-w64.



### Compiling to webassembly

Setup:
- Set up emscripten emsdk
- Compile raylib's libraylib.a from source using emcc and put in `raylib_web/lib`
- Refactor main loop for browser with `emscripten_set_main_loop[_arg]()`

Then (in cmd, not PowerShell):
```
(In emsdk directory)
./emsdk_env.bat

(In root directory)
make clean
make PLATFORM=web
cd bin 

(In bin directory)
python -m http.server

(In browser)
http://localhost:8000/index.html
```


## Directory structure

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
    +---lib
    |       libraylib.a
    |       libraylibdll.a   <-- optional
    |       raylib.dll       <-- optional
    |
    \---web
            libraylib.a
            shell.html
            favicon.ico      <-- optional
```