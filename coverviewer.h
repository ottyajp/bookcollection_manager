#ifndef COVERVIEWER_H
#define COVERVIEWER_H

#include <QDialog>
#include <QPixmap>

namespace Ui {
class coverViewer;
}

class coverViewer : public QDialog
{
    Q_OBJECT

public:
    explicit coverViewer(QWidget *parent = nullptr);
    ~coverViewer();
    void setPixmap(QPixmap *pixmap = nullptr);

private:
    Ui::coverViewer *ui;
};

#endif // COVERVIEWER_H
