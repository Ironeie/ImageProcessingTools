#include "imgproc.h"
#include "ui_imgproc.h"

#include <QFileDialog>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <opencv2/opencv.hpp>

#include <vector>
#include <limits>

using namespace cv;
using std::vector;
using std::rand;
using std::numeric_limits;

ImgProc::ImgProc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImgProc)
{
    ui->setupUi(this);

    QIcon icon("icon.jpg");
    this->setWindowIcon(icon);
    this->setWindowTitle(QStringLiteral("图像处理工具"));
    this->setWindowState(Qt::WindowMaximized);

    ui->scrollImg->setWidgetResizable(true);

    //“文件”菜单
    QMenu *file = ui->menuBar->addMenu(QStringLiteral("文件(&F)"));
    file->setShortcutEnabled(true);

    QAction *AddNew = new QAction(QStringLiteral("新建"), this);
    file->addAction(AddNew);
    AddNew->setStatusTip(QStringLiteral("清空所有图片"));
    connect(AddNew, &QAction::triggered, this, &ImgProc::OnAddNew);

    QAction *open = new QAction(QStringLiteral("打开"), this);
    file->addAction(open);
    open->setShortcuts(QKeySequence::Open);
    open->setStatusTip(QStringLiteral("打开一张图片"));
    connect(open, &QAction::triggered, this, &ImgProc::openFile);

    QAction *save = new QAction(QStringLiteral("保存"), this);
    file->addAction(save);
    save->setShortcuts(QKeySequence::SaveAs);
    save->setStatusTip(QStringLiteral("保存为图片文件"));
    connect(save, &QAction::triggered, this, &ImgProc::saveFile);

    //“效果”菜单
    QMenu *effect = ui->menuBar->addMenu(QStringLiteral("效果(&E)"));

    QAction *Gray = new QAction(QStringLiteral("转换为灰度图"), this);
    effect->addAction(Gray);
    Gray->setStatusTip(QStringLiteral("将彩色图片转换为灰度图"));
    connect(Gray, &QAction::triggered, this, &ImgProc::toGray);

    QMenu *Noise = new QMenu(QStringLiteral("添加噪声"), this);
    effect->addMenu(Noise);

    QAction *SaltPepperNoise = new QAction(QStringLiteral("椒盐噪声"), this);
    Noise->addAction(SaltPepperNoise);
    SaltPepperNoise->setStatusTip(QStringLiteral("为图片添加椒盐噪声"));
    connect(SaltPepperNoise, &QAction::triggered, this, &ImgProc::OnSaltPepperNoise);

    QAction *GuassianNoise = new QAction(QStringLiteral("高斯噪声"), this);
    Noise->addAction(GuassianNoise);
    GuassianNoise->setStatusTip(QStringLiteral("为图片添加高斯噪声"));
    connect(GuassianNoise, &QAction::triggered, this, &ImgProc::OnGuassianNoise);

    //“空间变换”菜单
    QMenu *spatial = ui->menuBar->addMenu(QStringLiteral("空间变换"));

    QAction *Degradation = new QAction(QStringLiteral("降低分辨率"), this);
    spatial->addAction(Degradation);
    Degradation->setStatusTip(QStringLiteral("降至为低空间分辨率"));
    connect(Degradation, &QAction::triggered, this, &ImgProc::OnDegradation);

    QAction *ChangeGray = new QAction(QStringLiteral("改变灰度级"), this);
    spatial->addAction(ChangeGray);
    ChangeGray->setStatusTip(QStringLiteral("分解为不同灰度分辨率"));
    connect(ChangeGray, &QAction::triggered, this, &ImgProc::OnChangeGray);

    QAction *Difference = new QAction(QStringLiteral("图像差分"), this);
    spatial->addAction(Difference);
    Difference->setStatusTip(QStringLiteral("图像差分"));
    connect(Difference, &QAction::triggered, this, &ImgProc::OnDifference);

    QMenu *AddDenoise = new QMenu(QStringLiteral("叠加去噪"), this);
    spatial->addMenu(AddDenoise);
    QAction *AddDenoise1 = new QAction(QStringLiteral("文件夹内所有图像叠加"), this);
    AddDenoise1->setStatusTip(QStringLiteral("选择文件夹对其内所有图像叠加去噪"));
    AddDenoise->addAction(AddDenoise1);
    connect(AddDenoise1, &QAction::triggered, this, &ImgProc::OnAddDenoise1);
    QAction *AddDenoise2 = new QAction(QStringLiteral("手动单次叠加"), this);
    AddDenoise2->setStatusTip(QStringLiteral("需已打开一张图片，选择欲叠加去噪的图片"));
    AddDenoise->addAction(AddDenoise2);
    connect(AddDenoise2, &QAction::triggered, this, &ImgProc::OnAddDenoise2);

    //“直方图&空域滤波”菜单
    QMenu *hist_and_spatial_filter = ui->menuBar->addMenu(QStringLiteral("直方图&&空域滤波"));

    QAction *HistDisplay = new QAction(QStringLiteral("显示直方图"), this);
    hist_and_spatial_filter->addAction(HistDisplay);
    HistDisplay->setStatusTip(QStringLiteral("统计直方图并显示"));
    connect(HistDisplay, &QAction::triggered, this, &ImgProc::OnHistDisplay);

    QAction *HistEqual = new QAction(QStringLiteral("直方图均衡"), this);
    hist_and_spatial_filter->addAction(HistEqual);
    HistEqual->setStatusTip(QStringLiteral("直方图均衡化"));
    connect(HistEqual, &QAction::triggered, this, &ImgProc::OnHistEqual);

    QAction *HistMatch = new QAction(QStringLiteral("直方图匹配"), this);
    hist_and_spatial_filter->addAction(HistMatch);
    HistMatch->setStatusTip(QStringLiteral("将直方图与所选图片匹配"));
    connect(HistMatch, &QAction::triggered, this, &ImgProc::OnHistMatch);

    QAction *MeanFilt = new QAction(QStringLiteral("均值滤波"), this);
    hist_and_spatial_filter->addAction(MeanFilt);
    MeanFilt->setStatusTip(QStringLiteral("对图像进行均值滤波"));
    connect(MeanFilt, &QAction::triggered, this, &ImgProc::OnMeanFilt);

    QAction *MedianFilt = new QAction(QStringLiteral("中值滤波"), this);
    hist_and_spatial_filter->addAction(MedianFilt);
    MedianFilt->setStatusTip(QStringLiteral("对图像进行中值滤波"));
    connect(MedianFilt, &QAction::triggered, this, &ImgProc::OnMedianFilt);

    QAction *Gussian = new QAction(QStringLiteral("高斯滤波"), this);
    hist_and_spatial_filter->addAction(Gussian);
    Gussian->setStatusTip(QStringLiteral("对图片进行高斯滤波"));
    connect(Gussian, &QAction::triggered, this, &ImgProc::GussianFilter);

    QAction *AdaptiveMedianFilt = new QAction(QStringLiteral("自适应中值滤波"), this);
    hist_and_spatial_filter->addAction(AdaptiveMedianFilt);
    AdaptiveMedianFilt->setStatusTip(QStringLiteral("对图像进行自适应中值滤波"));
    connect(AdaptiveMedianFilt, &QAction::triggered, this, &ImgProc::OnAdaptiveMedianFilt);

    //“频域滤波”菜单
    QMenu *frequency_filter = ui->menuBar->addMenu(QStringLiteral("频域滤波"));

    QAction *FFT = new QAction(QStringLiteral("FFT变换"), this);
    frequency_filter->addAction(FFT);
    FFT->setStatusTip(QStringLiteral("对图像进行FFT变换"));
    connect(FFT, &QAction::triggered, this, &ImgProc::OnFFT);

    QMenu *Ideal = new QMenu(QStringLiteral("理想滤波器"), this);
    frequency_filter->addMenu(Ideal);
    Ideal->setStatusTip(QStringLiteral("用理想滤波器对图像进行频域滤波"));
    QAction *IdealLPF = new QAction(QStringLiteral("低通"), this);
    Ideal->addAction(IdealLPF);
    connect(IdealLPF, &QAction::triggered, this, &ImgProc::OnIdealLPF);
    QAction *IdealHPF = new QAction(QStringLiteral("高通"), this);
    Ideal->addAction(IdealHPF);
    connect(IdealHPF, &QAction::triggered, this, &ImgProc::OnIdealHPF);

    QMenu *Butterworth = new QMenu(QStringLiteral("巴特沃斯滤波器"), this);
    frequency_filter->addMenu(Butterworth);
    Butterworth->setStatusTip(QStringLiteral("用理想滤波器对图像进行频域滤波"));
    QAction *ButterworthLPF = new QAction(QStringLiteral("低通"), this);
    Butterworth->addAction(ButterworthLPF);
    connect(ButterworthLPF, &QAction::triggered, this, &ImgProc::OnButterworthLPF);
    QAction *ButterworthHPF = new QAction(QStringLiteral("高通"), this);
    Butterworth->addAction(ButterworthHPF);
    connect(ButterworthHPF, &QAction::triggered, this, &ImgProc::OnButterworthHPF);

    QMenu *Gauss = new QMenu(QStringLiteral("高斯滤波器"), this);
    frequency_filter->addMenu(Gauss);
    Butterworth->setStatusTip(QStringLiteral("用高斯滤波器对图像进行频域滤波"));
    QAction *GaussLPF = new QAction(QStringLiteral("低通"), this);
    Gauss->addAction(GaussLPF);
    connect(GaussLPF, &QAction::triggered, this, &ImgProc::OnGaussLPF);
    QAction *GaussHPF = new QAction(QStringLiteral("高通"), this);
    Gauss->addAction(GaussHPF);
    connect(GaussHPF, &QAction::triggered, this, &ImgProc::OnGaussHPF);

    //“彩色空间处理”菜单
    QMenu *color = ui->menuBar->addMenu(QStringLiteral("彩色空间处理"));

    QAction *RGBDisplay = new QAction(QStringLiteral("分别显示RGB分量"), this);
    color->addAction(RGBDisplay);
    RGBDisplay->setStatusTip(QStringLiteral("分别显示RGB分量"));
    connect(RGBDisplay, &QAction::triggered, this, &ImgProc::OnRGBDisplay);

    QAction *HSIDisplay = new QAction(QStringLiteral("分别显示HSI分量"), this);
    color->addAction(HSIDisplay);
    HSIDisplay->setStatusTip(QStringLiteral("分别显示HSI分量"));
    connect(HSIDisplay, &QAction::triggered, this, &ImgProc::OnHSIDisplay);

    QMenu *ColorEqual = new QMenu(QStringLiteral("图像均衡"), this);
    color->addMenu(ColorEqual);
    QAction *RGBEqual = new QAction(QStringLiteral("RGB均衡"), this);
    ColorEqual->addAction(RGBEqual);
    RGBEqual->setStatusTip(QStringLiteral("分别对RGB分量做均衡"));
    connect(RGBEqual, &QAction::triggered, this, &ImgProc::OnRGBEqual);
    QAction *HSIEqual = new QAction(QStringLiteral("HSI均衡"), this);
    ColorEqual->addAction(HSIEqual);
    HSIEqual->setStatusTip(QStringLiteral("分别对HSI分量做均衡"));
    connect(HSIEqual, &QAction::triggered, this, &ImgProc::OnHSIEqual);

    QAction *RGBSeg = new QAction(QStringLiteral("RGB分割"), this);
    color->addAction(RGBSeg);
    RGBSeg->setStatusTip(QStringLiteral("人工画框以该区域彩色分量平均值为阈值分割"));
    connect(RGBSeg, &QAction::triggered, this, &ImgProc::OnRGBSeg);

    //“分割与检测”菜单
    QMenu *seg_and_detect = ui->menuBar->addMenu(QStringLiteral("分割与检测"));

    QAction *HoughDetect = new QAction(QStringLiteral("霍夫变换检测直线"), this);
    seg_and_detect->addAction(HoughDetect);
    HoughDetect->setStatusTip(QStringLiteral("利用霍夫变换检测图片的直线"));
    connect(HoughDetect, &QAction::triggered, this, &ImgProc::OnHoughDetect);

    QMenu *segment = new QMenu(QStringLiteral("图像分割"), this);
    seg_and_detect->addMenu(segment);
    QAction *GlobalSeg = new QAction(QStringLiteral("基本全局阈值法"), this);
    segment->addAction(GlobalSeg);
    GlobalSeg->setStatusTip(QStringLiteral("基本全局阈值法分割图像"));
    connect(GlobalSeg, &QAction::triggered, this, &ImgProc::OnGlobalSeg);
    QAction *Otsu = new QAction(QStringLiteral("最大方差法"), this);
    segment->addAction(Otsu);
    Otsu->setStatusTip(QStringLiteral("最大方差法分割图像"));
    connect(Otsu, &QAction::triggered, this, &ImgProc::OnOtsu);

    //恢复原图
    QAction *Restore = ui->menuBar->addAction(QStringLiteral("恢复原图"));
    Restore->setStatusTip(QStringLiteral("恢复最近一次打开的图片"));
    connect(Restore, &QAction::triggered, this, &ImgProc::RestoreImg);
}

