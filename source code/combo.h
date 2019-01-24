#ifndef COMBO_H
#define COMBO_H

#include <QDialog>

namespace Ui {
class Combo;
}

class Combo : public QDialog
{
    Q_OBJECT

public:
    explicit Combo(QWidget *parent = nullptr);
    ~Combo();
    //int GrayLevel;
    //friend class ImgProc;

private slots:
    void on_buttonBox_accepted();

signals:
    void sendData(int);

private:
    Ui::Combo *ui;
};

#endif // COMBO_H
