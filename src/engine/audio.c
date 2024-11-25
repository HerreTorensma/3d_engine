#include "audio.h"

void audio_play_sound(res_pack_t *res_pack, index_t index) {
    i32 channel = Mix_GroupAvailable(-1);
    Mix_Volume(channel, MIX_MAX_VOLUME);
    Mix_PlayChannel(channel, res_pack->sounds[index].chunk, 0);
}

void audio_play_sound_3d(res_pack_t *res_pack, index_t index, vec3 ears_pos, vec3 ears_dir, vec3 pos) {
    i32 channel = Mix_GroupAvailable(-1);

    vec3 sound_dir = {0};
    glm_vec3_sub(pos, ears_pos, sound_dir);
    glm_vec3_normalize(sound_dir);
    
    float angle = atan2f(sound_dir[2], sound_dir[0]) - atan2f(ears_dir[2], ears_dir[0]);

    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;

    float panning = (sinf(angle) + 1.0f) / 2.0f;
    u8 left = (u8)(255 * (1.0f - panning));
    u8 right = (u8)(255 * panning);

    Mix_SetPanning(channel, left, right);

    float distance = glm_vec3_distance(ears_pos, pos);

    if (distance == 0.0f) {
        Mix_PlayChannel(channel, res_pack->sounds[index].chunk, 0);
        return;
    }

    // i32 volume = MIX_MAX_VOLUME / (distance * 5.0f);
    i32 volume = MIX_MAX_VOLUME / (distance * 1.0f);
    Mix_Volume(channel, volume);

    Mix_PlayChannel(channel, res_pack->sounds[index].chunk, 0);
}
