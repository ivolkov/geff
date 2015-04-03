#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ipc.h"
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<QVector<double> >("QVector<double>");
    qRegisterMetaType<QVector<unsigned char> >("QVector<unsigned char>");

    eq = new EqDialog(ipc.info->sampRate, ipc.info->framesNum);
    plots = new IPCPlots(ui->plotReal, ui->plotFFT, eq, ipc.info->sampRate, ipc.info->framesNum);
    comp = new Comp(&ipc);    

    connect(&ipc, SIGNAL(updatePlotData(IPCPlotData*)), plots, SLOT(replot(IPCPlotData*)));

    enableRealData = ui->actionShow_real_data->isChecked();
}

MainWindow::~MainWindow()
{
    eq->deleteLater();
    comp->deleteLater();
    plots->deleteLater();
    delete ui;
}

void MainWindow::on_actionShow_real_data_triggered()
{
    ui->plotReal->setVisible(ui->actionShow_real_data->isChecked());
    enableRealData = ui->actionShow_real_data->isChecked();
}

void MainWindow::on_actionEqualizer_triggered()
{
    eq->setIPC(&ipc);
    eq->exec();
}

void MainWindow::on_actionNormalization_triggered()
{
    comp->exec();
}
