#include "audio.h"
#include <sched.h>

/* set desired audio parameters here */
#define BUF_FRAMES	512
#define SAMP_RATE	48000
//#define USE_PULSE

/* device name */
#ifdef USE_PULSE

const char *audio_c_device = "pulse";
const char *audio_p_device = "pulse";

#else

const char *audio_c_device = "hw:0,0";
const char *audio_p_device = "hw:0,0";

#endif

/* sampling rate */
unsigned int audio_samp_rate = SAMP_RATE;

/* buffer size in frames */
unsigned int audio_buf_size_frames = BUF_FRAMES;

/* period between interrupts in frames */
unsigned int audio_period_size_frames = BUF_FRAMES / 2;

snd_pcm_t *c_handle, *p_handle;

#define CHECK_ERR(func) { if ((err = func) < 0) { printf("Initialization error: %s\n", snd_strerror(err)); return false; } }

bool audio_init()
{
    int err;

    snd_pcm_hw_params_t *c_hw_params, *p_hw_params;
    snd_pcm_sw_params_t *c_sw_params, *p_sw_params;

    /* set scheduler */
    struct sched_param sched_param;
    if (sched_getparam(0, &sched_param) < 0) {
        printf("Scheduler getparam failed...\n");
    } else {
        sched_param.sched_priority = sched_get_priority_max(SCHED_RR);

        if (!sched_setscheduler(0, SCHED_RR, &sched_param))
            printf("Scheduler set to Round Robin with priority %i...\n", sched_param.sched_priority);
        else
            printf("Scheduler set to Round Robin with priority %i FAILED\n", sched_param.sched_priority);
    }

    snd_pcm_hw_params_alloca(&p_hw_params);
    snd_pcm_hw_params_alloca(&c_hw_params);
    snd_pcm_sw_params_alloca(&p_sw_params);
    snd_pcm_sw_params_alloca(&c_sw_params);

    /* open devices */
    CHECK_ERR( snd_pcm_open(&c_handle, audio_c_device, SND_PCM_STREAM_CAPTURE, 0) )
    CHECK_ERR( snd_pcm_open(&p_handle, audio_p_device, SND_PCM_STREAM_PLAYBACK, 0) )

    /* read current parameters */
    CHECK_ERR( snd_pcm_hw_params_any(c_handle, c_hw_params) )
    CHECK_ERR( snd_pcm_hw_params_any(p_handle, p_hw_params) )

    /* set access mode */
    CHECK_ERR( snd_pcm_hw_params_set_access(c_handle, c_hw_params, AUDIO_ACCMODE) )
    CHECK_ERR( snd_pcm_hw_params_set_access(p_handle, p_hw_params, AUDIO_ACCMODE) )

    /* set data format */
    CHECK_ERR( snd_pcm_hw_params_set_format(c_handle, c_hw_params, AUDIO_FORMAT) )
    CHECK_ERR( snd_pcm_hw_params_set_format(p_handle, p_hw_params, AUDIO_FORMAT) )

    /* set number of channels */
    CHECK_ERR( snd_pcm_hw_params_set_channels(c_handle, c_hw_params, AUDIO_CHANNELS) )
    CHECK_ERR( snd_pcm_hw_params_set_channels(p_handle, p_hw_params, AUDIO_CHANNELS) )

    /* set sampling rate */
	unsigned int c_rate = audio_samp_rate;
    unsigned int p_rate = audio_samp_rate;
    CHECK_ERR( snd_pcm_hw_params_set_rate_near(c_handle, c_hw_params, &c_rate, 0) )
    CHECK_ERR( snd_pcm_hw_params_set_rate_near(p_handle, p_hw_params, &p_rate, 0) )

    if (c_rate != audio_samp_rate)
        printf("Warning: actual capture sampling rate will be %u\n", c_rate);

    if (p_rate != audio_samp_rate)
    	printf("Warning: actual playback sampling rate will be %u\n", p_rate);

    if (c_rate != p_rate) {
    	printf("Error: capture and playback sampling rate does not match\n");
    	return false;
    }

    audio_samp_rate = c_rate;

    /* set buffer size */
    snd_pcm_uframes_t c_buf_size = audio_buf_size_frames;
    snd_pcm_uframes_t p_buf_size = audio_buf_size_frames;

    CHECK_ERR( snd_pcm_hw_params_set_buffer_size_near(c_handle, c_hw_params, &c_buf_size) )
    CHECK_ERR( snd_pcm_hw_params_set_buffer_size_near(p_handle, p_hw_params, &p_buf_size) )

    if (c_buf_size != audio_buf_size_frames)
        printf("Warning: actual capture buffer size will be %u frames\n", (unsigned int)c_buf_size);

    if (p_buf_size != audio_buf_size_frames)
    	printf("Warning: actual playback buffer size will be %u frames\n", (unsigned int)p_buf_size);

    if (c_buf_size != p_buf_size) {
    	printf("Error: capture and playback buffer size does not match\n");
    	return false;
    }

    audio_buf_size_frames = c_buf_size;

    /* set interrupt period */
    snd_pcm_uframes_t c_period = audio_period_size_frames;
    snd_pcm_uframes_t p_period = audio_period_size_frames;

    CHECK_ERR( snd_pcm_hw_params_set_period_size_near(c_handle, c_hw_params, &c_period, 0) )
    CHECK_ERR( snd_pcm_hw_params_set_period_size_near(p_handle, p_hw_params, &p_period, 0) )

	if (c_period != audio_period_size_frames)
		printf("Warning: actual capture period size will be %u frames\n", (unsigned int)c_period);

    if (p_period != audio_period_size_frames)
        printf("Warning: actual playback period size will be %u frames\n", (unsigned int)p_period);

    if (c_period != p_period) {
    	printf("Error: capture and playback period does not match\n");
    	return false;
    }

    audio_period_size_frames = c_period;

    /* write parameters */
    CHECK_ERR( snd_pcm_hw_params(c_handle, c_hw_params) )
    CHECK_ERR( snd_pcm_hw_params(p_handle, p_hw_params) )

#ifndef USE_PULSE
    /* link devices */
    CHECK_ERR( snd_pcm_link(c_handle, p_handle) )
#endif

    return true;
}

void audio_reinit(char *buffer)
{
    static int err_cntr = 0;

    err_cntr++;
    printf("errors counter %i\n", err_cntr);

    snd_pcm_prepare(c_handle);
    snd_pcm_prepare(p_handle);
    snd_pcm_writei(p_handle, buffer, audio_buf_size_frames);
}

bool audio_read(char *buffer, snd_pcm_sframes_t *frames_num)
{
    if ((*frames_num = snd_pcm_readi(c_handle, buffer, audio_period_size_frames)) < 0) {
        printf("Read error: %s\n", snd_strerror(*frames_num));
        return false;
    }

    return true;
}

bool audio_write(char *buffer, snd_pcm_sframes_t frames_num)
{
    if ((frames_num = snd_pcm_writei(p_handle, buffer, frames_num)) < 0) {
        printf("Write error: %s\n", snd_strerror(frames_num));
        return false;
    }

    return true;
}

void audio_free()
{
    snd_pcm_drop(c_handle);
    snd_pcm_drain(p_handle);

    snd_pcm_unlink(c_handle);
    snd_pcm_hw_free(p_handle);
    snd_pcm_hw_free(c_handle);

    snd_pcm_close(p_handle);
    snd_pcm_close(c_handle);
}
