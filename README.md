# 3D Engine

- A simple grid-based 3D engine written in C99 with OpenGL 3.3.

- Todo
    - [x] 2D sprite rendering
    - [x] Text rendering
    - [x] Level editing
    - More general renderer class
    - [x] (kinda) Transparency
    - Lighting???
    - Dithering shader for limiting color palette
    - [x] Billboard sprites
    - [x] Move the grid thing into a grid map and make something seperate for meshes that roam freely

    - Remake the ECS without the fuckass macros and dict
    - [x] Make a generic texture struct that also saves the pixels and whatnot
    - [x] Add support for normals (why didnt i do this in the first place...)
    - [x] Add flat shading to some stuff

    - The big refactor
        - [x] Never mind
            - [ ] Make rotation work for render_image_ex
                - Actually no
            - [ ] Think if i want to have a dst rect for render_image_ex
            - [ ] In render_image make pivot TOP_LEFT by default, remove parameter
        
        - [x] Remove the 'tile system' and just save a mesh and texture index directly in the grid
            - [ ] Probably gonna have to change this back actually

        - [ ] Implement memory arenas in a lot of places

        - [ ] Choose convention for shaders
        
        - [x] Never mind I guess
            - Clean up renderer
                - Clean up sprite rendering functions
                - Also colored drawing for the fonts
                - Probably forget about the screen anchors since I have a tile system which is gonna make that very difficult
        
        - [ ] Make a theme struct and read the gui things from that
        - [x] Tried it didn't like it
            - Make length-based strings for easier modification
        - [x] Kinda solved it in a hacky way
            - Make the isometric rendering not affected by the other 3d things
        
        - [] Create a generic pool data structure
            - Use for entities
        - [x] Remove the dictionary shit from the ecs
            - Actually remove the dict completely
            - Also zero-initialize components when setting (or when creating a new entity)
            - And remove the weird passing a struct va args macro to ECS_SET
        - [x] Rethink what should be in core.h and global.h
        - Instead of returning structs in create functions, make init function that have a pointer to a struct as a paramter. This way you can decide per case if the struct should be statically or dynamically allocated.

        - [x] Nah
            - Move util to core

        - [ ] Do the rotation of the billboard sprites on the GPU via a shader

        - [ ] Throw away the ECS in favor of just an entity struct that contains all components??? way simpler

        - [ ] For the GUI generate ids to know whats focused


    - [x] Add sound with SDL2_Mixer
    - [x] Add 3D audio
    - [x] Add collision
    - [ ] Make dialogue system
    - [ ] Make text wrap system for the text edit stuff
    - [ ] Also load BMPs by just loading it to SDL_Surface and putting that into my custom texture format
    - Might not do this and instead only support 60fps like true Eurojank
        - [ ] Implement delta time

    - GUI
        - [ ] Text edit (for dialogue editor)
        - [ ] Slider with snap (for editing specific levels in the grid)
        - [ ] Create debugging tool that prints values to the top left of the screen 'automatically' with formatted string support

    - Should still do this in more places  I think
        - [x] Temp allocators for strings n stuff

    - [x] Tried this didn't like it, opted for simpler solution
        - Multiple meshes per tile
            - Auto generate bounding box (easy af)
            - Save tile and texture inside grid

    - [x] Nah just make functions that reference a private state, this is too sketchy
        - Global state struct accesible from everywhere

    - [ ] Add redo / undo to the editor
        - Keep stack of actions
        - Every undoable action is an action object

    - [ ] Don't pass res_pack everywhere, just make it a static global variable inside res.c and use helper functions

    - [ ] Implement generic stack and queue data structures

    - [ ] Create "toast messages" that pop up on the screen when something happens in the game
        - They automatically fade away after a time but are also pushed to a permanent log
        - Happens after things like picking up items, killing someone etc.

    - [ ] Create "event" system where events are basically linux command line programs
        - They can be triggered from cheat console or in dialogue txt file
        - Things like give_player cigarette 10
        - Or give_health 10
        - or start_quest the_start
        - I could even do a cool preprocessor thing where it replaces the enum values with int literals in text

    - [ ] Objective system

    - [ ] NPC's
        - Pathfinding
        - Dialogue
        - Combat
        - Factions ???
            - Definitely necessary since it might become a drug dealer game

    - Current plan
        - [x] Nah
            - Write pool allocator
        
        - Rewrite entity system
            - Also clean up renderer such that there are no ECS references there
            - just functions to render meshes and sprites and the 'game' handles the rest

        - Write inventory system

        - Refactor shaders to have snake_case
        - Move shader c code to renderer

        - Rewrite res pack system so I don't have to pass res_pack everywhere

        - Put all meshes on same vao/vbo
        - Put all block textures (96x96) on atlas

        - Think about how to organize the game itself and the state (main menu, level select whatever) and edit mode
            - Edit mode is part of game

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
