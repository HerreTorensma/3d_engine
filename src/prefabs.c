/*
This file contains all the functions that spawn entities
*/
#include "entity.h"
#include "state.h"
#include "engine/util.h"

index_t spawn_player(transform_t transform) {
    index_t ent_index = ent_new();
    entity_t *ent = ent_get(ent_index);
    ent->flags = HAS_PLAYER;

    ent->transform.position[0] = 3.0f;
    ent->transform.position[1] = 1.5f;
    ent->transform.position[2] = 3.0f;

    // Camera position is relative to the transform position
    // Or maybe not idk
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, ent->camera.front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, ent->camera.up);
    ent->camera.yaw = -90.0f;

    ent->player_controller.walk_speed = 0.05f;
    ent->player_controller.crouch_speed = 0.005f;
    ent->player_controller.jump_height = 0.08f;

    ent->player_collider.box = (box_t){
        .min_x = -0.15f,
        .max_x = 0.15f,
        
        .min_y = -0.5f,
        .max_y = 0.0f,

        .min_z = -0.15f,
        .max_z = 0.15f,
    };

    ent->inventory.slots_amount = 40;
    ent->inventory.selected_slot = 0;
    ent->inventory.slots[0] = (inventory_slot_t){
        .item_index = ITEM_GLOCK,
        .amount = 1,
        .mut_stats = {0},
    };
    ent->inventory.slots[1] = (inventory_slot_t){
        .item_index = ITEM_CIGARETTE,
        .amount = 500,
        .mut_stats = {0},
    };

    return ent_index;
}

void spawn_barrel(transform_t transform) {
    entity_t *ent = ent_get(ent_new());
    ent->flags = HAS_SPRITE | HAS_COLLIDER;
    memcpy(&ent->transform, &transform, sizeof(transform_t));

    ent->sprite.texture_index = TEX_BARREL;
    ent->sprite.billboard = true;
    ent->sprite.x_scale = 1.0f;
    ent->sprite.y_scale = 1.0f;
}

void spawn_tree(transform_t transform) {
    entity_t *ent = ent_get(ent_new());
    ent->flags = HAS_SPRITE | HAS_COLLIDER;
    memcpy(&ent->transform, &transform, sizeof(transform_t));

    ent->sprite.texture_index = TEX_TREE;
    ent->sprite.billboard = true;
    ent->sprite.x_scale = 1.0f;
    ent->sprite.y_scale = 1.0f;
}

void spawn_gate(transform_t transform) {
    entity_t *ent = ent_get(ent_new());
    ent->flags = HAS_SPRITE | HAS_COLLIDER;
    memcpy(&ent->transform, &transform, sizeof(transform_t));

    ent->sprite.texture_index = TEX_IRON_BARS;
    ent->sprite.billboard = false;
    ent->sprite.x_scale = 1.0f;
    ent->sprite.y_scale = 1.0f;
}

void spawn_dropped_item(transform_t transform, index_t item_index) {
    index_t ent_index = ent_new();
    entity_t *ent = ent_get(ent_index);
    ent->flags = HAS_SPRITE | HAS_OVERLAP | HAS_ITEM;
    memcpy(&ent->transform, &transform, sizeof(transform_t));

    ent->sprite.billboard = true;
    ent->sprite.texture_index = state.res_pack.items[item_index].thumbnail_index;
    ent->sprite.x_scale = 0.17f;
    ent->sprite.y_scale = 0.17f;

    ent->overlap.box = generate_even_box(0.25f);
    ent->overlap.collision_layer = LAYER_ITEMS;
    ent->overlap.collision_mask = LAYER_PLAYER;

    ent->item_index = item_index;
};