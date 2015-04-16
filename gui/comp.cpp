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

void Comp::on_slThreshold_valueChanged(int value)
{
    ipc->comp->threshold = (double)value;
    ui->label_ThresholdValue->setText(QString::number(value));
}

void Comp::on_slRatio_valueChanged(int value)
{
    ipc->comp->ratio = (double)value;
    ui->label_RatioValue->setText(QString::number(value) + ":1");
}

void Comp::on_slAttack_valueChanged(int value)
{
    ipc->comp->attack_ms = (double)value;
    ui->label_AttackValue->setText(QString::number(value) + " ms");
}

void Comp::on_slRelease_valueChanged(int value)
{
    ipc->comp->release_ms = (double)value;
    ui->label_ReleaseValue->setText(QString::number(value) + " ms");
}
