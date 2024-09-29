# 3D Engine

- A simple grid-based 3D engine written in C99 with OpenGL 3.3.

- Todo
    - 2D sprite rendering
    - Text rendering
    - Level editing
    - More general renderer class
    - Transparency
    - Lighting???
    - Dithering shader for limiting color palette
    - Billboard sprites

## Dependencies
- SDL2
- SDL_mixer
- glad
- cglm

## Building
### Linux
#### Installing dependencies
##### Arch
- `sudo pacman -S sdl2 sdl2_mixer`

##### Debian
- `sudo apt install libsdl2-dev libsdl2-mixer-dev`

#### Building
- cd into the directory
- `make`

### MacOS
- Install [homebrew](https://brew.sh/)
- Install sdl2 with `brew install sdl2 sdl2_mixer`
- `make`

### Windows
#### Installing dependencies
- Download the file `w64devkit-1.23.0.zip` from the "Releases" section of this [repository](https://github.com/skeeto/w64devkit)

#### Building
- Extract the downloaded folder and open `w64devkit.exe`
- cd into the directory
- `make`
