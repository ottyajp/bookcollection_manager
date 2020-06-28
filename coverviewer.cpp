#include "coverviewer.h"
#include "ui_coverviewer.h"

coverViewer::coverViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::coverViewer)
{
    ui->setupUi(this);
}

coverViewer::~coverViewer()
{
    delete ui;
}

void coverViewer::setPixmap(QPixmap *pixmap) {
    ui->cover->setPixmap(*pixmap);
}
