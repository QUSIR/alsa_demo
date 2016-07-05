/*
 *  aplay.c - plays and records
 *
 *      CREATIVE LABS CHANNEL-files
 *      Microsoft WAVE-files
 *      SPARC AUDIO .AU-files
 *      Raw Data
 *
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *  Based on vplay program by Michael Beck
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */



#include"Linuxvoicenew.h"


static long long get_cur_time_ms(){
        struct timespec time1;
        clock_gettime(CLOCK_MONOTONIC, &time1);
        return (time1.tv_sec*1000LL)+((time1.tv_nsec)/1000000LL);
}

int main(int argc, char *argv[])
{



	int i;
	int bd;

	//set_pcm_nonblock(handle);

	//录音  以毫秒为计算单位
	long long leng;
	char name[50]="data.pcm";
	u_char *data;
	snd_pcm_t *play;
	snd_pcm_t *handle;


	handle=open_capturenew("plughw:1,0",SND_PCM_STREAM_CAPTURE,8000,1,16,1);


	printf("time=%lld\n",get_cur_time_ms());
	data=capture(handle,500,&leng);
	printf("time=%lld\n",get_cur_time_ms());
	save_voice(data,leng,name);



/*
	for(i=0;i<3;i++){
		sprintf(name, "%d", i);
		strcat(name,".pcm");
		printf("name is %s\n",name);
		data=capture(handle,250,&leng);

		printf("time=%lld\n",get_cur_time_ms());
		save_voice(data,leng,name);
		printf("%s run ok\n",name);
	}
*/
	/*
			sprintf(name, "%s", "test.pcm");
			remove(name);
			//创建音频存储文件
			bd = safe_open(name);
			if (bd < 0) {
				printf("openfile error\n");
				return -1;
			}
			for(i=0;i<100;i++){
				data=capture(handle,50,&leng);
				printf("capture leng is %lld\n",leng);
				printf("time=%lld\n",get_cur_time_ms());
				if (write(bd, data, leng) != leng) {
					printf("save_voice file error\n");
					return -1;
				}
			}
*/

			//play=open_playback(8000,1,16,&bd);
/*			play=open_capturenew("default",SND_PCM_STREAM_PLAYBACK,8000,1,16,1);
			bd = safe_open(name);
			if (bd < 0) {
				printf("openfile error\n");
				return -1;
			}
			while(1){
				data=capture(handle,50,&leng);
				pcm_write(play,data,leng);
				if (write(bd, data, leng) != leng) {
					printf("save_voice file error\n");
					return -1;
				}
			}
*/
/*	printf("time=%lld\n",get_cur_time_ms());

	data=capture(handle,250,&leng);

	printf("time=%lld\n",get_cur_time_ms());
*/

/*
	snd_pcm_t *play=open_playback(8000, 1, 16,&bd);
	bd = fopen("voicetest.pcm","rb");
	if (bd ==NULL) {
		printf("openfile error\n");
		return -1;
	}
	for(i=0;i<1000;i++){
		if (fread(data, 1000,1,bd) != 1000) {
			printf("read_voice file error\n");
			return -1;
		}
		sound_write(play,data,1000);
	}
	snd_pcm_close(play);
	play = NULL;
*/

	snd_pcm_close(handle);
	handle = NULL;

	free(audiobuf);

	snd_config_update_free_global();

	return EXIT_SUCCESS;
}

int save_voice(u_char *data,long long leng,char *name){
		remove(name);
		//创建音频存储文件
		int bd = safe_open(name);
		if (bd < 0) {
			printf("openfile error\n");
			return -1;
		}

		printf("data leng is %lld\n",leng);
		if (write(bd, data, leng) != leng) {
			printf("save_voice file error\n");
			return -1;
		}
		return 1;
}

int set_pcm_nonblock(snd_pcm_t *pcm){
	int err = snd_pcm_nonblock(pcm, 1);
	if (err < 0) {
		printf("set_pcm_nonbloce error\n");

		return 0;
	}
	return 1;
}

