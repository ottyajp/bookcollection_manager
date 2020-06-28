#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QSerialPort>
#include <QSettings>

namespace Ui {
class config;
}

class config : public QDialog
{
    Q_OBJECT

public:
    explicit config(QWidget *parent = nullptr);
    ~config();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::config *ui;
    QSettings *set;
};

#endif // CONFIG_H
