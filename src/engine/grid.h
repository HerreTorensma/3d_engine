/*
This header and source file contain functionality for saving and loading the grid
*/
#pragma once

#include "core.h"

void grid_init(grid_t *grid, u32 width, u32 height, u32 depth);

void grid_save(grid_t *grid);

void grid_load(grid_t *grid);