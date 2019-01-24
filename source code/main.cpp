#include "imgproc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImgProc w;
    w.show();

    return a.exec();
}
