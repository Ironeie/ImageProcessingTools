#ifndef FFT_H
#define FFT_H

#include <QDialog>

namespace Ui {
class FFT;
}

class FFT : public QDialog
{
    Q_OBJECT

public:
    explicit FFT(QWidget *parent = nullptr, QImage fftImg = QImage());
    ~FFT();

private:
    Ui::FFT *ui;
};

#endif // FFT_H
