#ifndef IPC_ROUTINES_H_
#define IPC_ROUTINES_H_

#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>

#define PLOT_MAX_LEN 8192
#define PLOT_CNT 4

struct plot_data {
	bool upd;
	uint16_t len;
	double x[PLOT_MAX_LEN];
	double y[PLOT_MAX_LEN];
};

struct comp_data {
	bool enable;
	double threshold;
	double gain;
};

bool ipc_attach(const char *fname, size_t size, char **shm_ptr);
void ipc_write(struct plot_data *plot, double *data_x, double *data_y, unsigned int len);
bool ipc_read(struct plot_data *plot, double *data_x, double *data_y, unsigned int *len);

#endif /* IPC_ROUTINES_H_ */