ImgProc::~ImgProc()
{
    delete ui;
    QFile file("tmp.bmp");
    //删除程序产生的临时图片
    if (file.exists())
        file.remove();
}

void ImgProc::receiveData(int k)
{
    if (k > 0)
        receiveInt = k;
}

void ImgProc::OnAddNew()
{
    srcImg = srcImg_copy = dstImg = QImage();
    cnt = 1;
    DisplayImg();
}

void ImgProc::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), ".",
                                                QStringLiteral("图片文件(*.jpg *.png *.bmp)"));
    if(!path.isEmpty())
    {
        srcImg.load(path);
        srcImg_copy = srcImg;
        dstImg = srcImg;
        cnt = 1;
        DisplayImg();
    }
}

void ImgProc::saveFile()
{
    if (!dstImg.isNull())
    {
        QString path = QFileDialog::getSaveFileName(this, QStringLiteral("保存图片"), "new.png",
                                                    QStringLiteral("图片文件(*.jpg *.png *.bmp)"));
        if (!path.isEmpty() && !dstImg.isNull())
        {
            dstImg.save(path);
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("图片为空！"));
}

void ImgProc::DisplayImg()
{
    QLabel *label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(dstImg));
    label->resize(QSize(dstImg.width(),dstImg.height()));
    ui->scrollImg->resize((QSize(dstImg.width(), dstImg.height())));
    ui->scrollImg->setWidget(label);
}

void ImgProc::RestoreImg()
{
    srcImg = srcImg_copy;
    dstImg = srcImg_copy;
    cnt = 1;
    DisplayImg();
}

Mat ImgProc::QImage2cvMat(QImage image)
{
    Mat mat;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cvtColor(mat, mat, COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage ImgProc::cvMat2QImage(const Mat &mat)
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
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            return image.copy();
        }
        else
        {
            QMessageBox::warning(this, QStringLiteral("程序异常"), QStringLiteral("Mat转化为QImage过程出错！"));
            return QImage();
        }
}

