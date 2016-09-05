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

snd_pcm_uframes_t    ca_frames=160;
unsigned 			 samples_per_frame=160;
unsigned             input_latency_ms=100;
unsigned             bits_per_sample=16;

unsigned             ca_buf_size;
static u_char *ca_buf = NULL;


snd_pcm_t * open_capture (char *name,unsigned int set_rate, int channel, int bit)
{

    snd_pcm_hw_params_t* params;
    snd_pcm_format_t format;
    int result;
    snd_pcm_uframes_t tmp_buf_size;
    snd_pcm_uframes_t tmp_period_size;

    snd_pcm_t *ca_pcm;

    /* Open PCM for capture */

    result = snd_pcm_open (&ca_pcm,name,SND_PCM_STREAM_CAPTURE,0);
    if (result < 0)
    	printf("error \n");


    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca (&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any (ca_pcm, params);

    /* Set interleaved mode */
    snd_pcm_hw_params_set_access (ca_pcm, params,
				  SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Set format */


    format = SND_PCM_FORMAT_S16_LE;
    	switch(bit/8){
    		case 1:format=SND_PCM_FORMAT_U8;
    				break ;
    		case 3:format=SND_PCM_FORMAT_S24_LE;
    				break ;
    	}

    snd_pcm_hw_params_set_format (ca_pcm, params, format);

    /* Set number of channels */

    snd_pcm_hw_params_set_channels (ca_pcm, params,channel);

    /* Set clock rate */

    snd_pcm_hw_params_set_rate_near (ca_pcm, params, &set_rate, NULL);


    /* Set the sound device buffer size and latency */

	tmp_buf_size = 4096;//(set_rate / 1000) * input_latency_ms;
	//设置缓冲区
	snd_pcm_hw_params_set_buffer_size_near (ca_pcm, params,
						&tmp_buf_size);

	input_latency_ms = tmp_buf_size / (set_rate / 1000);

    /* Set period size to samples_per_frame frames. */
    //ca_frames = (snd_pcm_uframes_t) samples_per_frame /channel;
	
    tmp_period_size = tmp_buf_size/8;//ca_frames;
	//设置周期
    snd_pcm_hw_params_set_period_size_near (ca_pcm, params,
					    &tmp_period_size, NULL);


    /* Set our buffer */
    ca_buf_size = tmp_period_size/2;//ca_frames * channel * (bits_per_sample/8);

    printf("ca_buf_size is %d\n",ca_buf_size);

    ca_buf = (u_char *)malloc(ca_buf_size);


    /* Activate the parameters */
    result = snd_pcm_hw_params (ca_pcm, params);
    if (result < 0) {
    	snd_pcm_close (ca_pcm);
    }

    return ca_pcm;

}
int create_path(const char *path)
{
	char *start;
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	if (path[0] == '/')
		start = strchr(path + 1, '/');
	else
		start = strchr(path, '/');

	while (start) {
		char *buffer = strdup(path);
		buffer[start-path] = 0x00;

		if (mkdir(buffer, mode) == -1 && errno != EEXIST) {
			fprintf(stderr, "Problem creating directory %s", buffer);
			perror(" ");
			free(buffer);
			return -1;
		}
		free(buffer);
		start = strchr(start + 1, '/');
	}
	return 0;
}
int open_voice(char *name){

	remove(name);
		//创建音频存储文件

	int bd =open(name, O_WRONLY | O_CREAT, 0644);
	if (bd == -1) {
		if (errno != ENOENT)
			return -1;
		if (create_path(name) == 0)
			bd = open(name, O_WRONLY | O_CREAT, 0644);
	}
// safe_open(name);
		if (bd < 0) {
			printf("openfile error\n");
			return -1;
		}


		return bd;
}
int save_voice(int bd,u_char *data,long long leng){

		//printf("data leng is %lld\n",leng);
		if (write(bd, data, leng) != leng) {
			printf("save_voice file error\n");
			return -1;
		}
		return 1;
}

int main(){
	snd_pcm_t *new=open_capture("plughw:1,0",8000,1,16);
	int fb=open_voice("pjsip_alsa_dev.pcm");
	for(;;){

		int result = snd_pcm_readi (new, ca_buf, ca_buf_size);
		//保存数据到声音文件
		save_voice(fb,ca_buf,ca_buf_size*2);

		if (result == -EPIPE) {
		    printf( "ca_thread_func: overrun!\n");
		    snd_pcm_prepare (new);
		    continue;
		} else if (result < 0) {
		    printf( "ca_thread_func: error reading data!\n");
		}
	}
	return 1;
}
