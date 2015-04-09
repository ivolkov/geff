#include "ipc.h"
#include <QString>
#include <QDebug>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

const char *plotsKeyFileName = "/tmp/gproc_plots";
const char *eqKeyFileName = "/tmp/gproc_eq";
const char *compKeyFileName = "/tmp/gproc_comp";
const char *infoKeyFileName = "/tmp/gproc_info";

IPC::IPC(QObject *parent) : QObject(parent)
{
    plotsBuf = NULL;
    eq = NULL;

    if (!shmAttach(plotsKeyFileName, sizeof(struct ShmPlot) * PLOT_CNT, &plotsBuf)) {
        qDebug() << "IPC: unable to attach plots key file";
        return;
    }

    if (!shmAttach(eqKeyFileName, sizeof(double) * PLOT_MAX_LEN / 2, (char**)&eq)) {
        qDebug() << "IPC: unable to attach equalizer key file";
        return;
    }

    if (!shmAttach(compKeyFileName, sizeof(struct ShmComp), (char**)&comp)) {
        qDebug() << "IPC: unable to attach normalization key file";
        return;
    }

    if (!shmAttach(infoKeyFileName, sizeof(struct ShmInfo), (char **)&info)) {
        qDebug() << "IPC: unable to attach data information key file";
        return;
    }

    for (int i = 0; i < PLOT_CNT; i++)
        plots[i] = (struct ShmPlot*)(plotsBuf + (sizeof(struct ShmPlot) * i));

    connect(&tmr, SIGNAL(timeout()), this, SLOT(onTimer()));
    tmr.start(10);
}

IPC::~IPC()
{
    if (plotsBuf != NULL)
        shmdt(plotsBuf);

    if (eq != NULL)
        shmdt(eq);
}

bool IPC::readPlot(int num, QVector<double> *dataX, QVector<double> *dataY)
{
    double x[PLOT_MAX_LEN];
    double y[PLOT_MAX_LEN];
    unsigned int len;

    if (shmRead(plots[num], x, y, &len)) {
        if (num > 1)
            magnToDb(y, len);

        dataX->clear();
        dataY->clear();

        for (unsigned int i = 0; i < len; i++) {
            dataX->append(x[i]);
            dataY->append(y[i]);
        }

        return true;
    }

    return false;
}

bool IPC::shmAttach(const char *fname, size_t size, char **shmPtr)
{
    int fd;
    key_t key;
    int shm_id;

    // check file availability
    if (access(fname, F_OK) == -1) {
        if ((fd = open(fname, O_RDWR, S_IRWXU)) == -1) {
            qDebug() << "IPC: Could not open file " << fname << ": " << strerror(errno);
            return false;
        }
        close(fd);
    }

    // get file key
    if ((key = ftok(fname, 1)) == -1) {
        qDebug() << "IPC: Could not generate IPC key from file " << fname << ": " << strerror(errno);
        return false;
    }

    // get shared memory id
    if ((shm_id = shmget(key, size, S_IRUSR | S_IWUSR)) == -1) {
        qDebug() << "IPC: Could not get shared memory segment identifier: " << strerror(errno);
        return false;
    }

    // attach shared memory
    if ((*shmPtr = (char *)shmat(shm_id, NULL, 0)) == (void *)-1) {
        qDebug() << "IPC: Could not attach shared memory segment% " << strerror(errno);
        return false;
    }

    return true;
}

bool IPC::shmRead(struct ShmPlot *plot, double *dataX, double *dataY, unsigned int *len)
{
    if (plot && plot->upd) {
        memcpy(dataX, plot->x, sizeof(double) * plot->len);
        memcpy(dataY, plot->y, sizeof(double) * plot->len);
        *len = plot->len;
        plot->upd = false;
        return true;
    }

    return false;
}

void IPC::magnToDb(double *dataY, unsigned int len)
{
        /* tranform to dB */
    for (unsigned int i = 0; i < len; i++)
        dataY[i] = 20.0 * log10(dataY[i]);

    /* apply dynamic range */
    const double drValue = 20.0 * log10(16384.0);
    for (unsigned int i = 0; i < len; i++)
        dataY[i] -= drValue;
}

void IPC::onTimer()
{
    IPCPlotData data;

    if (plots[0]->upd && plots[1]->upd && plots[2]->upd && plots[3]->upd)
        if (readPlot(0, &data.realInX, &data.realInY) && \
                readPlot(1, &data.realOutX, &data.realOutY) && \
                readPlot(2, &data.fftInX, &data.fftInY) && \
                readPlot(3, &data.fftOutX, &data.fftOutY)) {
            emit updatePlotData(&data);
        }
}
