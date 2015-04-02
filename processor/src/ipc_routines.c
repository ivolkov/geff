#include "ipc_routines.h"
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

bool ipc_attach(const char *fname, size_t size, char **shm_ptr)
{
	int fd;
	key_t key;
	int shm_id;

	// check file availability
	if (access(fname, F_OK) == -1) {
		if ((fd = open(fname, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
			printf("Could not open file %s: %s\n", fname, strerror(errno));
			return false;
		}
		close(fd);
	}

	// get file key
	if ((key = ftok(fname, 1)) == -1) {
		printf("Could not generate IPC key from file %s: %s\n", fname, strerror(errno));
		return false;
	}

	// get shared memory id
	if ((shm_id = shmget(key, size, IPC_CREAT | S_IRUSR | S_IWUSR)) == -1) {
		printf("Could not get shared memory segment identifier: %s\n", strerror(errno));
		return false;
	}

	// attach shared memory
	if ((*shm_ptr = (char *)shmat(shm_id, NULL, 0)) == (void *)-1) {
		printf("Could not attach shared memory segment: %s\n", strerror(errno));
		return false;
	}

	return true;
}

void ipc_write(struct plot_data *plot, double *data_x, double *data_y, unsigned int len)
{
	if (!plot->upd) {
		plot->len = len;
		memcpy(&plot->x, data_x, sizeof(double) * len);
		memcpy(&plot->y, data_y, sizeof(double) * len);
		plot->upd = true;
	}
}

bool ipc_read(struct plot_data *plot, double *data_x, double *data_y, unsigned int *len)
{
    if (plot->upd) {
        memcpy(data_x, plot->x, sizeof(double) * plot->len);
        memcpy(data_y, plot->y, sizeof(double) * plot->len);
        *len = plot->len;
        plot->upd = false;
        return true;
    }

    return false;
}
