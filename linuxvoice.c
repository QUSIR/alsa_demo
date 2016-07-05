#include "linuxvoice.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>

#define VOLUME_MAX		100
#define VOLUME_MIN		0

snd_pcm_t *open_sound_dev(char *pcm_name,snd_pcm_stream_t type,int rate,int channel,int bit,int* pFrame){
	int err;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *hw_params;

	if ((err = snd_pcm_open (&handle, pcm_name, type, 0)) < 0) {
		return NULL;
	}

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	snd_pcm_format_t format=SND_PCM_FORMAT_S16_LE;
	switch(bit/8){
		case 1:format=SND_PCM_FORMAT_U8;
				break ;
		case 3:format=SND_PCM_FORMAT_S24_LE;
				break ;
	}
	if ((err = snd_pcm_hw_params_set_format (handle, hw_params, format)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, channel)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, channel)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	snd_pcm_uframes_t buffer_size=rate/1000*channel*bit*8*2.1;
//	snd_pcm_uframes_t buffer_size= 4096*2.1;
	if(type==SND_PCM_STREAM_CAPTURE){
		if ((err = snd_pcm_hw_params_set_buffer_size_near (handle, hw_params, &buffer_size)) < 0) {
			fprintf (stderr, "cannot set buffer size (%s)\n",
				 snd_strerror (err));
			return NULL;
		}
	}

	snd_pcm_uframes_t period_size=rate/1000*channel*bit*4;
//	snd_pcm_uframes_t period_size = 256;
	if(type==SND_PCM_STREAM_CAPTURE){
		if ((err = snd_pcm_hw_params_set_period_size_near (handle, hw_params, &period_size, 0)) < 0) {
			fprintf (stderr, "cannot set period size (%s)\n",
				 snd_strerror (err));
			return NULL;
		}
	}

	if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_get_period_size(hw_params,pFrame,0)) < 0) {
		fprintf (stderr, "cannot get period size (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	printf("ken debug, buffer_size=%d period_size=%d frames=%d\n",buffer_size,period_size,*pFrame);

	snd_pcm_hw_params_free (hw_params);

	return handle;
}

snd_pcm_t *open_capture(int rate, int channel, int bit,int* frame){
	return open_sound_dev("plughw:1,0",SND_PCM_STREAM_CAPTURE,rate,channel,bit,frame);
}

snd_pcm_t *open_playback(int rate, int channel, int bit,int* frame){
	return open_sound_dev("default",SND_PCM_STREAM_PLAYBACK,rate,channel,bit,frame);//"plughw:1,0"?
}

void close_sound_dev(snd_pcm_t *handle){
	if(handle!=NULL) snd_pcm_close (handle);
}

int sound_dev_prepare(snd_pcm_t *handle){
	return snd_pcm_prepare(handle);
}

int sound_read(snd_pcm_t *handle,unsigned char *data,int len){
	return snd_pcm_readi (handle, data, len);
}

int sound_write(snd_pcm_t *handle,unsigned char *data,int len){
	return snd_pcm_writei (handle, data, len);
}

int change_volume(int volume){
	if (volume < VOLUME_MIN)
		volume = VOLUME_MIN;
	else if (volume > VOLUME_MAX)
		volume = VOLUME_MAX;
	const int i32Volume = (volume << 8) | volume;

	int i32DevMixer = open("/dev/mixer", O_RDWR);
	if (i32DevMixer < 0){
		printf("open(/dev/mixer) failed\n");
		return -1;
	}


	if (ioctl(i32DevMixer, MIXER_WRITE(SOUND_MIXER_PCM), &i32Volume) < 0){
		printf("ioctl(/dev/mixer, MIXER_WRITE(SOUND_MIXER_PCM)) failed\n");
		close(i32DevMixer);
		return -1;
	}

	close(i32DevMixer);
	return 0;
}
