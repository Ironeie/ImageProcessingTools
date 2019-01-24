#include "combo.h"
#include "ui_combo.h"

Combo::Combo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Combo)
{
    ui->setupUi(this);

    this->setWindowTitle(QStringLiteral("改变灰度级"));
}

Combo::~Combo()
{
    delete ui;
}

void Combo::on_buttonBox_accepted()
{
    int k = 256;
    if (ui->comboBox->currentIndex() == 0)
        k = 128;
    if (ui->comboBox->currentIndex() == 1)
        k = 64;
    if (ui->comboBox->currentIndex() == 2)
        k = 32;
    if (ui->comboBox->currentIndex() == 3)
        k = 16;
    if (ui->comboBox->currentIndex() == 4)
        k = 8;
    if (ui->comboBox->currentIndex() == 5)
        k = 4;
    if (ui->comboBox->currentIndex() == 6)
        k = 2;
    if (k > 0)
        emit sendData(k);
}
