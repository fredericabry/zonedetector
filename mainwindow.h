#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qlabel.h"
#include "qthread.h"



class mask_c;
class Diff_c;
class camera_c;
class sharedCom;

class Mask;

namespace Ui {
class MainWindow;
}



class mark:public QLabel
{
    Q_OBJECT
public:
    explicit mark(QWidget* parent=0,int x=0,int y=0,int size=1);
    int x;
    int y;
    int size;
public slots:
    void hide2(void);

};













class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QLabel *getLabel(void);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* e);
    Diff_c *lbl_imageDiff;
    Mask *lbl_imageSnap;
    std::vector<std::vector<mark*> > markers;
    int resolution,threshold,thresholdZone,exposition;
    QString zoneFileName;
    void setUpMarkers(void);
    void clearMarkers(void);
    QString imageFileName;
    int getExposition(void);


private:
    Ui::MainWindow *ui;
    camera_c *cam;
    QThread *camThread;
    sharedCom *com;
    void saveParameters();
    void loadParameters();
    void prepZoneFile(QStringList cmdline_args);
    void getParams(void);
    bool autoLearnStatus;


private slots:
    void setMarkerVisible(unsigned int x,unsigned int y,bool status);
    void clearDetected(void);
    void restart(void);
    void init(void);
    void updateDisplay(bool show);
    void reset(void);
    void autoLearn(void);
    void setEnabledRadio(bool status);

signals:
    void snap(void)    ;
    void testCom(char);
    void reload(void);

};





#endif // MAINWINDOW_H