snd_pcm_t *open_capturenew(char *pcm_name,snd_pcm_stream_t stream,int set_rate, int channel, int bit,int interleaved){
	snd_pcm_t *handle;

   //rhwparams 使用全局变量
	//c  设置通道数
	rhwparams.channels=channel;
    //f  设置采样精度
	rhwparams.format = SND_PCM_FORMAT_S16_LE;
	switch(bit/8){
		case 1:rhwparams.format=SND_PCM_FORMAT_U8;
				break ;
		case 3:rhwparams.format=SND_PCM_FORMAT_S24_LE;
				break ;
	}

	//r   设置采样率
	rhwparams.rate = set_rate;

	snd_pcm_info_t *info;
	snd_pcm_info_alloca(&info);


	 //打开音频驱动  snd_pcm_open最后一个参数为打开模式
	int err = snd_pcm_open(&handle, pcm_name, stream, 0);
	if (err < 0) {
		printf("audio open error\n");
		return NULL;
	}
	//提取一个值从一个提示。
	if ((err = snd_pcm_info(handle, info)) < 0) {
		printf("info error\n");
		return NULL;
	}

	chunk_size = 1024;
		//audiobuf  音频数据缓冲区
	audiobuf = (u_char *)malloc(1024);

	if (audiobuf == NULL) {
		printf("audiobuf not enough memory\n");
		return NULL;
	}

	writei_func = snd_pcm_writei;
	readi_func = snd_pcm_readi;
	writen_func = snd_pcm_writen;
	readn_func = snd_pcm_readn;

	printf("set_params\n");
	snd_pcm_hw_params_t *params;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_uframes_t buffer_size;
	//size_t n;

	//snd_pcm_uframes_t start_threshold, stop_threshold;
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_sw_params_alloca(&swparams);
	err = snd_pcm_hw_params_any(handle, params);
	if (err < 0) {
		printf("Broken configuration for this PCM: no configurations available\n");
	}

	//interleaved  设置交叉模式
	/*
	SND_PCM_ACCESS_MMAP_INTERLEAVED
	mmap access with simple interleaved channels

	SND_PCM_ACCESS_MMAP_NONINTERLEAVED
	mmap access with simple non interleaved channels

	SND_PCM_ACCESS_MMAP_COMPLEX
	mmap access with complex placement

	SND_PCM_ACCESS_RW_INTERLEAVED
	snd_pcm_readi/snd_pcm_writei access

	SND_PCM_ACCESS_RW_NONINTERLEAVED
	snd_pcm_readn/snd_pcm_writen access
	*/
	if (interleaved)
	{
		  printf("interleaved\n");
		  err = snd_pcm_hw_params_set_access(handle, params,
					   SND_PCM_ACCESS_RW_INTERLEAVED);
	 }
	else{
		  err = snd_pcm_hw_params_set_access(handle, params,
						   SND_PCM_ACCESS_RW_NONINTERLEAVED);
	}

	if (err < 0) {
		printf("Access type not available\n");
	}
	printf("rhwparams.format is %d\n",rhwparams.format);
	// 设置采样精度
	err = snd_pcm_hw_params_set_format(handle, params, rhwparams.format);
	if (err < 0) {
		printf("Sample format non available\n");
	}
	//设置通道
	printf("rhwparams.channels is%d\n",rhwparams.channels);
	err = snd_pcm_hw_params_set_channels(handle, params, rhwparams.channels);
	if (err < 0) {
		printf("Channels count non available\n");
	}

	unsigned int rate = rhwparams.rate;
	printf("rhwparams.rate is %d\n",rate);
	//设置采样率
	err = snd_pcm_hw_params_set_rate_near(handle, params, &rhwparams.rate, 0);
    if(err<0)	printf("snd_pcm_hw_params_set_rate_near error\n");

	if (buffer_time == 0 && buffer_frames == 0) {
		printf("buffer_time\n");
			//提取最大缓冲区时间
		err = snd_pcm_hw_params_get_buffer_time_max(params,
								&buffer_time, 0);
		printf("buffer_time is%d\n",buffer_time);
		if(err<0) printf("snd_pcm_hw_params_get_buffer_time_max error\n");
		if (buffer_time > 500000)
		{
			buffer_time = 500000;
		}

	}

	printf("buffer_time is%d\n",buffer_time);
	if (period_time == 0 && period_frames == 0) {
		printf("period_time\n");
		if (buffer_time > 0)
			period_time = buffer_time / 4;
		else
			period_frames = buffer_frames / 4;
	}
	printf("period_time is %d\n",period_time);
	if (period_time > 0)
				//限制一个配置空间有段时间最近的目标。
		err = snd_pcm_hw_params_set_period_time_near(handle, params,
								 &period_time, 0);
	else
		err = snd_pcm_hw_params_set_period_size_near(handle, params,
								 &period_frames, 0);

	if(err<0) printf("snd_pcm_hw_params_set_period_size_near error\n");
	if (buffer_time > 0) {
		printf("buffer_time > 0\n");
				//限制一个配置空间有缓冲时间最近的一个目标。
		err = snd_pcm_hw_params_set_buffer_time_near(handle, params,
								 &buffer_time, 0);
	} else {
		err = snd_pcm_hw_params_set_buffer_size_near(handle, params,
								 &buffer_frames);
	}

	if(err<0) printf("snd_pcm_hw_params_set_buffer_size_near error\n");
		//检查时间戳是否单调对于给定配置。
//	monotonic = snd_pcm_hw_params_is_monotonic(params);
		//检查硬件是否支持暂停
//	can_pause = snd_pcm_hw_params_can_pause(params);
		//安装一个PCM硬件配置从一个配置选择空间和snd_pcm_prepare它。
	err = snd_pcm_hw_params(handle, params);
	if (err < 0) {
		printf("Unable to install hw params\n");
	}
	chunk_size=0;
	buffer_size=0;
	printf("chunk_size is %ld\n",chunk_size);
	//提取时间从一个配置空间大小
	snd_pcm_hw_params_get_period_size(params, &chunk_size, 0);
	printf("chunk_size is %ld\n",chunk_size);
		//提取配置空间缓冲区大小
	snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
		printf("buffer_size is %ld\n",buffer_size);
	if (chunk_size == buffer_size) {
		printf("Can't use period equal to buffer size (%lu == %lu)", chunk_size, buffer_size);
	}
		//返回当前PCM的软件配置。
	snd_pcm_sw_params_current(handle, swparams);

	//软件配置容器内设置效果最小。
	err = snd_pcm_sw_params_set_avail_min(handle, swparams, chunk_size);

	/* round up to closest transfer boundary */

	//n = buffer_size;
	/*
	int start_delay=1;
	if (start_delay <= 0) {
		start_threshold = n + (double) rate * start_delay / 1000000;
	} else
		start_threshold = (double) rate * start_delay / 1000000;
	if (start_threshold < 1)
		start_threshold = 1;
	if (start_threshold > n)
		start_threshold = n;

		//设置启动阈值在一个软件配置容器。
	err = snd_pcm_sw_params_set_start_threshold(handle, swparams, start_threshold);
	if(err<0) printf("snd_pcm_sw_params_set_start_threshold error\n");


	int stop_delay = 0;
	if (stop_delay <= 0)
		stop_threshold = buffer_size + (double) rate * stop_delay / 1000000;
	else
		stop_threshold = (double) rate * stop_delay / 1000000;


		//软件配置容器内设置停止阈值。
	err = snd_pcm_sw_params_set_stop_threshold(handle, swparams, stop_threshold);
	if(err<0){
		printf("snd_pcm_sw_params_set_stop_threshold error\n");
	}
	*/
		//安装PCM软件配置参数定义的。
	if (snd_pcm_sw_params(handle, swparams) < 0) {
		printf("unable to install sw params\n");
	}

	printf("setup_chmap\n");

	/*
	SetDataType data;
	data.channels=rhwparams.channels;
	data.format=rhwparams.format;
	data.rate=rhwparams.rate;
	 */

	//返回位需要存储一个PCM样本。
	bits_per_sample = snd_pcm_format_physical_width(rhwparams.format);
	printf("bits_per_sample is %d\n",bits_per_sample);
	bits_per_frame = bits_per_sample * rhwparams.channels;
	printf("bits_per_frame is %d\n",bits_per_frame);
	chunk_bytes = chunk_size * bits_per_frame / 8;
	printf("chunk_bytes is %d\n",chunk_bytes);
	printf("chunk_size is %ld\n",chunk_size);
	audiobuf = realloc(audiobuf, chunk_bytes);
	if (audiobuf == NULL) {
		printf("not enough memory\n");
	}

	buffer_frames = buffer_size;	/* for position test */
	printf("open_capture run ok\n");
	return handle;
}



