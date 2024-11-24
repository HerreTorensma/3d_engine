#pragma once

#include "global.h"
#include "core.h"

void audio_play_sound(res_pack_t *res_pack, index_t index);

void audio_play_sound_3d(res_pack_t *res_pack, index_t index, vec3 ears_pos, vec3 pos);