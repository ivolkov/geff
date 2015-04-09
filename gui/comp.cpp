#include "comp.h"
#include "ui_compdialog.h"
#include <QDebug>

Comp::Comp(IPC *pIPC, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Comp)
{
    ui->setupUi(this);

    ipc = pIPC;

    ui->cbEnable->setChecked(ipc->comp->enable);
    ui->slThreshold->setValue(int(ipc->comp->threshold));
    ui->slRatio->setValue(int(ipc->comp->ratio));
    ui->slAttack->setValue(int(ipc->comp->attack_ms));
    ui->slRelease->setValue(int(ipc->comp->release_ms));

}

Comp::~Comp()
{
    delete ui;
}

void Comp::on_cbEnable_clicked()
{
    ipc->comp->enable = ui->cbEnable->isChecked();
}

void Comp::on_slThreshold_sliderMoved(int position)
{
    ipc->comp->threshold = (double)position;
}

void Comp::on_slRatio_sliderMoved(int position)
{
    ipc->comp->ratio = (double)position;
}

void Comp::on_slAttack_sliderMoved(int position)
{
    ipc->comp->attack_ms = (double)position;
}

void Comp::on_slRelease_sliderMoved(int position)
{
    ipc->comp->release_ms = (double)position;
}
