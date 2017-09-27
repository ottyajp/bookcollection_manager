#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QLabel>
#include <QTreeWidgetItem>
#include <cstdlib>
#include <fstream>
#include <QDebug>

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

    void scrape_amazon();

    void on_addButton_clicked();

signals:
    void scrape();

private:
    Ui::MainWindow *ui;
    QWebEngineView *view;
    QWebEngineView *image;
    QString splitBracket(QString str);
};

#endif // MAINWINDOW_H