void ImgProc::GussianFilter()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        bool ok;
        int i = QInputDialog::getInt(this, QStringLiteral("滤波器窗口尺寸"),
                                     QStringLiteral("请输入滤波器窗口尺寸（奇数）"),
                                     5, 1, 15, 2, &ok);
        if (ok)
        {
            if (i % 2 == 0)
                QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请输入奇数！"));
            else
            {
                GaussianBlur(src, dst, Size(i, i), 0, 0);
                dstImg = cvMat2QImage(dst);
                srcImg = dstImg;
                DisplayImg();
            }
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::toGray()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() != 1)
            cvtColor(src, dst, COLOR_BGR2GRAY);
        else
            dst = src;
        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnDegradation()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        int height = src.rows;
        int width = src.cols;
        bool ok;
        double i = QInputDialog::getDouble(this, QStringLiteral("降质系数"),
                                     QStringLiteral("请输入降质系数（0.1~1）"),
                                     0.5, 0.01, 1, 2, &ok);
        if (ok)
        {
            cv::resize(src, dst, Size(i * width, i * height));
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnChangeGray()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        int height = src.rows;
        int width = src.cols;
        if (src.channels() != 1)
            cvtColor(src, dst, COLOR_BGR2GRAY);
        else
            dst = src;
        combo = new Combo(this);
        combo->setModal(true);
        connect(combo, SIGNAL(sendData(int)), this, SLOT(receiveData(int)));
        if(combo->exec() == QDialog::Accepted)
        {
            if (receiveInt > 0)
            {
                int k = 256 / receiveInt;
                for (int i = 0; i != height; ++i)
                {
                    uchar* data = dst.ptr<uchar>(i);
                    for (int j = 0; j != width; ++j)
                        data[j] = data[j] / k * k;
                }
                dstImg = cvMat2QImage(dst);
                srcImg = dstImg;
                DisplayImg();
            }
        }
        delete combo;
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnDifference()
{ 
    if (!dstImg.isNull())
    {
        QString path = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), ".",
                                                    QStringLiteral("图片文件(*.jpg *.png *.bmp)"));
        if (!path.isEmpty())
        {
            Mat src = QImage2cvMat(srcImg);
            Mat dst;
            int height = src.rows;
            int width = src.cols;
            int channel = src.channels();
            QImage tmpImg;
            tmpImg.load(path);
            Mat tmp = QImage2cvMat(tmpImg);
            int height2 = tmp.rows;
            int width2 = tmp.cols;
            int channel2 = tmp.channels();
            if (height != height2 || width != width2 || channel != channel2)
                 QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("图片尺寸和通道数必须相同！"));
            else
            {
                absdiff(src, tmp, dst);
                imwrite("tmp.bmp", dst); //差分后的图转化为QImage有BUG无法显示，只好先保存一次
                dstImg.load("tmp.bmp");
                srcImg = dstImg;
                DisplayImg();
            }
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

//方式一：对整个文件夹操作
void ImgProc::OnAddDenoise1()
{
    QDir dir = QFileDialog::getExistingDirectory(this, QStringLiteral("选择路径"),// ".",
                                                QStringLiteral("F:/DIP/实验用图/叠加去噪"));
    if (!dir.exists())
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("路径错误！"));
    QStringList filters;
    filters << QString("*.jpg");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);
    int dir_count = dir.count();
    if (dir_count == 0 && dir.absolutePath() != "F:/temp/Qt/build-imgproc-MSVC2017_64bit-Debug" &&
                          dir.absolutePath() != "F:/temp/Qt/build-imgproc-MSVC2017_64bit-Release")
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("该路径下无jpg文件！"));
    QStringList string_list;
    for (int i = 0; i != dir_count; ++i)
    {
        QString file_name = dir[i];  //文件名称
        QString file_path = dir.absolutePath();
        file_path.append("/").append(file_name);
        string_list.append(file_path);
    }
    Mat dst;
    for (auto it = string_list.begin(); it != string_list.end(); ++it)
    {
        dstImg.load(*it);
        Mat tmp = QImage2cvMat(dstImg);
        if (it == string_list.begin())
        {
            dst = tmp/dir_count;
        }
        else
        {
            dst += tmp/dir_count;
        }
    }
    dstImg = cvMat2QImage(dst);
    srcImg = dstImg;
    DisplayImg();
}

//方式二：每次叠加手动操作
void ImgProc::OnAddDenoise2()
{
    if (!dstImg.isNull())
    {
        QString path = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), ".",
                                                    QStringLiteral("图片文件(*.jpg *.png *.bmp)"));
        if (!path.isEmpty())
        {
            Mat src = QImage2cvMat(srcImg);
            Mat dst;
            int height = src.rows;
            int width = src.cols;
            int channel = src.channels();
            QImage tmpImg;
            tmpImg.load(path);
            Mat tmp = QImage2cvMat(tmpImg);
            int height2 = tmp.rows;
            int width2 = tmp.cols;
            int channel2 = tmp.channels();
            if (height != height2 || width != width2 || channel != channel2)
                 QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("图片尺寸和通道数必须相同！"));
            else
            {
                dst = src * cnt/(cnt+1) + tmp / (cnt+1);
                cnt += 1;
                dstImg = cvMat2QImage(dst);
                srcImg = dstImg;
                DisplayImg();
            }
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnHistDisplay()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        int channels = 0;
        MatND dstHist;
        int histSize[] = { 600 };       //如果写成int histSize = 256;调用计算直方图的函数的时，该变量需写成&histSize
        float midRanges[] = { 0, 256 };
        const float *ranges[] = { midRanges };
        calcHist(&src, 1, &channels, Mat(), dstHist, 1, histSize, ranges);
        //绘制直方图,首先先创建一个黑底的图像，为了可以显示彩色，所以该绘制图像是一个8位的3通道图像
        Mat drawImage = Mat::zeros(Size(600, 450), CV_8UC3);
        //任何一个图像的某个像素的总个数有可能会很多，甚至超出所定义的图像的尺寸，
        //所以需要先对个数进行范围的限制，用minMaxLoc函数来得到计算直方图后的像素的最大个数
        double g_dHistMaxValue;
        minMaxLoc(dstHist, nullptr, &g_dHistMaxValue);
        //将像素的个数整合到图像的最大范围内
        for (int i = 0; i != 600; ++i)
        {
            int value = cvRound(dstHist.at<float>(i) * 450 * 0.9 / g_dHistMaxValue);
            line(drawImage, Point(i, drawImage.rows - 1), Point(i, drawImage.rows - 1 - value), Scalar(0, 0, 255));
        }
        QImage histImg = cvMat2QImage(drawImage);
        hist = new Hist(this, histImg);
        hist->setModal(false);
        hist->show();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnHistEqual()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() == 1)
            equalizeHist(src, dst);
        else
        {
            Mat YCC;
            cvtColor(src, YCC, COLOR_RGB2YCrCb);
            vector<Mat> channels;
            split(YCC, channels);

            equalizeHist(channels[0], channels[0]);//对Y通道进行均衡化

            merge(channels, YCC);
            cvtColor(YCC, dst, COLOR_YCrCb2RGB);
        }
        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

