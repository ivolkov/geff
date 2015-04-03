#ifndef IPCPLOTS_H
#define IPCPLOTS_H

#include <QObject>
#include <QTimer>
#include "qcustomplot/qcustomplot.h"
#include "ipc.h"
#include "eq.h"

class IPCPlots : public QObject
{
    Q_OBJECT
public:
    explicit IPCPlots(QCustomPlot *RealPlot, QCustomPlot *FFTPlot, EqDialog *eq, unsigned int SamplingRate, unsigned int FramesNum, QObject *parent = 0);
    ~IPCPlots();

private:
    QCustomPlot *plotReal;
    QCustomPlot *plotFFT;
    QCPItemText *lblFFT;
    QCPItemText *lblOverloadIn;
    QCPItemText *lblOverloadOut;

private slots:
    void plotMouseMove(QMouseEvent *event);

public slots:    
    void replot(IPCPlotData *data);
};

#endif // IPCPLOTS_H
