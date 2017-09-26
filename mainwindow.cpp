#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    view = new QWebEngineView(this);
    ui->layout->addWidget(view);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    std::system("timeout 2 cat /dev/ttyACM0 > out.txt");
    std::ifstream fs("./out.txt");
    std::string str;
    std::getline(fs, str);
    ui->isbn->setText(QString::fromStdString(str));
    QString isbn13 = QString::fromStdString(str).replace(QRegularExpression("\r"),"");
    this->view->setUrl("https://www.amazon.co.jp/dp/product/"+isbn13to10(isbn13));
    qDebug()<<view->url();
    fs.close();
}

QString isbn13to10(QString isbn13){
    QString isbn10 = isbn13.replace(0,3,"");
    int checkdigit = 0;
    for(int i=10; i>1; i--){
        checkdigit += i * isbn10.at(10 - i).digitValue();
    }
    checkdigit = 11 - checkdigit%11;
    isbn10.replace(9,1,QString::number(checkdigit));
    qDebug()<<isbn10;
    return isbn10;
}

void MainWindow::on_action_Quit_triggered()
{
    this->close();
}
