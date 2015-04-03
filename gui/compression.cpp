#include "compression.h"
#include "ui_norm.h"
#include <QDebug>

Compression::Compression(IPC *pIPC, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Compression)
{
    ui->setupUi(this);

    ipc = pIPC;

    ui->cbEnable->setChecked(ipc->norm->enable);
    ui->slThreshold->setValue(int(ipc->norm->threshold));
    ui->slOutputLevel->setValue(int(ipc->norm->gain * 100));
}

Compression::~Compression()
{
    delete ui;
}

void Compression::updateInputLevel(double value)
{
    ui->pbInputLevel->setValue(int(value));
}

void Compression::on_cbEnable_clicked()
{
    ipc->norm->enable = ui->cbEnable->isChecked();
}

void Compression::on_slThreshold_sliderMoved(int position)
{
    ipc->norm->threshold = (double)position;
}

void Compression::on_slOutputLevel_sliderMoved(int position)
{
    ipc->norm->gain = (double)(position) / 100.0;
}
