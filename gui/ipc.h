#ifndef IPC2_H
#define IPC2_H

#define PLOT_MAX_LEN 8192
#define PLOT_CNT 4

#include <QObject>
#include <QVector>
#include <QTimer>
#include <stdint.h>

struct IPCPlotData {
    QVector<double> realInX;
    QVector<double> realInY;
    QVector<double> realOutX;
    QVector<double> realOutY;
    QVector<double> fftInX;
    QVector<double> fftInY;
    QVector<double> fftOutX;
    QVector<double> fftOutY;
};

struct ShmPlot {
    bool upd;
    uint16_t len;
    double x[PLOT_MAX_LEN];
    double y[PLOT_MAX_LEN];
};

struct ShmComp {
    bool enable;
    double threshold;
    double gain;
};

struct ShmInfo {
    unsigned int sampRate;
    unsigned int framesNum;
};

class IPC : public QObject
{
    Q_OBJECT
private:
    char *plotsBuf;
    struct ShmPlot *plots[PLOT_CNT];    
    QTimer tmr;
    bool readPlot(int num, QVector<double> *dataX, QVector<double> *dataY);
    bool shmAttach(const char *fname, size_t size, char **shmPtr);
    void shmWrite(struct ShmPlot *plot, double *dataX, double *dataY, unsigned int len);
    bool shmRead(struct ShmPlot *plot, double *dataX, double *dataY, unsigned int *len);
    void magnToDb(double *dataY, unsigned int len);

public:
    explicit IPC(QObject *parent = 0);
    ~IPC();    
    double *eq;
    ShmComp *norm;
    struct ShmInfo *info;

signals:
    void updatePlotData(IPCPlotData *data);

private slots:
    void onTimer();
};

#endif // IPC2_H
