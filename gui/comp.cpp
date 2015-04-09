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
    ui->slRatio->setValue(int(ipc->norm->ratio));
    ui->slAttack->setValue(int(ipc->norm->attack_ms));
    ui->slRelease->setValue(int(ipc->norm->release_ms));

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

void Comp::on_slRatio_sliderMoved(int position)
{
    ipc->norm->ratio = (double)position;
}

void Comp::on_slAttack_sliderMoved(int position)
{
    ipc->norm->attack_ms = (double)position;
}

void Comp::on_slRelease_sliderMoved(int position)
{
    ipc->norm->release_ms = (double)position;
}
