#include "fft.h"
#include "ui_fft.h"
#include <QLabel>

FFT::FFT(QWidget *parent, QImage(fftImg)) :
    QDialog(parent),
    ui(new Ui::FFT)
{
    ui->setupUi(this);

    this->setWindowTitle(QStringLiteral("频谱图"));
    int w = fftImg.width();
    int h = fftImg.height();
    this->resize(w+5, h+5);

    QLabel *label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(fftImg));
    label->resize(QSize(w, h));
    ui->scrollImg->resize((QSize(w+2, h+3)));
    ui->scrollImg->setWidget(label);
}

FFT::~FFT()
{
    delete ui;
}
