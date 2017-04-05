#include "diff_c.h"
#include "qimage.h"
#include "qdebug.h"
#include "qpainter.h"
#include "mainwindow.h"




Diff_c::Diff_c(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
}



void Diff_c::setImg(QImage img)
{

    QImage img2 = img.scaled(width(),height(),Qt::KeepAspectRatio);
    image = img2.copy();
    QPixmap pix = QPixmap::fromImage(image);

    setPixmap(pix);
}


Diff_c::~Diff_c()
{
    qDebug()<<"diff destroyed";


}
