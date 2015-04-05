#include "eq.h"
#include "ui_eqdialog.h"

double EqDialog::interpolation(double bandStartFreq, double bandStartVal, double bandEndFreq, double bandEndVal, double frequency)
{
    double mu = (frequency - bandStartFreq) / (bandEndFreq - bandStartFreq);
    double mu2 = (1.0 - cos(mu * M_PI)) / 2.0;
    return (bandStartVal * (1.0 - mu2) + bandEndVal * mu2);
}

EqDialog::EqDialog(IPC *pIPC, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EqDialog)
{
    ui->setupUi(this);

    ipc = pIPC;

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

    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(QBrush(Qt::SolidPattern), 2.0));
    ui->plot->xAxis->setRange(QCPRange(bands.first(), bands.last()));
    ui->plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->plot->xAxis->setAutoTicks(false);
    ui->plot->xAxis->setTickVector(bands);

    connect(ui->sliderPreAmp, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider01, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider02, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider03, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider04, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider05, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider06, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider07, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider08, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider09, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));
    connect(ui->slider10, SIGNAL(valueChanged(int)), this, SLOT(updateEq()));

    updateEq();
}

EqDialog::~EqDialog()
{
    delete ui;
}

QVector<double> EqDialog::getBands()
{
    return bands;
}

void EqDialog::updateEq()
{
    QVector<double> eqValues;
    QVector<double> dataX, dataY;

    const double preAmp = (double)ui->sliderPreAmp->value() / 100.0;
    const double powFactor = 1.5;

    eqValues.append(pow((double)ui->slider01->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider02->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider03->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider04->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider05->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider06->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider07->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider08->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider09->value() * preAmp / 100.0, powFactor));
    eqValues.append(pow((double)ui->slider10->value() * preAmp / 100.0, powFactor));

    for (unsigned int i = 0; i < ipc->info->framesNum; i++) {
        double frequency = (double)i * (double)ipc->info->sampRate / (double)ipc->info->framesNum;
        double bandStartFreq = -1.0;
        double bandStartVal = -1.0;
        double bandEndFreq = -1.0;
        double bandEndVal = -1.0;


                // find band for current point
        if (frequency < bands.first()) {
            bandStartFreq = 0.0;
            bandStartVal = preAmp;
            bandEndFreq = bands.first();
            bandEndVal = eqValues.first();
        } else if (frequency > bands.last()) {
            bandStartFreq = bands.last();
            bandStartVal = eqValues.last();
            bandEndFreq = ipc->info->framesNum - 1;
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
    ui->plot->yAxis->rescale();
    ui->plot->yAxis->setRangeLower(0.0);
    ui->plot->replot();
}
