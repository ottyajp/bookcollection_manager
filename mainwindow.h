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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QBuffer>

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
    void loadItems();

private slots:
    void on_pushButton_clicked();

    void on_action_Quit_triggered();

    void scrape_openbd();

    void on_addButton_clicked();

    void on_getDetailOpenBD_clicked();

    void fetchFinished();

    void coverFetchFinished();

signals:
    void scrape();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *qnam;
    QNetworkReply *detailReply;
    QNetworkReply *coverReply;
    QPixmap *coverPixMap;
    QString connectionName;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
