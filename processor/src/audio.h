#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdbool.h>
#include <alsa/asoundlib.h>

/* capture device name */
extern const char *audio_c_device;

/* playback device name */
extern const char *audio_p_device;

/* capture sampling rate */
extern unsigned int audio_samp_rate;

/* playback sampling rate */
extern unsigned int audio_p_rate;

/* data format */
#define AUDIO_FORMAT            	SND_PCM_FORMAT_S16_LE

/* access mode */
#define AUDIO_ACCMODE           	SND_PCM_ACCESS_RW_INTERLEAVED

/* number of channels */
#define AUDIO_CHANNELS          	2

/* period between samples in seconds */
#define AUDIO_PERIOD_SEC ((1.0 / (double)audio_samp_rate))

/* frame size in bytes */
#define AUDIO_FRAME_SIZE_BYTES		(snd_pcm_format_width(AUDIO_FORMAT) / 8)

/* period between interrupts in frames */
extern unsigned int audio_period_size_frames;

/* software buffer size in bytes */
#define AUDIO_SOFT_BUF_SIZE_BYTES	(audio_period_size_frames * AUDIO_CHANNELS * AUDIO_FRAME_SIZE_BYTES)

bool audio_init();
void audio_reinit(char *buffer);
bool audio_read(char *buffer, snd_pcm_sframes_t *frames_num);
bool audio_write(char *buffer, snd_pcm_sframes_t frames_num);
void audio_free();

#endif /* AUDIO_H_ */
