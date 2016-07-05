#define _GNU_SOURCE
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <locale.h>
#include <alsa/asoundlib.h>
#include <assert.h>
#include <termios.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <endian.h>
#include <aconfig.h>
#include <gettext.h>
#include <version.h>

#ifndef LLONG_MAX
#define LLONG_MAX    9223372036854775807LL
#endif


#include"linuxvoice.h"

/* global data */

static snd_pcm_sframes_t (*readi_func)(snd_pcm_t *handle, void *buffer, snd_pcm_uframes_t size);
static snd_pcm_sframes_t (*writei_func)(snd_pcm_t *handle, const void *buffer, snd_pcm_uframes_t size);
static snd_pcm_sframes_t (*readn_func)(snd_pcm_t *handle, void **bufs, snd_pcm_uframes_t size);
static snd_pcm_sframes_t (*writen_func)(snd_pcm_t *handle, void **bufs, snd_pcm_uframes_t size);


int save_voice(u_char *data,long long leng,char *name);

static struct {
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
}rhwparams;

typedef struct SetData{
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
}SetDataType;



static u_char *audiobuf = NULL;

static unsigned period_time = 0;
static unsigned buffer_time = 0;
static snd_pcm_uframes_t period_frames = 0;
static snd_pcm_uframes_t buffer_frames = 0;



static snd_pcm_uframes_t chunk_size;

static size_t bits_per_sample, bits_per_frame,chunk_bytes;

static int use_strftime = 0;



int set_pcm_nonblock(snd_pcm_t *pcm);
u_char *capture(snd_pcm_t *handle,long long count,long long *len);
snd_pcm_t *open_capturenew(char *pcm_name,snd_pcm_stream_t stream,int rate, int channel, int bit,int interleaved);
ssize_t pcm_read(snd_pcm_t *handle,u_char *data, size_t rcount);

size_t pcm_write(snd_pcm_t *handle,u_char *data, size_t count);
long long new_calc_count(long long data);

static long long get_cur_time_ms();

int safe_open(const char *name);

