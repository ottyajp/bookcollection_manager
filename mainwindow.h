#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidgetItem>
#include <cstdlib>
#include <fstream>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QString isbn13to10(QString isbn13);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_action_Quit_triggered();

    void scrape_openbd();

    void on_addButton_clicked();

    void on_getDetailOpenBD_clicked();

    void fetchFinished();

signals:
    void scrape();

private:
    Ui::MainWindow *ui;
    QString splitBracket(QString str);
    QNetworkAccessManager *qnam;
    QNetworkReply *reply;
};

#endif // MAINWINDOW_H
