#ifndef MASK_C_H
#define MASK_C_H

#include "qlabel.h"
#include "qthread.h"






class Mask:public QLabel
{
    Q_OBJECT
public:
    int resolution;
    explicit Mask(QWidget* parent=0, int resolution=10 );
    ~Mask(void);

    void redraw(void);
    void resetDetection(void);
    void markUndetected(int x,int y);
    void markDetected(int x,int y);
    int click_x,click_y;
    double dx,dy;


    std::vector<std::vector<bool> > markedZone;
  //  std::vector<std::vector<bool> > detectedZone;
    bool isZoneMarked(int x, int y);
    QImage image;

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
signals:

    void setRes(int);

public slots:
    void setImg(QImage);

private slots:
    void selectAllZones();
    void unselectAllZones();
    void loadZones();
    void saveZones();
    void selectZone(std::vector<std::vector<bool> >);
};


#endif // MASK_C_H
