#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "qdebug.h"
#include "opencv2/videoio.hpp"
#include "camera_c.h"
#include "qmessagebox.h"
#include "QKeyEvent"
#include "QPainter"
#include "qfiledialog.h"
#include "qtimer.h"
#include "mask_c.h"
#include "diff_c.h"
#include "qtimer.h"
#include "qstringlist.h"
#include "QtGlobal"
#include "sharedcom.h"


//#define resolution 20
#define WIDTH 320
#define HEIGHT 240



using namespace cv;



mark::mark(QWidget* parent,int x,int y,int size):QLabel(parent),x(x),y(y),size(size)
{
    QRect rect(x,y,size,size);
    setGeometry(rect);
    setStyleSheet("border: 3px solid red");
}



void mark::hide2(void)
{
    hide();
    //QTimer::singleShot(5000,this,SLOT(hide()));
}

void MainWindow::updateDisplay(bool show)
{
    if(show)
    {
        lbl_imageDiff->show();
    }
    else
    {
        lbl_imageDiff->hide();
    }
}


int MainWindow::getExposition(void)
{
    return ui->spinExposition->value();
}

void MainWindow::saveParameters(void)
{
    QFile param( QCoreApplication::applicationDirPath() +"/"+"config.cfg");
    if (!param.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug()<<"config file not found";


    param.resize(0);

    QTextStream out(&param);

    out << "width="<<QString::number(ui->spinWidth->value())<<"\n";
    out << "height="<<QString::number(ui->spinHeight->value())<<"\n";
    out << "resolution="<<QString::number(ui->spinRes->value())<<"\n";
    out << "threshold_zone="<<QString::number(ui->spinThresholdZone->value())<<"\n";
    out << "threshold_sat="<<QString::number(ui->spinThreshold->value())<<"\n";
    out << "exposition="<<QString::number(ui->spinExposition->value())<<"\n";

    param.close();


    qDebug()<<"saved";

}

void MainWindow::loadParameters(void)
{
    QString line;
    QStringList fields;
    bool test;
    int value;
    QFile param( QCoreApplication::applicationDirPath() +"/"+"config.cfg");
    if (!param.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"config file not found";

    while(!param.atEnd()) {
        line = param.readLine();
        line.remove("\n");

        fields = line.split("=");

        if(fields.length()==2)
        {
            value = fields[1].toInt(&test);

            if((fields[0].contains("width"))&&(test))
                ui->spinWidth->setValue(value);
            else if(fields[0].contains("height"))
                ui->spinHeight->setValue(value);
            else if(fields[0].contains("resolution"))
            {
                // qDebug()<<value;
                ui->spinRes->setValue(value);
            }
            else if(fields[0].contains("threshold_zone"))
                ui->spinThresholdZone->setValue(value);
            else if(fields[0].contains("threshold_sat"))
                ui->spinThreshold->setValue(value);
            else if(fields[0].contains("exposition"))
                ui->spinExposition->setValue(value);
            else
                qDebug()<<"unknown parameter";

            test = false;
        }
    }

    param.close();


}

void MainWindow::getParams(void)
{
    resolution = ui->spinRes->value();
    if(resolution <= 0) resolution = 1;

    threshold = ui->spinThreshold->value();
    if(threshold <= 0) threshold = 1;


    thresholdZone = ui->spinThresholdZone->value();
    if(thresholdZone <= 0) thresholdZone = 1;
}

void MainWindow::init()
{

    getParams();

    lbl_imageSnap = new Mask(this,resolution);
    QRect rect2(40 , 20, WIDTH,HEIGHT);
    lbl_imageSnap->setGeometry(rect2);
    lbl_imageSnap->setStyleSheet("border: 2px solid black");
    lbl_imageSnap->show();


    connect(ui->bSave,SIGNAL(pressed()),lbl_imageSnap,SLOT(saveZones()));
    connect(ui->bLoad,SIGNAL(pressed()),lbl_imageSnap,SLOT(loadZones()));
    connect(this,SIGNAL(reload()),lbl_imageSnap,SLOT(loadZones()));
    connect(ui->bSelect,SIGNAL(pressed()),lbl_imageSnap,SLOT(selectAllZones()));
    connect(ui->bUnselect,SIGNAL(pressed()),lbl_imageSnap,SLOT(unselectAllZones()));

    lbl_imageDiff = new Diff_c(this);


    lbl_imageDiff->setStyleSheet("border: 2px solid black");
    lbl_imageDiff->show();

    int w = ui->spinWidth->value();
    int h = ui->spinHeight->value();


    cam = new camera_c(this,w,h,resolution,threshold,thresholdZone);
    connect(ui->radioEnabled,SIGNAL(clicked(bool)),cam,SLOT(enable(bool)));
    connect(ui->radioEnabled,SIGNAL(clicked(bool)),this,SLOT(clearDetected()));
    connect(cam,SIGNAL(sendZone(std::vector<std::vector<bool> >)),lbl_imageSnap,SLOT(selectZone(std::vector<std::vector<bool> >)));
    connect(cam,SIGNAL(sendSnap(QImage)),lbl_imageSnap,SLOT(setImg(QImage)));
    connect(cam,SIGNAL(sendSnap(QImage)),this,SLOT(clearDetected()));
    connect(cam,SIGNAL(sendDiff(QImage)),lbl_imageDiff,SLOT(setImg(QImage)));

    connect(ui->bSnap,SIGNAL(clicked(bool)),cam,SLOT(snap()));
    connect(com,SIGNAL(signalEnable(bool)),cam,SLOT(enable(bool)));
    connect(com,SIGNAL(signalEnable(bool)),this,SLOT(setEnabledRadio(bool)));
    connect(cam,SIGNAL(triggerSignal(int)),com,SLOT(triggerSlot(int)));

    camThread = new QThread;
    cam->moveToThread(camThread);
    camThread->start();

    cam->init();




    QRect rect3(60+WIDTH, 20, WIDTH,HEIGHT);
    lbl_imageDiff->setGeometry(rect3);
    setUpMarkers();
    lbl_imageSnap->redraw();

    emit testCom(msgPing);


}








void MainWindow::setUpMarkers(void)
{

    double dx = (double)WIDTH/resolution;
    double dy = (double)HEIGHT/resolution;

    markers.resize( resolution ,std::vector<mark*>( resolution , NULL ) );


    for (unsigned int xi = 0;xi<markers.size();xi++)
        for (unsigned int yi = 0;yi<markers[0].size();yi++)
        {
            markers[xi][yi] = new mark(this,dx/2 + 40+xi*dx,dy/2+20+yi*dy,5);
            markers[xi][yi]->hide();

        }

}

void MainWindow::restart()
{
    //clean chain destruction
    cam->shutdown();
    ui->radioEnabled->setChecked(false);
}

void MainWindow::reset(void)
{
    restart();

    //  QTimer::singleShot(4000,this,SIGNAL(reload()));
    cam->enabled = false;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    com = new sharedCom(this);
    connect(this,SIGNAL(testCom(char)),com,SLOT(sendMsg(char)));
    connect(com,SIGNAL(signalReset()),this,SLOT(reset()));
    com->start();

    connect(ui->bRestart,SIGNAL(pressed()),this,SLOT(restart()));
    connect(ui->bShutdown,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->bClearDetection,SIGNAL(pressed()),this,SLOT(clearDetected()));
    connect(ui->radioDisplay,SIGNAL(clicked(bool)),this,SLOT(updateDisplay(bool)));



    connect(ui->bAutoLearn,SIGNAL(pressed()),this,SLOT(autoLearn()));
    autoLearnStatus= false;

    imageFileName = QCoreApplication::applicationDirPath() +"/"+ "snap.png";

    loadParameters();
    prepZoneFile(QCoreApplication::arguments());

    init();

}

void MainWindow::autoLearn(void)
{
    autoLearnStatus= !autoLearnStatus;

    if(autoLearnStatus)
    {
        ui->bAutoLearn->setText("stop");
        cam->startLearning();
    }
    else
    {
        ui->bAutoLearn->setText("start");
        cam->stopLearning();

        for (unsigned int xi = 0;xi<markers.size();xi++)
            for (unsigned int yi = 0;yi<markers[0].size();yi++)
            {
                markers[xi][yi]->hide();
            }
    }
}

void MainWindow::prepZoneFile(QStringList cmdline_args)
{
    int nures;
    QString line;
    QStringList fields;

    if(cmdline_args.size()>1)
    {
        QStringList buf;
        buf = cmdline_args[1].split(".");
        if(buf.size()==2)
        {
            if(buf[1] == "txt")
                zoneFileName = cmdline_args[1];
        }
    }
    /*else
        zoneFileName = "default.txt";*/

    zoneFileName = QCoreApplication::applicationDirPath() +"/"+ zoneFileName;





    // zoneFileName = "/home/pi/qt5/test/zones42.txt";
    QFile file(zoneFileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"cannot open zones file";
        return;
    }

    line = file.readLine();
    fields = line.split("=");
    if(fields.length()==2)
    {
        if(fields[0]=="res")
        {
            nures = fields[1].toInt();
            if((nures<=0)||(nures>50))
            {
                qDebug()<<"invalid resolution: "<<nures;
                return;
            }
            resolution = nures;
            ui->spinRes->setValue(resolution);
        }
    }
    else
    {
        qDebug()<<"cannot read resolution";
        return;
    }
    file.close();
}

void MainWindow::setMarkerVisible(unsigned int x,unsigned int y,bool status)
{
    if(x>=markers.size()) return;
    if(y>=markers[0].size()) return;

    if(status)
        markers[x][y]->show();
    else
        markers[x][y]->hide2();
}

void MainWindow::clearDetected(void)
{

    for (unsigned int xi = 0;xi<markers.size();xi++)
        for (unsigned int yi = 0;yi<markers[0].size();yi++)
            markers[xi][yi]->hide();


}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    emit snap();
}

void MainWindow::setEnabledRadio(bool status)
{
    ui->radioEnabled->setChecked(status);

}


MainWindow::~MainWindow()
{
    saveParameters();
    delete ui;
}
