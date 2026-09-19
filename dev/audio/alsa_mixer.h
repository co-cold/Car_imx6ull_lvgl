#ifndef ALSA_MIXER_H
#define ALSA_MIXER_H

typedef struct alsa_mixer_t alsa_mixer_t;

alsa_mixer_t *alsa_mixer_init(void);
void alsa_mixer_close(alsa_mixer_t *am);
int  alsa_mixer_set_volume(alsa_mixer_t *am, float volume);
float alsa_mixer_get_volume(alsa_mixer_t *am);

#endif