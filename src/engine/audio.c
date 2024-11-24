#include "audio.h"

void audio_play_sound(res_pack_t *res_pack, index_t index) {
    Mix_PlayChannel(-1, res_pack->sounds[index].chunk, 0);
}

void audio_play_sound_3d(res_pack_t *res_pack, index_t index, vec3 ears_pos, vec3 pos) {

}