#include "color.h"
#include "ui_color.h"
#include <opencv2/opencv.hpp>

using namespace cv;

Color::Color(QWidget *parent, const QString title, std::vector<QString> channels, std::vector<QImage> images) :
    QDialog(parent),
    ui(new Ui::Color)
{
    ui->setupUi(this);

    this->setWindowTitle(title);

    ui->channel1->setText(channels[0]);
    ui->channel2->setText(channels[1]);
    ui->channel3->setText(channels[2]);

    Mat src = QImage2cvMat(images[0]);
    int h = src.rows;
    int w = src.cols;
    double h_scale = static_cast<double>(h) / 289;
    double w_scale = static_cast<double>(w) / 249;
    double max_scale = max(h_scale, w_scale);
    Mat tmp;
    cv::resize(src, tmp, Size(w/max_scale, h/max_scale));
    QImage Img_R = cvMat2QImage(tmp);
    QLabel *label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(Img_R));
    ui->scrollArea->setWidget(label);

    src = QImage2cvMat(images[1]);
    cv::resize(src, tmp, Size(w/max_scale, h/max_scale));
    QImage Img_G = cvMat2QImage(tmp);
    label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(Img_G));
    ui->scrollArea_2->setWidget(label);

    src = QImage2cvMat(images[2]);
    cv::resize(src, tmp, Size(w/max_scale, h/max_scale));
    QImage Img_B = cvMat2QImage(tmp);
    label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(Img_B));
    ui->scrollArea_3->setWidget(label);
}

Color::~Color()
{
    delete ui;
}

Mat Color::QImage2cvMat(QImage image)
{
    Mat mat;
    //qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage Color::cvMat2QImage(const Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
        if(mat.type() == CV_8UC1)
        {
            QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
            // Set the color table (used to translate colour indexes to qRgb values)
            image.setColorCount(256);
            for(int i = 0; i != 256; ++i)
            {
                image.setColor(i, qRgb(i, i, i));
            }
            // Copy input Mat
            uchar *pSrc = mat.data;
            for(int row = 0; row != mat.rows; ++row)
            {
                uchar *pDest = image.scanLine(row);
                memcpy(pDest, pSrc, mat.cols);
                pSrc += mat.step;
            }
            return image;
        }
        // 8-bits unsigned, NO. OF CHANNELS = 3
        else if(mat.type() == CV_8UC3)
        {
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        else if(mat.type() == CV_8UC4)
        {
            //qDebug() << "CV_8UC4";
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            return image.copy();
        }
        else
        {
            /*try
            {
                const uchar *pSrc = (const uchar*)mat.data;
                // Create QImage with same dimensions as input Mat
                QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
                return image.copy();
            }*/
            //QMessageBox::warning(this, QStringLiteral("程序异常"), QStringLiteral("Mat转化为QImage过程出错！"));
            return QImage();
        }
}
