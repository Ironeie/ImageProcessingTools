#ifndef HIST_H
#define HIST_H

#include <QDialog>

namespace Ui {
class Hist;
}

class Hist : public QDialog
{
    Q_OBJECT

public:
    explicit Hist(QWidget *parent = nullptr, QImage histImg = QImage());
    ~Hist();

private:
    Ui::Hist *ui;
};

#endif // HIST_H
