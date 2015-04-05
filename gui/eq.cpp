#include "eq.h"
#include "ui_eqdialog.h"

double EqDialog::interpolation(double bandStartFreq, double bandStartVal, double bandEndFreq, double bandEndVal, double frequency)
{
    double mu = (frequency - bandStartFreq) / (bandEndFreq - bandStartFreq);
    double mu2 = (1.0 - cos(mu * M_PI)) / 2.0;
    return (bandStartVal * (1.0 - mu2) + bandEndVal * mu2);
}

EqDialog::EqDialog(unsigned int SamplingRate, unsigned int FramesNum, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EqDialog)
{
    ui->setupUi(this);

    sampRate = SamplingRate;
    framesNum = FramesNum;

    bands.append(30.0);
    bands.append(60.0);
    bands.append(120.0);
    bands.append(200.0);
    bands.append(400.0);
    bands.append(800.0);
    bands.append(1600.0);
    bands.append(3200.0);
    bands.append(6400.0);
    bands.append(12800.0);

    ipc = NULL;

    ui->plot->addGraph();
    ui->plot->xAxis->setRange(QCPRange(bands.first(), bands.last()));
    ui->plot->yAxis->setRange(QCPRange(0.0, 2.0));
    ui->plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->plot->xAxis->setAutoTicks(false);
    ui->plot->xAxis->setTickVector(bands);
}

EqDialog::~EqDialog()
{
    delete ui;
}

void EqDialog::setIPC(IPC *pIPC)
{
    ipc = pIPC;

    connect(ui->slider01, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider02, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider03, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider04, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider05, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider06, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider07, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider08, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider09, SIGNAL(sliderReleased()), this, SLOT(updateEq()));
    connect(ui->slider10, SIGNAL(sliderReleased()), this, SLOT(updateEq()));

    updateEq();
}

QVector<double> EqDialog::getBands()
{
    return bands;
}

void EqDialog::updateEq()
{
    QVector<double> eqValues;
    QVector<double> dataX, dataY;

    if (ipc == NULL) {
        qDebug() << "EqDialog::updateEq() : ipc not set";
        return;
    }

    const double powFactor = 1.5;

    eqValues.append(pow((double)ui->slider01->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider02->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider03->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider04->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider05->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider06->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider07->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider08->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider09->value() / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider10->value() / 100.0, powFactor));

    for (unsigned int i = 0; i < framesNum; i++) {
        double frequency = (double)i * (double)sampRate / (double)framesNum;
        double bandStartFreq = -1.0;
        double bandStartVal = -1.0;
        double bandEndFreq = -1.0;
        double bandEndVal = -1.0;


                // find band for current point
        if (frequency < bands.first()) {
            bandStartFreq = 0.0;
            bandStartVal = 1.0;
            bandEndFreq = bands.first();
            bandEndVal = eqValues.first();
        } else if (frequency > bands.last()) {
            bandStartFreq = bands.last();
            bandStartVal = eqValues.last();
            bandEndFreq = framesNum - 1;
            bandEndVal = 1.0;
        } else {
            for (int j = 1; j < bands.size(); j++)
                if (frequency < bands[j]) {
                    bandStartFreq = bands[j-1];
                    bandStartVal = eqValues[j-1];
                    bandEndFreq = bands[j];
                    bandEndVal = eqValues[j];
                    break;
                }
        }

        if ((bandStartFreq == -1.0) || (bandEndFreq == -1.0)) {
            qDebug() << "EqDialog::updateEq(): enable to determine band range";
            break;
        }

        ipc->eq[i] = interpolation(bandStartFreq, bandStartVal, bandEndFreq, bandEndVal, frequency);

        dataX.append(frequency);
        dataY.append(ipc->eq[i]);
    }

    ui->plot->graph(0)->setData(dataX, dataY);
    ui->plot->replot();
}
