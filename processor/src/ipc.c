#include "ipc.h"
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* keyfile names for shmem */
const char *keyfile_plots = "/tmp/gproc_plots";
const char *keyfile_eq = "/tmp/gproc_eq";
const char *keyfile_norm = "/tmp/gproc_proc";
const char *keyfile_info = "/tmp/gproc_info";

/* pointers to data segments in shared memory */
double *ipc_eq;
char *ipc_plots_buffer;
struct plot_data *ipc_plots[PLOT_CNT];
struct comp_data *ipc_comp;
struct ipc_audio_info *ipc_info;

bool ipc_init()
{
	int i, j;

    if (!ipc_attach(keyfile_plots, sizeof(struct plot_data) * PLOT_CNT, &ipc_plots_buffer))
        return false;

    if (!ipc_attach(keyfile_eq, sizeof(double) * PLOT_MAX_LEN / 2, (char **)&ipc_eq))
        return false;

    if (!ipc_attach(keyfile_norm, sizeof(struct comp_data), (char **)&ipc_comp))
    	return false;

    if (!ipc_attach(keyfile_info, sizeof(struct ipc_audio_info), (char **)&ipc_info))
    	return false;

    /* set plots pointers */
    for (i = 0; i < PLOT_CNT; i++)
    	ipc_plots[i] = (struct plot_data*)(ipc_plots_buffer + (sizeof(struct plot_data) * i));

    /* plots data initialization */
    for (i = 0; i < PLOT_CNT; i++) {
    	ipc_plots[i]->len = 0;
    	ipc_plots[i]->upd = false;

    	for (j = 0; j < PLOT_MAX_LEN; j++) {
    		ipc_plots[i]->x[j] = 0.0;
    		ipc_plots[i]->y[j] = 0.0;
    	}
    }

    /* equalizer data initialization */
    for (i = 0; i < PLOT_MAX_LEN / 2; i++)
    	ipc_eq[i] = 1.0;

    /* compression data initialization */
    ipc_comp->enable = false;
    ipc_comp->threshold = 3200.0;
    ipc_comp->gain = 1.2;

    /* ipc info initialization */
    ipc_info->period_size_frames = audio_period_size_frames;
    ipc_info->sampling_rate = audio_samp_rate;

    return true;
}

void ipc_free()
{

}

void ipc_write_plot_real(int num, double *data_x, double *data_y, unsigned int len)
{
	ipc_write(ipc_plots[num], data_x, data_y, len);
}

void ipc_write_plot_fft(int num, double *data_x, fftw_complex *data_y, unsigned int len)
{
	double magnitude[len];

	fft_to_magn(data_y, magnitude, len);
	ipc_write(ipc_plots[num], data_x, magnitude, len);
}