size_t pcm_write(snd_pcm_t *handle,u_char *data, size_t count)
{
	ssize_t r;
	ssize_t result = 0;

	if (count < chunk_size) {
		snd_pcm_format_set_silence(rhwparams.format, data + count * bits_per_frame / 8, (chunk_size - count) * rhwparams.channels);
		count = chunk_size;
	}

	while (count > 0) {

		//check_stdin();
		r = writei_func(handle, data, count);

		if (r == -EAGAIN || (r >= 0 && (size_t)r < count)) {
				snd_pcm_wait(handle, 100);
		} else if (r == -EPIPE) {
			//xrun();
		} else if (r == -ESTRPIPE) {
			//suspend();
		} else if (r < 0) {
			//error(_("write error: %s"), snd_strerror(r));
			//prg_exit(EXIT_FAILURE);
		}
		if (r > 0) {
			result += r;
			count -= r;
			data += r * bits_per_frame / 8;
		}
	}
	return result;
}

ssize_t pcm_read(snd_pcm_t *handle,u_char *data, size_t rcount)
{
	ssize_t r;
	size_t result = 0;
	size_t count = rcount;
	while (count > 0) {

		r = readi_func(handle, data, count);

	    if (r == -EPIPE) {
	    	printf("pcm_read error EPIPE\n");
		}
		else if (r == -ESTRPIPE) {
			printf("pcm_read error ESTRPIPE\n");
		}
		else if (r < 0) {
			printf("pcm read error\n");
		}
		if (r > 0) {
			result += r;
			count -= r;
			data += r * bits_per_frame / 8;
		}
	}
	return rcount;
}


