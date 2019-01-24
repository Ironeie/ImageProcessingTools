#include "hist.h"
#include "ui_hist.h"
#include <QLabel>

Hist::Hist(QWidget *parent, QImage histImg) :
    QDialog(parent),
    ui(new Ui::Hist)
{
    ui->setupUi(this);

    this->setWindowTitle(QStringLiteral("直方图"));
    int w = histImg.width();
    int h = histImg.height();
    this->resize(w+5, h+5);

    QLabel *label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(histImg));
    label->resize(QSize(w, h));
    ui->scrollImg->resize((QSize(w+2, h+3)));
    ui->scrollImg->setWidget(label);
}

Hist::~Hist()
{
    delete ui;
}
