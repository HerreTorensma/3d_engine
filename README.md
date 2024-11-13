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
    - Move the grid thing into a grid map and make something seperate for meshes that roam freely

    - Remake the ECS without the fuckass macros and dict
    - Make a generic texture struct that also saves the pixels and whatnot
    - Add support for normals (why didnt i do this in the first place...)
    - Add flat shading to some stuff

    - The big refactor
        - Make rotation work for render_image_ex
            - Actually no
        - Think if i want to have a dst rect for render_image_ex
        - In render_image make pivot TOP_LEFT by default, remove parameter
        
        - Remove the 'tile system' and just save a mesh and texture index directly in the grid
        - Implement memory arenas in a lot of places
        - Choose convention for shaders
        - Clean up renderer
            - Clean up sprite rendering functions
            - Also colored drawing for the fonts
            - Probably forget about the screen anchors since I have a tile system which is gonna make that very difficult
        - Make a theme struct and read the gui things from that
        - Make length-based strings for easier modification
        - Make the isometric rendering not affected by the other 3d things
        - Create a generic pool data structure
            - Use for ECS
        - Remove the dictionary shit from the ecs
            - Actually remove the dict completely
            - Also zero-initialize components when setting (or when creating a new entity)
            - And remove the weird passing a struct va args macro to ECS_SET
        - [x] Rethink what should be in core.h and global.h
        - Instead of returning structs in create functions, make init function that have a pointer to a struct as a paramter. This way you can decide per case if the struct should be statically or dynamically allocated.

        - Move util to core

        - Put all meshes in one vbo/vao
        - Automatically create texture atlas???

        - Do the rotation of the billboard sprites on the GPU via a shader

    - Add sound with SDL2_Mixer
    - Add collision
    - Make dialogue system
    - Make text wrap system for the text edit stuff
    - Also load BMPs by just loading it to SDL_Surface and putting that into my custom texture format
    

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
