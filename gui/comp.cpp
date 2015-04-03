#include "comp.h"
#include "ui_compdialog.h"
#include <QDebug>

Comp::Comp(IPC *pIPC, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Comp)
{
    ui->setupUi(this);

    ipc = pIPC;

    ui->cbEnable->setChecked(ipc->norm->enable);
    ui->slThreshold->setValue(int(ipc->norm->threshold));
    ui->slGain->setValue(int(ipc->norm->gain * 100.0));

}

Comp::~Comp()
{
    delete ui;
}

void Comp::on_cbEnable_clicked()
{
    ipc->norm->enable = ui->cbEnable->isChecked();
}

void Comp::on_slThreshold_sliderMoved(int position)
{
    ipc->norm->threshold = (double)position;
}

void Comp::on_slGain_sliderMoved(int position)
{
    ipc->norm->gain = (double)position / 100.0;
}
