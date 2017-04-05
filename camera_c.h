#ifndef CAMERA_C_H
#define CAMERA_C_H

#include "qthread.h"
#include "qobject.h"
#include "qlabel.h"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "qdebug.h"
#include "opencv2/videoio.hpp"
#include "mainwindow.h"
#include "qmutex.h"
#include "QTimer"


class Mask;


using namespace cv;

class camera_c;


class camera_c:public QObject
{
    Q_OBJECT
public:

    camera_c(QObject *parent, int width, int heigth, int resolution, int threshold, int thresholdZone);
    ~camera_c(void);
    QObject *parent;

    VideoCapture *capture;

    void init(void);

    Mat imageSnap;
    Mat imageDiff;
    Mat image;
    cv::Mat buf;

    QImage qImageSnap;
    QImage qImageDiff;
    QPixmap pixmapImageDiff;
    QMutex computing;
    void shutdown(void);
    double dx;
    double dy;
    bool enabled;
    void startLearning(void);
    void stopLearning(void);
    bool isLearning;
    std::vector<std::vector<bool> > detectedZone;


private:
    int width;
    int height;
    const int resolution;
    int threshold;
    int thresholdZone;
    void config(void);
    int getZoneValue(int X,int Y);
    void checkZones(void);
    QTimer *updTimer;

signals:
    void dataReady(int);
    void triggerSignal(int);
    void setSize(int,int);
    void startProcess();
    void sendZone(std::vector<std::vector<bool> >);
    void setMarkerVisible(uint,uint,bool);
    void sendDiff(QImage);
    void sendSnap(QImage);

private slots:
    void update(void);
    void snap(void)    ;
    void enable(bool);

};
#endif // CAMERA_C_H
