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

void grid_save(grid_t *grid, const char path[]) {
    FILE *file = fopen(path, "wb");

    fwrite(&grid->width, sizeof(u32), 1, file);
    fwrite(&grid->height, sizeof(u32), 1, file);
    fwrite(&grid->depth, sizeof(u32), 1, file);

    fwrite(grid->map, sizeof(tile_t), grid->width * grid->height * grid->depth, file);

    fclose(file);
}

void grid_load(grid_t *grid, const char path[]) {
    FILE *file = fopen(path, "rb");

    u32 width, height, depth;

    fread(&width, sizeof(u32), 1, file);
    fread(&height, sizeof(u32), 1, file);
    fread(&depth, sizeof(u32), 1, file);

    grid_init(grid, width, height, depth);

    fread(grid->map, sizeof(tile_t), width * height * depth, file);

    fclose(file);
}