#ifndef IPC_H_
#define IPC_H_

#include "fft.h"
#include "ipc_routines.h"
#include <stdbool.h>
#include <stdint.h>

extern double *ipc_eq;
extern struct ipc_plot_data *ipc_plots[PLOT_CNT];
extern struct ipc_comp_data *ipc_comp;

bool ipc_init();
void ipc_write_plot_real(int num, double *data_x, double *data_y, unsigned int len);
void ipc_write_plot_fft(int num, double *data_x, fftw_complex *data_y, unsigned int len);

#endif /* IPC_H_ */
