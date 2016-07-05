#ifndef LINUX_VOICE_H_
#define LINUX_VOICE_H_

#include <sys/soundcard.h>
#include <alsa/asoundlib.h>

snd_pcm_t *open_capture(int rate,int channel,int bit,int* frame);
snd_pcm_t *open_playback(int rate,int channel,int bit,int* frame);
void close_sound_dev(snd_pcm_t *handle);
int sound_dev_prepare(snd_pcm_t *handle);
int sound_read(snd_pcm_t *handle,unsigned char *data,int len);
int sound_write(snd_pcm_t *handle,unsigned char *data,int len);
int change_volume(int volume);

#endif
