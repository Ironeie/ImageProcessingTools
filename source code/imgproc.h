#ifndef IMGPROC_H
#define IMGPROC_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QImage>
#include "combo.h"
#include "hist.h"
#include "fft.h"
#include "color.h"

namespace Ui {
class ImgProc;
}

class ImgProc : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImgProc(QWidget *parent = nullptr);
    ~ImgProc();
    cv::Mat QImage2cvMat(QImage);
    QImage cvMat2QImage(const cv::Mat&);

private slots:
    void receiveData(int);

private:
    Ui::ImgProc *ui;
    Combo *combo;
    Hist *hist;
    FFT *fft;
    Color *color;
    QImage srcImg;
    QImage srcImg_copy;
    QImage dstImg;
    int receiveInt;
    int cnt;
    void OnAddNew();
    void openFile();
    void saveFile();
    void DisplayImg();
    void RestoreImg();
    void GussianFilter();
    void toGray();
    void OnDegradation();
    void OnChangeGray();
    void OnDifference();
    void OnAddDenoise1();
    void OnAddDenoise2();
    void OnHistDisplay();
    void OnHistEqual();
    void OnHistMatch();
    void OnMeanFilt();
    void OnMedianFilt();
    void OnFFT();
    void OnIdealLPF();
    void OnIdealHPF();
    void OnButterworthLPF();
    void OnButterworthHPF();
    void OnGaussLPF();
    void OnGaussHPF();
    void OnSaltPepperNoise();
    void OnGuassianNoise();
    void OnAdaptiveMedianFilt();
    void OnRGBDisplay();
    void OnHSIDisplay();
    void OnRGBEqual();
    void OnHSIEqual();
    void OnRGBSeg();
    void OnHoughDetect();
    void OnGlobalSeg();
    void OnOtsu();
};

#endif // IMGPROC_H
