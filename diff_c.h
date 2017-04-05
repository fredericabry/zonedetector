#ifndef SNAP_H
#define SNAP_H
#include "qlabel.h"
#include "qthread.h"

class Diff_c:public QLabel
{
    Q_OBJECT
public:

    explicit Diff_c(QWidget* parent=0);
    ~Diff_c(void);

private:
    QImage image;

public slots:
    void setImg(QImage img);
};







#endif // SNAP_H