//灰度图
void ImgProc::OnHistMatch()
{
    if (!dstImg.isNull())
    {
        QString path = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), ".",
                                                    QStringLiteral("图片文件(*.jpg *.png *.bmp)"));
        if (!path.isEmpty())
        {
            Mat src = QImage2cvMat(srcImg);
            dstImg.load(path);
            Mat tmp = QImage2cvMat(dstImg);
            // 首先把图像转化成灰度图像
            if (src.channels() != 1)
                cvtColor(src, src, COLOR_BGR2GRAY);
            if (tmp.channels() != 1)
                cvtColor(tmp, tmp, COLOR_BGR2GRAY);
            // 先对图像进行均衡化处理
            Mat src_copy;
            src.copyTo(src_copy);
            equalizeHist(src, src);
            equalizeHist(tmp, tmp);
            // 新建目标图像同样大小和类型的图像
            Mat dst(src.cols, src.rows, CV_32FC1);
            MatND src_hist, dst_hist;
            int dims = 1;
            float hranges[] = { 0,255 };
            const float *ranges[] = { hranges };
            int size = 256;
            int channels = 0;

            // 计算模板图像和目标图像的直方图
            calcHist(&tmp, 1, &channels, Mat(), src_hist, dims, &size, ranges);
            calcHist(&src, 1, &channels, Mat(), dst_hist, dims, &size, ranges);
            // 得到模板图像和目标图像的累积直方图
            float src_cdf[256] = { 0 };
            float dst_cdf[256] = { 0 };
            for (int i = 0; i != 256; ++i)
            {
                if (i == 0)
                {
                    src_cdf[i] = src_hist.at<float>(i);
                    dst_cdf[i] = dst_hist.at<float>(i);
                }
                else
                {
                    src_cdf[i] = src_cdf[i - 1] + src_hist.at<float>(i);
                    dst_cdf[i] = dst_cdf[i - 1] + dst_hist.at<float>(i);
                }
            }

            // 对目标图像进行规定化处理
            // 1.计算累积概率的差值
            float diff_cdf[256][256];
            for (size_t i = 0; i != 256; ++i)
            {
                for (size_t j = 0; j != 256; ++j)
                {
                    diff_cdf[i][j] = fabs(src_cdf[i] - dst_cdf[j]);
                }
            }

            //2.构建灰度级映射表
            Mat lut(1, 256, CV_8U);
            for (int i = 0; i != 256; ++i)
            {
                //查找源灰度级为i的映射灰度和i的累积概率差最小的规定化灰度
                float min = diff_cdf[i][0];
                int index = 0;
                for (int j = 0; j != 256; ++j)
                {
                    if (min > diff_cdf[i][j])
                    {
                        min = diff_cdf[i][j];
                        index = j;
                    }
                }
                lut.at<uchar>(i) = static_cast<uchar>(index);
            }
            // 应用查找表得到均衡化后的图像
            LUT(src_copy, lut, dst);
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

//彩色图，未完成
/*
void ImgProc::OnHistMatch()
{
    if (!dstImg.isNull())
    {
        QString path = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), ".",
                                                    QStringLiteral("图片文件(*.jpg *.png *.bmp)"));
        if (!path.isEmpty())
        {
            Mat src = QImage2cvMat(srcImg);
            dstImg.load(path);
            Mat tmp = QImage2cvMat(dstImg);
            // 首先把图像转化成灰度图像
            if (src.channels() != 1)
                cvtColor(src, src, COLOR_BGR2GRAY);
            if (tmp.channels() != 1)
                cvtColor(tmp, tmp, COLOR_BGR2GRAY);
            // 先对图像进行均衡化处理
            Mat src_copy;
            src.copyTo(src_copy);
            Mat src_YCC, tmp_YCC;
            cvtColor(src, src_YCC, COLOR_RGB2YCrCb);
            cvtColor(tmp, tmp_YCC, COLOR_RGB2YCrCb);
            vector<Mat> src_channels, tmp_channels;
            split(src, src_channels);
            split(tmp, tmp_channels);
            equalizeHist(src_channels[0], src_channels[0]);
            equalizeHist(tmp_channels[0], tmp_channels[0]);
            merge(src_channels, src_YCC);
            merge(tmp_channels, tmp_YCC);
            cvtColor(src_YCC, src, COLOR_YCrCb2RGB);
            cvtColor(tmp_YCC, tmp, COLOR_YCrCb2RGB);
            // 新建目标图像同样大小和类型的图像
            Mat dst(src.cols, src.rows, CV_32FC3);
            MatND src_hist, dst_hist;
            int dims = 1;
            float hranges[] = { 0,255 };
            const float *ranges[] = { hranges };
            int size = 256;
            int channels = 0;

            // 计算模板图像和目标图像的直方图
            calcHist(&tmp, 1, &channels, Mat(), src_hist, dims, &size, ranges);
            calcHist(&src, 1, &channels, Mat(), dst_hist, dims, &size, ranges);
            // 得到模板图像和目标图像的累积直方图
            float src_cdf[256] = { 0 };
            float dst_cdf[256] = { 0 };
            for (int i = 0; i < 256; i++) {
                if (i == 0) {
                    src_cdf[i] = src_hist.at<float>(i);
                    dst_cdf[i] = dst_hist.at<float>(i);
                } else {
                    src_cdf[i] = src_cdf[i - 1] + src_hist.at<float>(i);
                    dst_cdf[i] = dst_cdf[i - 1] + dst_hist.at<float>(i);
                }
            }

            // 对目标图像进行规定化处理
            // 1.计算累积概率的差值
            float diff_cdf[256][256];
            for (int i = 0; i < 256; i++) {
                for (int j = 0; j < 256; j++) {
                    diff_cdf[i][j] = fabs(src_cdf[i] - dst_cdf[j]);
                }
            }

            //2.构建灰度级映射表
            Mat lut(1, 256, CV_8U);
            for (int i = 0; i < 256; i++) {
                //查找源灰度级为i的映射灰度和i的累积概率差最小的规定化灰度
                float min = diff_cdf[i][0];
                int index = 0;
                for (int j = 0; j < 256; j++) {
                    if (min > diff_cdf[i][j]) {
                        min = diff_cdf[i][j];
                        index = j;
                    }
                }
                lut.at<uchar>(i) = static_cast<uchar>(index);
            }
            // 应用查找表得到均衡化后的图像
            LUT(src_copy, lut, dst);
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}
*/

void ImgProc::OnMeanFilt()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        bool ok;
        int i = QInputDialog::getInt(this, QStringLiteral("滤波器窗口尺寸"),
                                     QStringLiteral("请输入滤波器窗口尺寸（奇数）"),
                                     5, 1, 15, 1, &ok);
        if (ok)
            {
                blur(src, dst, Size(i, i));
                dstImg = cvMat2QImage(dst);
                srcImg = dstImg;
                DisplayImg();
            }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnMedianFilt()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        bool ok;
        int i = QInputDialog::getInt(this, QStringLiteral("滤波器窗口尺寸"),
                                     QStringLiteral("请输入滤波器窗口尺寸（奇数）"),
                                     5, 1, 15, 2, &ok);
        if (ok)
        {
            if (i % 2 == 0)
                QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请输入奇数！"));
            else
            {
                medianBlur(src, dst, i);
                dstImg = cvMat2QImage(dst);
                srcImg = dstImg;
                DisplayImg();
            }
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnFFT()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        if (src.channels() != 1)
            cvtColor(src, src, COLOR_BGR2GRAY);
        int m = getOptimalDFTSize( src.rows );
        int n = getOptimalDFTSize( src.cols );
        //将添加的像素初始化为0.
        Mat padded;
        copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));

        //【3】为傅立叶变换的结果(实部和虚部)分配存储空间。
        //将planes数组组合合并成一个多通道的数组complexI
        Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
        Mat complexI;
        merge(planes, 2, complexI);

        //【4】进行就地离散傅里叶变换
        dft(complexI, complexI);

        //【5】将复数转换为幅值，即=> log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
        split(complexI, planes); // 将多通道数组complexI分离成几个单通道数组，planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
        magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
        Mat magnitudeImage = planes[0];

        //【6】进行对数尺度(logarithmic scale)缩放
        magnitudeImage += Scalar::all(1);
        log(magnitudeImage, magnitudeImage);//求自然对数

        //【7】剪切和重分布幅度图象限
        //若有奇数行或奇数列，进行频谱裁剪
        magnitudeImage = magnitudeImage(Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));
        //重新排列傅立叶图像中的象限，使得原点位于图像中心
        int cx = magnitudeImage.cols/2;
        int cy = magnitudeImage.rows/2;
        Mat q0(magnitudeImage, Rect(0, 0, cx, cy));   // ROI区域的左上
        Mat q1(magnitudeImage, Rect(cx, 0, cx, cy));  // ROI区域的右上
        Mat q2(magnitudeImage, Rect(0, cy, cx, cy));  // ROI区域的左下
        Mat q3(magnitudeImage, Rect(cx, cy, cx, cy)); // ROI区域的右下
        //交换象限（左上与右下进行交换）
        Mat tmp;
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);
        //交换象限（右上与左下进行交换）
        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);

        //【8】归一化，用0到1之间的浮点值将矩阵变换为可视的图像格式
        normalize(magnitudeImage, magnitudeImage, 0, 255, NORM_MINMAX);

        //【9】显示效果图
        Mat dst;
        double Min, Max;
        minMaxLoc(magnitudeImage, &Min, &Max);
        magnitudeImage.convertTo(dst, CV_8U);
        QImage fftImg = cvMat2QImage(dst);
        fft = new FFT(this, fftImg);
        fft->setModal(false);
        fft->show();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnIdealLPF()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        bool ok;
        int D0_max = min(src.rows, src.cols);
        int i = QInputDialog::getInt(this, QStringLiteral("滤波半径"),
                                     QStringLiteral("请输入滤波半径"),
                                     60, 1, D0_max, 1, &ok);
        if (ok)
        {
            Mat img;
            if (src.channels() != 1)
                cvtColor(src, img, COLOR_BGR2GRAY);
            else
                img = src;
            //调整图像加速傅里叶变换
            int M = getOptimalDFTSize(img.rows);
            int N = getOptimalDFTSize(img.cols);
            Mat padded;
            copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));
            //记录傅里叶变换的实部和虚部
            Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
            Mat complexImg;
            merge(planes, 2, complexImg);
            //进行傅里叶变换
            dft(complexImg, complexImg);
            //获取图像
            Mat mag = complexImg;
            mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));//这里为什么&上-2具体查看opencv文档
            //其实是为了把行和列变成偶数 -2的二进制是11111111.......10 最后一位是0
            //获取中心点坐标
            int cx = mag.cols / 2;
            int cy = mag.rows / 2;
            //调整频域
            Mat tmp;
            Mat q0(mag, Rect(0, 0, cx, cy));
            Mat q1(mag, Rect(cx, 0, cx, cy));
            Mat q2(mag, Rect(0, cy, cx, cy));
            Mat q3(mag, Rect(cx, cy, cx, cy));

            q0.copyTo(tmp);
            q3.copyTo(q0);
            tmp.copyTo(q3);

            q1.copyTo(tmp);
            q2.copyTo(q1);
            tmp.copyTo(q2);
            //Do为自己设定的阀值具体看公式
            double D0 = i;
            //处理按公式保留中心部分
            for (int y = 0; y != mag.rows; ++y)
            {
                double* data = mag.ptr<double>(y);
                for (int x = 0; x != mag.cols; ++x)
                {
                    double d = sqrt(pow((y - cy),2) + pow((x - cx),2));
                    if (d > D0)
                        data[x] = 0;
                }
            }
            //再调整频域
            q0.copyTo(tmp);
            q3.copyTo(q0);
            tmp.copyTo(q3);
            q1.copyTo(tmp);
            q2.copyTo(q1);
            tmp.copyTo(q2);
            //逆变换
            Mat invDFT, invDFTcvt;
            idft(mag, invDFT, DFT_SCALE | DFT_REAL_OUTPUT); // Applying IDFT
            invDFT.convertTo(invDFTcvt, CV_8U);
            dstImg = cvMat2QImage(invDFTcvt);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnIdealHPF()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        bool ok;
        int D0_max = min(src.rows, src.cols);
        int i = QInputDialog::getInt(this, QStringLiteral("滤波半径"),
                                     QStringLiteral("请输入滤波半径"),
                                     60, 1, D0_max, 1, &ok);
        if (ok)
        {
            Mat img;
            if (src.channels() != 1)
                cvtColor(src, img, COLOR_BGR2GRAY);
            else
                img = src;
            //调整图像加速傅里叶变换
            int M = getOptimalDFTSize(img.rows);
            int N = getOptimalDFTSize(img.cols);
            Mat padded;
            copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));
            //记录傅里叶变换的实部和虚部
            Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
            Mat complexImg;
            merge(planes, 2, complexImg);
            //进行傅里叶变换
            dft(complexImg, complexImg);
            //获取图像
            Mat mag = complexImg;
            mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));//这里为什么&上-2具体查看opencv文档
            //其实是为了把行和列变成偶数 -2的二进制是11111111.......10 最后一位是0
            //获取中心点坐标
            int cx = mag.cols / 2;
            int cy = mag.rows / 2;
            //调整频域
            Mat tmp;
            Mat q0(mag, Rect(0, 0, cx, cy));
            Mat q1(mag, Rect(cx, 0, cx, cy));
            Mat q2(mag, Rect(0, cy, cx, cy));
            Mat q3(mag, Rect(cx, cy, cx, cy));

            q0.copyTo(tmp);
            q3.copyTo(q0);
            tmp.copyTo(q3);

            q1.copyTo(tmp);
            q2.copyTo(q1);
            tmp.copyTo(q2);
            //Do为自己设定的阀值具体看公式
            double D0 = i;
            //处理按公式保留中心部分
            for (int y = 0; y != mag.rows; ++y)
            {
                double* data = mag.ptr<double>(y);
                for (int x = 0; x != mag.cols; ++x)
                {
                    double d = sqrt(pow((y - cy),2) + pow((x - cx),2));
                    if (d < D0)
                        data[x] = 0;
                }
            }
            //再调整频域
            q0.copyTo(tmp);
            q3.copyTo(q0);
            tmp.copyTo(q3);
            q1.copyTo(tmp);
            q2.copyTo(q1);
            tmp.copyTo(q2);
            //逆变换
            Mat invDFT, invDFTcvt;
            idft(mag, invDFT, DFT_SCALE | DFT_REAL_OUTPUT); // Applying IDFT
            invDFT.convertTo(invDFTcvt, CV_8U);
            dstImg = cvMat2QImage(invDFTcvt);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnButterworthLPF()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        int n = 1;//表示巴特沃斯滤波器的次数
        //H = 1 / (1+(D/D0)^2n)
        Mat img;
        if (src.channels() != 1)
            cvtColor(src, img, COLOR_BGR2GRAY);
        else
            img = src;
        bool ok;
        int D0_max = min(src.rows, src.cols);
        int i = QInputDialog::getInt(this, QStringLiteral("滤波半径"),
                                     QStringLiteral("请输入滤波半径"),
                                     60, 1, D0_max, 1, &ok);
        //调整图像加速傅里叶变换
        int M = getOptimalDFTSize(img.rows);
        int N = getOptimalDFTSize(img.cols);
        Mat padded;
        copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));

        Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
        Mat complexImg;
        merge(planes, 2, complexImg);

        dft(complexImg, complexImg);

        Mat mag = complexImg;
        mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));

        int cx = mag.cols / 2;
        int cy = mag.rows / 2;

        Mat tmp;
        Mat q0(mag, Rect(0, 0, cx, cy));
        Mat q1(mag, Rect(cx, 0, cx, cy));
        Mat q2(mag, Rect(0, cy, cx, cy));
        Mat q3(mag, Rect(cx, cy, cx, cy));

        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);

        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);

        double D0 = i;

        for (int y = 0; y != mag.rows; ++y)
        {
            double* data = mag.ptr<double>(y);
            for (int x = 0; x != mag.cols; ++x)
            {
                double d = sqrt(pow((y - cy), 2) + pow((x - cx), 2));
                double h = 1.0 / (1 + pow(d / D0, 2 * n));
                if (h <= 0.5)
                    data[x] = 0;
            }
        }
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);
        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);
        //逆变换
        Mat invDFT, invDFTcvt;
        idft(complexImg, invDFT, DFT_SCALE | DFT_REAL_OUTPUT); // Applying IDFT
        invDFT.convertTo(invDFTcvt, CV_8U);
        dstImg = cvMat2QImage(invDFTcvt);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnButterworthHPF()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        int n = 1;//表示巴特沃斯滤波器的次数
        //H = 1 / (1+(D/D0)^2n)
        Mat img;
        if (src.channels() != 1)
            cvtColor(src, img, COLOR_BGR2GRAY);
        else
            img = src;
        bool ok;
        int D0_max = min(src.rows, src.cols);
        int i = QInputDialog::getInt(this, QStringLiteral("滤波半径"),
                                     QStringLiteral("请输入滤波半径"),
                                     60, 1, D0_max, 1, &ok);
        //调整图像加速傅里叶变换
        int M = getOptimalDFTSize(img.rows);
        int N = getOptimalDFTSize(img.cols);
        Mat padded;
        copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));

        Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
        Mat complexImg;
        merge(planes, 2, complexImg);

        dft(complexImg, complexImg);

        Mat mag = complexImg;
        mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));

        int cx = mag.cols / 2;
        int cy = mag.rows / 2;

        Mat tmp;
        Mat q0(mag, Rect(0, 0, cx, cy));
        Mat q1(mag, Rect(cx, 0, cx, cy));
        Mat q2(mag, Rect(0, cy, cx, cy));
        Mat q3(mag, Rect(cx, cy, cx, cy));

        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);

        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);

        double D0 = i;

        for (int y = 0; y != mag.rows; ++y)
        {
            double* data = mag.ptr<double>(y);
            for (int x = 0; x != mag.cols; ++x)
            {
                double d = sqrt(pow((y - cy), 2) + pow((x - cx), 2));
                double h = 1.0 / (1 + pow(d / D0, 2 * n));
                if (h > 0.5)
                    data[x] = 0;
            }
        }
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);
        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);
        //逆变换
        Mat invDFT, invDFTcvt;
        idft(complexImg, invDFT, DFT_SCALE | DFT_REAL_OUTPUT); // Applying IDFT
        invDFT.convertTo(invDFTcvt, CV_8U);
        dstImg = cvMat2QImage(invDFTcvt);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnGaussLPF()
{
    if (!dstImg.isNull())
    {
        Mat input = QImage2cvMat(srcImg);
        bool ok;
        int D0_max = min(input.rows, input.cols);
        int i = QInputDialog::getInt(this, QStringLiteral("滤波半径"),
                                     QStringLiteral("请输入滤波半径"),
                                     60, 1, D0_max, 1, &ok);
        if (input.channels() != 1)
            cvtColor(input, input, COLOR_BGR2GRAY);
        int w = getOptimalDFTSize(input.cols);
        int h = getOptimalDFTSize(input.rows);
        Mat padded;
        copyMakeBorder(input,padded,0,h-input.rows,0,w-input.cols,BORDER_CONSTANT,Scalar::all(0));
        padded.convertTo(padded,CV_32FC1);
        for(int i=0;i<padded.rows;i++)//中心化操作，其余操作和上一篇博客的介绍一样
        {
            float *ptr=padded.ptr<float>(i);
            for(int j=0;j<padded.cols;j++)	ptr[j]*=pow(-1,i+j);
        }
        Mat plane[]={padded,Mat::zeros(padded.size(),CV_32F)};
        Mat complexImg;
        merge(plane,2,complexImg);
        dft(complexImg,complexImg);
        //************************gaussian****************************
        Mat gaussianBlur(padded.size(),CV_32FC2);
        double D0=i*20;
        for(int i=0;i<padded.rows;i++)
        {
            float*p=gaussianBlur.ptr<float>(i);
            for(int j=0;j<padded.cols;j++)
            {
                float d=pow(i-padded.rows/2,2)+pow(j-padded.cols/2,2);
                p[2*j]=expf(-d/D0);
                p[2*j+1]=expf(-d/D0);
            }
        }
        multiply(complexImg,gaussianBlur,gaussianBlur);//矩阵元素对应相乘法，注意，和矩阵相乘区分
        //*****************************************************************
        split(complexImg,plane);
        magnitude(plane[0],plane[1],plane[0]);
        plane[0]+=Scalar::all(1);
        log(plane[0],plane[0]);
        normalize(plane[0],plane[0],1,0,CV_MINMAX);
        //******************************************************************
        split(gaussianBlur,plane);
        magnitude(plane[0],plane[1],plane[0]);
        plane[0]+=Scalar::all(1);
        log(plane[0],plane[0]);
        normalize(plane[0],plane[0],1,0,CV_MINMAX);

        //******************************************************************
        //*************************idft*************************************
        idft(gaussianBlur,gaussianBlur);
        split(gaussianBlur,plane);
        magnitude(plane[0],plane[1],plane[0]);
        normalize(plane[0],plane[0],255,0,CV_MINMAX);
        Mat dst;
        plane[0].convertTo(dst, CV_8U);
        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnGaussHPF()
{
    if (!dstImg.isNull())
    {
        Mat input = QImage2cvMat(srcImg);
        bool ok;
        int D0_max = min(input.rows, input.cols);
        int i = QInputDialog::getInt(this, QStringLiteral("滤波半径"),
                                     QStringLiteral("请输入滤波半径"),
                                     60, 1, D0_max, 1, &ok);
        if (input.channels() != 1)
            cvtColor(input, input, COLOR_BGR2GRAY);
        int w = getOptimalDFTSize(input.cols);
        int h = getOptimalDFTSize(input.rows);
        Mat padded;
        copyMakeBorder(input,padded,0,h-input.rows,0,w-input.cols,BORDER_CONSTANT,Scalar::all(0));
        padded.convertTo(padded,CV_32FC1);
        for(int i=0;i<padded.rows;i++)//中心化操作，其余操作和上一篇博客的介绍一样
        {
            float *ptr=padded.ptr<float>(i);
            for(int j=0;j<padded.cols;j++)	ptr[j]*=pow(-1,i+j);
        }
        Mat plane[]={padded,Mat::zeros(padded.size(),CV_32F)};
        Mat complexImg;
        merge(plane,2,complexImg);
        dft(complexImg,complexImg);
        //************************gaussian****************************
        Mat gaussianSharpen(padded.size(),CV_32FC2);
        float D0=i*20;
        for(int i=0;i<padded.rows;i++)
        {
            float*q=gaussianSharpen.ptr<float>(i);
            for(int j=0;j<padded.cols;j++)
            {
                float d=pow(i-padded.rows/2,2)+pow(j-padded.cols/2,2);
                q[2*j]=1-expf(-d/D0);
                q[2*j+1]=1-expf(-d/D0);
            }
        }
        multiply(complexImg,gaussianSharpen,gaussianSharpen);
        //*****************************************************************
        split(complexImg,plane);
        magnitude(plane[0],plane[1],plane[0]);
        plane[0]+=Scalar::all(1);
        log(plane[0],plane[0]);
        normalize(plane[0],plane[0],1,0,CV_MINMAX);
        //******************************************************************
        split(gaussianSharpen,plane);
        magnitude(plane[0],plane[1],plane[0]);
        plane[0]+=Scalar::all(1);
        log(plane[0],plane[0]);
        normalize(plane[0],plane[0],1,0,CV_MINMAX);
        //******************************************************************
        //*************************idft*************************************
        idft(gaussianSharpen,gaussianSharpen);

        split(gaussianSharpen,plane);
        magnitude(plane[0],plane[1],plane[0]);
        normalize(plane[0],plane[0],255,0,CV_MINMAX);
        Mat dst;
        plane[0].convertTo(dst, CV_8U);
        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

//盐噪声
Mat salt(const Mat &src, double p)
{
    Mat dst;
    src.copyTo(dst);
    int height = dst.rows;
    int width = dst.cols;
    int n = height * width * p;
    for (int k = 0; k != n; ++k)
    {
        int i = rand() % height; // %整除取余数运算符,rand=1022,cols=1000,rand%cols=22
        int j = rand() % width;
        if (dst.channels() == 1)
        {
            dst.at<uchar>(i, j)= 255; //at方法需要指定Mat变量返回值类型,如uchar等
        }
        else
        {
            dst.at<Vec3b>(i, j)[0]= 255; //cv::Vec3b为opencv定义的一个3个值的向量类型
            dst.at<Vec3b>(i, j)[1]= 255; //[]指定通道，B:0，G:1，R:2
            dst.at<Vec3b>(i, j)[2]= 255;
        }
    }
    return dst;
}

//椒噪声
Mat pepper(const Mat &src, double p)
{
    Mat dst;
    src.copyTo(dst);
    int height = dst.rows;
    int width = dst.cols;
    int n = height * width * p;
    for (int k = 0; k != n; ++k)
    {
        int i = rand() % height; // %整除取余数运算符,rand=1022,cols=1000,rand%cols=22
        int j = rand() % width;
        if (dst.channels() == 1)
        {
            dst.at<uchar>(i, j)= 0; //at方法需要指定Mat变量返回值类型,如uchar等
        }
        else
        {
            dst.at<Vec3b>(i, j)[0]= 0; //cv::Vec3b为opencv定义的一个3个值的向量类型
            dst.at<Vec3b>(i, j)[1]= 0; //[]指定通道，B:0，G:1，R:2
            dst.at<Vec3b>(i, j)[2]= 0;
        }
    }
    return dst;
}

void ImgProc::OnSaltPepperNoise()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        imwrite("tmp.bmp", src);
        src = imread("tmp.bmp"); //未知BUG，必须先保存再读取一次，否则噪点会错位
        bool ok1;
        double salt_probability = QInputDialog::getDouble(this, QStringLiteral("盐噪声概率"),
                                     QStringLiteral("请输入盐噪声概率（0.1~1）"),
                                     0.05, 0.01, 1, 2, &ok1);
        if (ok1)
        {
            bool ok2;
            double pepper_probability = QInputDialog::getDouble(this, QStringLiteral("椒噪声概率"),
                                         QStringLiteral("请输入椒噪声概率（0.1~1）"),
                                         0.05, 0.01, 1, 2, &ok2);
            Mat dst = pepper(salt(src, salt_probability), pepper_probability);
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

double generateGaussianNoise(double mu, double sigma)
{
    //定义一个特别小的值
    const double epsilon = numeric_limits<double>::min();//返回目标数据类型能表示的最逼近1的正数和1的差的绝对值
    static double z0, z1;
    static bool flag = false;
    flag = !flag;
    //flag为假，构造高斯随机变量
    if (!flag)
        return z1*sigma + mu;
    double u1, u2;
    //构造随机变量

    do
    {
        u1 = rand()*(1.0 / RAND_MAX);
        u2 = rand()*(1.0 / RAND_MAX);
    } while (u1 <= epsilon);
    //flag为真构造高斯随机变量X
    z0 = sqrt(-2.0*log(u1))*cos(2 * CV_PI * u2);
    z1 = sqrt(-2.0*log(u1))*sin(2 * CV_PI * u2);
    return z1*sigma + mu;
}

//为图像添加高斯噪声
Mat addGaussianNoise(Mat& srcImage, int mu, double sigma)
{
    Mat resultImage = srcImage.clone();    //深拷贝,克隆
    int channels = resultImage.channels();    //获取图像的通道
    int nRows = resultImage.rows;    //图像的行数
    int nCols = resultImage.cols * channels;   //图像的总列数
    //判断图像的连续性
    if (resultImage.isContinuous())    //判断矩阵是否连续，若连续，我们相当于只需要遍历一个一维数组
    {
        nCols *= nRows;
        nRows = 1;
    }
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {	//添加高斯噪声
            int val = resultImage.ptr<uchar>(i)[j] + generateGaussianNoise(mu, sigma) * 32;
            if (val < 0)
                val = 0;
            if (val > 255)
                val = 255;
            resultImage.ptr<uchar>(i)[j] = (uchar)val;
        }
    }
    return resultImage;
}

void ImgProc::OnGuassianNoise()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        imwrite("tmp.bmp", src);
        src = imread("tmp.bmp"); //未知BUG，必须先保存再读取一次，否则噪点会错位
        bool ok1;
        int mu = QInputDialog::getInt(this, QStringLiteral("噪声均值"),
                                     QStringLiteral("请输入盐噪声均值"),
                                     0, -255, 255, 1, &ok1);
        if (ok1)
        {
            bool ok2;
            double sigma = QInputDialog::getDouble(this, QStringLiteral("噪声方差"),
                                         QStringLiteral("请输入椒噪声方差"),
                                         1, 0.1, 10, 1, &ok2);
            Mat dst = addGaussianNoise(src, mu, sigma);
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

uchar adaptiveProcess(const Mat &im, int row,int col,int kernelSize,int maxSize)
{
    vector<uchar> pixels;
    for (int a = -kernelSize / 2; a <= kernelSize / 2; a++)
        for (int b = -kernelSize / 2; b <= kernelSize / 2; b++)
        {
            pixels.push_back(im.at<uchar>(row + a, col + b));
        }
    sort(pixels.begin(), pixels.end());
    auto min = pixels[0];
    auto max = pixels[kernelSize * kernelSize - 1];
    auto med = pixels[kernelSize * kernelSize / 2];
    auto zxy = im.at<uchar>(row, col);
    if (med > min && med < max)
    {
        // to B
        if (zxy > min && zxy < max)
            return zxy;
        else
            return med;
    }
    else
    {
        kernelSize += 2;
        if (kernelSize <= maxSize)
            return adaptiveProcess(im, row, col, kernelSize, maxSize); // 增大窗口尺寸，继续A过程。
        else
            return med;
    }
}

void ImgProc::OnAdaptiveMedianFilt()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        src.copyTo(dst);
        imwrite("tmp.bmp", src);
        src = imread("tmp.bmp"); //未知BUG，必须先保存再读取一次，否则噪点会错位
        bool ok;
        int maxSize = QInputDialog::getInt(this, QStringLiteral("滤波器最大尺寸"),
                                     QStringLiteral("请输入滤波器最大尺寸（奇数）"),
                                     5, 3, 255, 2, &ok);
        if (ok)
        {
            if (maxSize % 2 == 0)
                QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请输入奇数！"));
            else
            {
                for (int j = maxSize / 2; j < src.rows - maxSize / 2; j++)
                    {
                        for (int i = maxSize / 2; i < src.cols * src.channels() - maxSize / 2; i++)
                        {
                            dst.at<uchar>(j, i) = adaptiveProcess(src, j, i, 1, maxSize);
                        }
                    }
                dstImg = cvMat2QImage(dst);
                srcImg = dstImg;
                DisplayImg();
                QMessageBox::warning(this, QStringLiteral("错误提示"),
                                     QStringLiteral("本方法结果有误，请等待后续版本修复"));
            }
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}
void ImgProc::OnRGBDisplay()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        if (src.channels() == 1)
            QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开彩色图片！"));
        else
        {
            vector<Mat> m;
            split(src, m);
            vector<Mat> Rchannels, Gchannels, Bchannels;
            //提取通道信息，把orig的通道数据复制到channals
            split(src, Rchannels);
            split(src, Gchannels);
            split(src, Bchannels);
            //将其他通道信息设置为0
            Rchannels[1] = 0;
            Rchannels[2] = 0;
            Gchannels[0] = 0;
            Gchannels[2] = 0;
            Bchannels[0] = 0;
            Bchannels[1] = 0;
            //合并通道信息
            merge(Rchannels, m[0]);
            merge(Gchannels, m[1]);
            merge(Bchannels, m[2]);
            //转换为QImage
            QImage dst_B = cvMat2QImage(m[0]);
            QImage dst_G = cvMat2QImage(m[1]);
            QImage dst_R = cvMat2QImage(m[2]);
            QString title(QStringLiteral("RGB分量"));
            vector<QString> channels = { "R", "G", "B"};
            vector<QImage> images = { dst_R, dst_G, dst_B };
            color = new Color(this, title, channels, images);
            color->setModal(false);
            color->show();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnHSIDisplay()
{
    if (!dstImg.isNull())
    {
        Mat LeafBGRImage = QImage2cvMat(srcImg);
        if (LeafBGRImage.channels() == 1)
            QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开彩色图片！"));
        else
        {
            Mat LeafHSIImage = Mat(Size(LeafBGRImage.cols, LeafBGRImage.rows), CV_8UC3);

            vector <Mat> channels;
            split(LeafHSIImage, channels);
            Mat Hvalue = channels.at(0);
            Mat Svalue = channels.at(1);
            Mat Ivalue = channels.at(2);

            for (int i = 0; i != LeafBGRImage.rows; ++i)
                for (int j = 0; j != LeafBGRImage.cols; ++j)
                {
                    double H, S, I;
                    int Bvalue = LeafBGRImage.at<Vec3b>(i, j)[0];
                    int Gvalue = LeafBGRImage.at<Vec3b>(i, j)[1];
                    int Rvalue = LeafBGRImage.at<Vec3b>(i, j)[2];

                    double numerator = ((Rvalue - Gvalue) + (Rvalue - Bvalue)) / 2;
                    double denominator = sqrt(pow((Rvalue - Gvalue), 2) + (Rvalue - Bvalue)*(Gvalue - Bvalue));
                    if (denominator == 0)
                        H = 0;
                    else
                    {
                        double Theta = acos(numerator / denominator) * 180 / 3.14;
                        if (Bvalue <= Gvalue)
                            H = Theta;
                        else
                            H = 360 - Theta;
                    }
                    Hvalue.at<uchar>(i, j) = (int)(H * 255 / 360); //为了显示将[0~360]映射到[0~255]

                    //求S = 1-3*min(Bvalue,Gvalue,Rvalue)/(Rvalue+Gvalue+Bvalue);
                    int minvalue = Bvalue;
                    if (minvalue > Gvalue)
                        minvalue = Gvalue;
                    if (minvalue > Rvalue)
                        minvalue = Rvalue;
                    numerator = 3 * minvalue;
                    denominator = Rvalue + Gvalue + Bvalue;
                    if (denominator == 0)
                        S = 0;
                    else
                        S = 1 - numerator / denominator;
                    Svalue.at<uchar>(i, j) = static_cast<int>(S * 255);//为了显示将[0~1]映射到[0~255]

                    I = (Rvalue + Gvalue + Bvalue) / 3;
                    Ivalue.at<uchar>(i, j) = static_cast<int>(I);

                    //debug
                    H = Bvalue;
                    S = Gvalue;
                    I = Rvalue;
                }
            //转换为QImage
            QImage dst_H = cvMat2QImage(channels[0]);
            QImage dst_S = cvMat2QImage(channels[1]);
            QImage dst_I = cvMat2QImage(channels[2]);
            QString title(QStringLiteral("HSI分量"));
            vector<QString> channels_HSI = { "H", "S", "I"};
            vector<QImage> images = { dst_H, dst_S, dst_I };
            color = new Color(this, title, channels_HSI, images);
            color->setModal(false);
            color->show();
            QMessageBox::warning(this, QStringLiteral("错误提示"),
                                 QStringLiteral("本方法结果有误，请等待后续版本修复"));
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnRGBEqual()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() == 1)
            QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开彩色图片！"));
        else
        {
            vector<Mat> channels;
            split(src, channels);
            for (size_t i = 0; i != 3; ++i)
                equalizeHist(channels[i], channels[i]); //对RGB通道进行均衡化
            merge(channels, dst);
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnHSIEqual()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() == 1)
            QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开彩色图片！"));
        else
        {
            Mat HSI = Mat(Size(src.cols, src.rows), CV_8UC3);

            vector <Mat> channels;
            split(HSI, channels);
            Mat Hvalue = channels.at(0);
            Mat Svalue = channels.at(1);
            Mat Ivalue = channels.at(2);

            for (int i = 0; i != src.rows; ++i)
                for (int j = 0; j != src.cols; ++j)
                {
                    double H, S, I;
                    int Bvalue = src.at<Vec3b>(i, j)[0];
                    int Gvalue = src.at<Vec3b>(i, j)[1];
                    int Rvalue = src.at<Vec3b>(i, j)[2];

                    double numerator = ((Rvalue - Gvalue) + (Rvalue - Bvalue)) / 2;
                    double denominator = sqrt(pow((Rvalue - Gvalue), 2) + (Rvalue - Bvalue)*(Gvalue - Bvalue));
                    if (denominator == 0)
                        H = 0;
                    else
                    {
                        double Theta = acos(numerator / denominator) * 180 / 3.14;
                        if (Bvalue <= Gvalue)
                            H = Theta;
                        else
                            H = 360 - Theta;
                    }
                    Hvalue.at<uchar>(i, j) = (int)(H * 255 / 360); //为了显示将[0~360]映射到[0~255]

                    int minvalue = Bvalue;
                    if (minvalue > Gvalue)
                        minvalue = Gvalue;
                    if (minvalue > Rvalue)
                        minvalue = Rvalue;
                    numerator = 3 * minvalue;
                    denominator = Rvalue + Gvalue + Bvalue;
                    if (denominator == 0)
                        S = 0;
                    else
                        S = 1 - numerator / denominator;
                    Svalue.at<uchar>(i, j) = static_cast<int>(S * 255); //为了显示将[0~1]映射到[0~255]

                    I = (Rvalue + Gvalue + Bvalue) / 3;
                    Ivalue.at<uchar>(i, j) = static_cast<int>(I);
                }

            equalizeHist(channels[2], channels[2]); //对I通道进行均衡化

            merge(channels, HSI);
            dstImg = cvMat2QImage(HSI);
            srcImg = dstImg;
            DisplayImg();

            QMessageBox::warning(this, QStringLiteral("错误提示"),
                                 QStringLiteral("本方法结果有误，请点击“恢复原图”后在“直方图&空域滤波->直方图匹配”中使用此功能"));
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnRGBSeg()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() == 1)
            QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开彩色图片！"));
        else
        {
            vector<Mat> m;
            split(src, m);
            Mat m_b, m_g, m_r, sd_b, sd_g, sd_r;
            meanStdDev(m[0], m_b, sd_b);
            meanStdDev(m[1], m_g, sd_g);
            meanStdDev(m[2], m_r, sd_r);
            double mean_b = m_b.at<double>(0, 0);
            double mean_g = m_g.at<double>(0, 0);
            double mean_r = m_r.at<double>(0, 0);
            double std_b = sd_b.at<double>(0, 0);
            double std_g = sd_g.at<double>(0, 0);
            double std_r = sd_r.at<double>(0, 0);
            int b_min = static_cast<int>(mean_b - std_b);
            int b_max = static_cast<int>(mean_b + std_b);
            int g_min = static_cast<int>(mean_g - std_g);
            int g_max = static_cast<int>(mean_g + std_g);
            int r_min = static_cast<int>(mean_r - std_r);
            int r_max = static_cast<int>(mean_r + std_r);
            Mat dst(Size(src.cols, src.rows), CV_8UC1, Scalar(0));
            for (int i = 0; i != src.rows; ++i)
                for (int j = 0; j != src.cols; ++j)
                    if (src.at<Vec3b>(i, j)[0] >= b_min && src.at<Vec3b>(i, j)[0] <= b_max &&
                        src.at<Vec3b>(i, j)[1] >= g_min && src.at<Vec3b>(i, j)[1] <= g_max &&
                        src.at<Vec3b>(i, j)[2] >= r_min && src.at<Vec3b>(i, j)[2] <= r_max)
                        dst.at<uchar>(i, j) = 255;
            dstImg = cvMat2QImage(dst);
            srcImg = dstImg;
            DisplayImg();
            QMessageBox::warning(this, QStringLiteral("错误提示"),
                                 QStringLiteral("本方法结果有误，请等待后续版本修复"));
        }
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnHoughDetect()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat mid, dst;
        Canny(src, mid, 50, 200, 3);
        if (src.channels() != 1)
        {
            cvtColor(src, dst, COLOR_BGR2GRAY);
            cvtColor(dst, dst, COLOR_GRAY2BGR);
        }
        else
            cvtColor(src, dst, COLOR_GRAY2BGR);

        //标准霍夫变换，效果不佳
        /*vector<Vec2f> lines;
        HoughLines(mid, lines, 1, CV_PI/180, 150, 0, 0);
        for (size_t i = 0; i != lines.size(); ++i)
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line(dst, pt1, pt2, Scalar(55,100,195), 1, LINE_AA);
        }*/

        //累计概率霍夫变换
        vector<Vec4i> lines;
        HoughLinesP(mid, lines, 1, CV_PI/180, 80, 50, 10);
        for (size_t i = 0; i != lines.size(); ++i)
        {
            Vec4i l = lines[i];
            line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, LINE_AA);
        }

        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnGlobalSeg()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() != 1)
            cvtColor(src, dst, COLOR_BGR2GRAY);
        else
            dst = src;
        threshold(dst, dst, 128, 255, THRESH_BINARY);
        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}

void ImgProc::OnOtsu()
{
    if (!dstImg.isNull())
    {
        Mat src = QImage2cvMat(srcImg);
        Mat dst;
        if (src.channels() != 1)
            cvtColor(src, dst, COLOR_BGR2GRAY);
        else
            dst = src;

        //手动算法
        /*Mat image = dst;
        int T=0; //Otsu算法阈值;
        double varValue=0; //类间方差中间值保存  ;
        double w0=0; //前景像素点数所占比例  ;
        double w1=0; //背景像素点数所占比例  ;
        double u0=0; //前景平均灰度  ;
        double u1=0; //背景平均灰度  ;
        double Histogram[256]={0}; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  ;
        uchar *data=image.data;
        double totalNum=image.rows*image.cols; //像素总数  ;

        //计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数  ;

        for(int i=0;i<image.rows;i++)   //为表述清晰，并没有把rows和cols单独提出来  ;
        {
            for(int j=0;j<image.cols;j++)
            {
                Histogram[data[i*image.step+j]]++;
            }
        }

        for(int i=0;i<255;i++)
        {
            //每次遍历之前初始化各变量 ;
            w1=0;       u1=0;       w0=0;       u0=0;

            //***********背景各分量值计算**************************
            for(int j=0;j<=i;j++) //背景部分各值计算;
            {
                w1+=Histogram[j];  //背景部分像素点总数 ;
                u1+=j*Histogram[j]; //背景部分像素总灰度和  ;
            }

            if(w1==0) //背景部分像素点数为0时退出  ;
            {
                break;
            }

            u1=u1/w1; //背景像素平均灰度;
            w1=w1/totalNum; // 背景部分像素点数所占比例;
            //***********背景各分量值计算**************************

            //***********前景各分量值计算**************************
            for(int k=i+1;k<255;k++)
            {
                w0+=Histogram[k];  //前景部分像素点总数 ;
                u0+=k*Histogram[k]; //前景部分像素总灰度和  ;
            }
            if(w0==0) //前景部分像素点数为0时退出  ;
            {
                break;
            }
            u0=u0/w0; //前景像素平均灰度 ;
            w0=w0/totalNum; // 前景部分像素点数所占比例  ;
            //***********前景各分量值计算**************************

            //***********类间方差计算******************************
            double varValueI=w0*w1*(u1-u0)*(u1-u0); //当前类间方差计算  ;

            if(varValue<varValueI)
            {
                varValue=varValueI;
                T=i;
            }
        }
        threshold(dst, dst, T, 255, THRESH_BINARY);*/

        //opencv自带算法
        threshold(dst, dst, 0, 255, CV_THRESH_OTSU);
        dstImg = cvMat2QImage(dst);
        srcImg = dstImg;
        DisplayImg();
    }
    else
        QMessageBox::critical(this, QStringLiteral("错误提示"), QStringLiteral("请打开一张图片！"));
}
