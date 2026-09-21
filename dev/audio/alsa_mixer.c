#include "alsa_mixer.h"
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct alsa_mixer_t {
    snd_mixer_t *mixer;
    snd_mixer_elem_t *elem;
    long vol_min;
    long vol_max;
    float volume;
};

alsa_mixer_t *alsa_mixer_init(void)
{
    alsa_mixer_t *am = calloc(1, sizeof(alsa_mixer_t));
    if (!am) return NULL;

    if (snd_mixer_open(&am->mixer, 0) < 0) goto fail;
    if (snd_mixer_attach(am->mixer, "default") < 0) goto fail;
    snd_mixer_selem_register(am->mixer, NULL, NULL);
    if (snd_mixer_load(am->mixer) < 0) goto fail;

    const char *ctrl_names[] = {"Master", "PCM", "Playback", "Headphone", "Speaker", NULL};
    for (int i = 0; ctrl_names[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, ctrl_names[i]);
        am->elem = snd_mixer_find_selem(am->mixer, sid);
        if (am->elem) {
            snd_mixer_selem_get_playback_volume_range(am->elem, &am->vol_min, &am->vol_max);
            printf("[alsa_mixer] found ctrl '%s' range=[%ld,%ld]\n",
                   ctrl_names[i], am->vol_min, am->vol_max);
            break;
        }
    }

    if (!am->elem) goto fail;

    /* DAPM path init — only once, toggling these during playback kills audio */
    const char *dapm_ctrls[] = {
        "Right Output Mixer PCM", "Left Output Mixer PCM",
        "Right Output Mixer DAC",  "Left Output Mixer DAC",
        NULL
    };
    for (int i = 0; dapm_ctrls[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, dapm_ctrls[i]);
        snd_mixer_elem_t *e = snd_mixer_find_selem(am->mixer, sid);
        if (e) snd_mixer_selem_set_playback_switch_all(e, 1);
    }

    /* enable playback switches and set volume to max for output stage controls */
    const char *other_ctrls[] = {"Playback", "Headphone", "Speaker", NULL};
    for (int i = 0; other_ctrls[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, other_ctrls[i]);
        snd_mixer_elem_t *e = snd_mixer_find_selem(am->mixer, sid);
        if (e) {
            snd_mixer_selem_set_playback_switch_all(e, 1);
            long min, max;
            snd_mixer_selem_get_playback_volume_range(e, &min, &max);
            snd_mixer_selem_set_playback_volume_all(e, max);
        }
    }

    am->volume = 0.7f;
    alsa_mixer_set_volume(am, am->volume);
    printf("[alsa_mixer] init OK, default volume=%.2f\n", am->volume);
    return am;

fail:
    printf("[alsa_mixer] init failed\n");
    if (am->mixer) { snd_mixer_close(am->mixer); am->mixer = NULL; }
    free(am);
    return NULL;
}

void alsa_mixer_close(alsa_mixer_t *am)
{
    if (!am) return;
    if (am->mixer) snd_mixer_close(am->mixer);
    free(am);
}

int alsa_mixer_set_volume(alsa_mixer_t *am, float volume)
{
    if (!am || !am->mixer) return -1;
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    am->volume = volume;

    /* DAPM路径兜底 — 每次调音量都重新打开，防止硬件断电 */
    const char *dapm_ctrls[] = {"Right Output Mixer PCM", "Left Output Mixer PCM", NULL};
    for (int i = 0; dapm_ctrls[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, dapm_ctrls[i]);
        snd_mixer_elem_t *e = snd_mixer_find_selem(am->mixer, sid);
        if (e) snd_mixer_selem_set_playback_switch_all(e, 1);
    }

    /* Playback数字音量固定85%，永不静音DAC */
    {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, "Playback");
        snd_mixer_elem_t *e = snd_mixer_find_selem(am->mixer, sid);
        if (e) {
            long min, max;
            snd_mixer_selem_get_playback_volume_range(e, &min, &max);
            long hw = min + (max - min) * 85 / 100;
            snd_mixer_selem_set_playback_volume_all(e, hw);
        }
    }

    /* cubic curve -> Headphone / Speaker 硬件放大器 */
    float curve = powf(volume, 1.0f / 3.0f);
    int vol_pct = (int)(curve * 100.0f);
    if (vol_pct >= 0 && vol_pct < 20) vol_pct = 20;
    if (vol_pct > 100) vol_pct = 100;

    const char *hw_ctrls[] = {"Headphone", "Speaker", NULL};
    for (int i = 0; hw_ctrls[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, hw_ctrls[i]);
        snd_mixer_elem_t *e = snd_mixer_find_selem(am->mixer, sid);
        if (e) {
            long min, max;
            snd_mixer_selem_get_playback_volume_range(e, &min, &max);
            long hw = min + (max - min) * vol_pct / 100;
            snd_mixer_selem_set_playback_volume_all(e, hw);
        }
    }

    snd_mixer_handle_events(am->mixer);
    return 0;
}

float alsa_mixer_get_volume(alsa_mixer_t *am)
{
    return am ? am->volume : 0.0f;
}