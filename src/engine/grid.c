#include "grid.h"

void grid_init(grid_t *grid, u32 width, u32 height, u32 depth) {
    grid->width = width;
    grid->height = height;
    grid->depth = depth;

    grid->map = calloc(width * height * depth, sizeof(tile_t));
    if (grid->map == NULL) {
        printf("Couln't allocate map.\n");
    }
}

void grid_save(grid_t *grid) {

}

void grid_load(grid_t *grid) {

}