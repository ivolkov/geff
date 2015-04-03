#include "plots.h"
#include <QFont>

IPCPlots::IPCPlots(QCustomPlot *RealPlot, QCustomPlot *FFTPlot, EqDialog *eq, unsigned int SamplingRate, unsigned int FramesNum, QObject *parent) : QObject(parent)
{
    plotReal = RealPlot;
    plotFFT = FFTPlot;

    plotReal->xAxis->setRange(0.0, (double)FramesNum / (double)SamplingRate);
    plotReal->yAxis->setRange(-32900.0, 32900.0);
    plotReal->xAxis->setTicks(false);
    plotReal->yAxis->setTicks(false);
    plotReal->legend->setVisible(true);

    plotFFT->xAxis->setRange(30.0, (double)SamplingRate / 2.0);
    plotFFT->yAxis->setRange(-120.0, 10.0);
    plotFFT->xAxis->setAutoTicks(false);
    plotFFT->xAxis->setScaleType(QCPAxis::stLogarithmic);
    plotFFT->xAxis->setTickVector(eq->getBands());    
    plotFFT->xAxis->setLabel("Frequency (Hz)");
    plotFFT->yAxis->setLabel("Power (dB)");
    plotFFT->legend->setVisible(true);

            // real data in graph
    plotReal->addGraph();
    plotReal->graph(0)->setPen(QPen(QBrush("black"), 1.5));
    plotReal->graph(0)->setName("Input");

            // real data out graph
    plotReal->addGraph();
    plotReal->graph(1)->setPen(QPen(QBrush("blue"), 1.5));
    plotReal->graph(1)->setName("Output");

            // real plot overload labels
    lblOverloadIn = new QCPItemText(plotReal);
    lblOverloadOut = new QCPItemText(plotReal);
    lblOverloadIn->setText("Input Overload");
    lblOverloadOut->setText("Output Overload");
    lblOverloadIn->position->setType(QCPItemPosition::ptViewportRatio);
    lblOverloadOut->position->setType(QCPItemPosition::ptViewportRatio);
    lblOverloadIn->position->setCoords(0.1, 0.1);
    lblOverloadOut->position->setCoords(0.1, 0.2);
    lblOverloadIn->setColor("black");
    lblOverloadOut->setColor("blue");
    lblOverloadIn->setVisible(false);
    lblOverloadOut->setVisible(false);

            // FFT in graph
    plotFFT->addGraph();
    plotFFT->graph(0)->setPen(QPen(QBrush("black"), 1.5));
    plotFFT->graph(0)->setName("Input");

            // FFT out graph
    plotFFT->addGraph();
    plotFFT->graph(1)->setPen(QPen(QBrush("blue"), 1.5));
    plotFFT->graph(1)->setName("Output");

            // FFT plot label
    lblFFT = new QCPItemText(plotFFT);
    plotFFT->addItem(lblFFT);
    lblFFT->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
    lblFFT->position->setType(QCPItemPosition::ptAxisRectRatio);
    lblFFT->position->setCoords(0.01, 0.95);
    lblFFT->setText("");
    lblFFT->setFont(QFont("Sans", 16));
    connect(plotFFT, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(plotMouseMove(QMouseEvent*)));
}

IPCPlots::~IPCPlots()
{
    lblFFT->deleteLater();
    lblOverloadIn->deleteLater();
    lblOverloadOut->deleteLater();
}

void IPCPlots::plotMouseMove(QMouseEvent *event)
{    
    //double val = plotFFT->xAxis->pixelToCoord(event->localPos().x());
    double val = plotFFT->xAxis->pixelToCoord(event->pos().x());
    lblFFT->setText(QString::number(val, 'f', 2));
}

void IPCPlots::replot(IPCPlotData *data)
{
    plotReal->graph(0)->setData(data->realInX, data->realInY);
    plotReal->graph(1)->setData(data->realOutX, data->realOutY);
    plotFFT->graph(0)->setData(data->fftInX, data->fftInY);
    plotFFT->graph(1)->setData(data->fftOutX, data->fftOutY);

    plotReal->replot();
    plotFFT->replot();

    bool overloadIn = false;
    for (int i = 0; i < data->realInY.size(); i++) {
        if ((data->realInY[i] > 32760.0) || (data->realInY[i] < -32760.0)) {
            overloadIn = true;
            break;
        }
    }

    bool overloadOut = false;
    for (int i = 0; i < data->realOutY.size(); i++) {
        if ((data->realOutY[i] > 32760.0) || (data->realOutY[i] < -32760.0)) {
            overloadOut = true;
            break;
        }
    }

    lblOverloadIn->setVisible(overloadIn);
    lblOverloadOut->setVisible(overloadOut);
}
