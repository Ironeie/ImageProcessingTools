#ifndef COLOR_H
#define COLOR_H

#include <QDialog>
#include <vector>
#include <opencv2/opencv.hpp>

namespace Ui {
class Color;
}

class Color : public QDialog
{
    Q_OBJECT

public:
    explicit Color(QWidget *parent = nullptr, const QString title="",
                   std::vector<QString> channels={"通道1", "通道2", "通道3"},
                   std::vector<QImage> images={QImage(), QImage(), QImage()});
    ~Color();
    cv::Mat QImage2cvMat(QImage image);
    QImage cvMat2QImage(const cv::Mat &mat);

private:
    Ui::Color *ui;
};

#endif // COLOR_H
