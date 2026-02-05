# Interactive PDE Visualizer
Visualize and interact with PDEs. Made in C with raylib.  

Equations implemented so far:
- Swift-Hohenberg Equation
- Cahn-Hilliard Equation

## Controls
- Scroll mouse wheel to change brush size
- Left (or right) mouse click to 'paint' negative (or positive) values at the cursor
- Spacebar to pause/resume simulation
- Tab to switch equations

## Compilation
Set up raylib as described [here](raylib/README.md). Then build with `make` and run `./bin/main`.


### For web

Setup:
- Set up emscripten emsdk
- Compile raylib's libraylib.a from source using emcc and put in `raylib_web/lib`
- Refactor main loop for browser with `emscripten_set_main_loop[_arg]()`

Then:
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