long long new_calc_count(long long data)
{
	long long temp;
	if (data == 0) {
		temp = LLONG_MAX;
	} else {
		temp = snd_pcm_format_size(rhwparams.format, rhwparams.rate * rhwparams.channels);
		printf("calc_count is %lld\n",temp);
		temp=temp/1000;
		temp *= data;
	}
        printf("calc_count is %lld\n",temp);
	return temp < LLONG_MAX ? temp : LLONG_MAX;
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

int safe_open(const char *name)
{
	int fd;

	fd = open(name, O_WRONLY | O_CREAT, 0644);
	if (fd == -1) {
		if (errno != ENOENT || !use_strftime)
			return -1;
		if (create_path(name) == 0)
			fd = open(name, O_WRONLY | O_CREAT, 0644);
	}
	return fd;
}

u_char *capture(snd_pcm_t *handle,long long time,long long *len)
{

	long long count,rest;

	*len=0;

	u_char *voicetemp;

        //获取捕获总的字节数
	count = new_calc_count(time);

    printf("capture count is %lld\n",count);
	if (count == 0)
		count = LLONG_MAX;

	if (count < LLONG_MAX)
		count += count % 2;
	else
		count -= count % 2;

	voicetemp=(u_char *)malloc(count * sizeof(u_char));

	rest = count;
	printf("capture count is %lld\n",count);

	long long fdcount = 0;
	printf("capture rest is %lld\n",rest);
	printf("capture count is %lld\n",count);
	while (rest > 0 ) {
		printf("1 count %lld rest %lld fdcount %lld\n",count,rest,fdcount);
		printf("chunk_bytes is %d\n",chunk_bytes);
		size_t c;
		if(rest <= (long long)chunk_bytes){
			 c= (size_t)rest;
			 printf("rest <= (long long)chunk_bytes\n");
		}
		else{
			printf("rest > (long long)chunk_bytes\n");
			c=chunk_bytes;
		}
		printf("bits_per_frame is %d\n",bits_per_frame);
		size_t f = c * 8 / bits_per_frame;

			printf("size_t c is %d\n",c);
			printf("size_t f is %d\n",f);
		if (pcm_read(handle,audiobuf, f) != f)
			break;

		memcpy(voicetemp+fdcount,audiobuf,c);
		*len=*len+c;

		count -= c;
		rest -= c;
		fdcount += c;
		printf("2 count %lld rest %lld fdcount %lld\n",count,rest,fdcount);
	}

	printf("data len is %lld\n",*len);
	return voicetemp;
}







