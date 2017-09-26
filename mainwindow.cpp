#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>

using namespace std;

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
    system("timeout 2 cat /dev/ttyACM0 > out.txt");
    ifstream fs("./out.txt");
    string str;
    getline(fs, str);
    QString isbn13 = QString::fromStdString(str).replace(QRegularExpression("\r"),"");
    ui->isbn->setText(isbn13);
    this->view->setUrl("https://www.amazon.co.jp/dp/product/"+isbn13to10(isbn13));
    connect(this->view, SIGNAL(loadFinished(bool)), this, SLOT(scrape_amazon()));
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
    return isbn10;
}

void MainWindow::on_action_Quit_triggered()
{
    this->close();
}

void MainWindow::scrape_amazon(){
    string com = "ruby ./paapi.rb " + isbn13to10(ui->isbn->text()).toStdString() + " >out.txt 2>err.txt";
    qDebug()<<QString::fromStdString(com);
    system(com.c_str());
    ifstream fs("./out.txt");
    QString title, author;
    string str;
    getline(fs, str); title = QString::fromStdString(str);
    getline(fs, str); author = QString::fromStdString(str);
    ui->title->setText(title);
    ui->author->setText(author);
}
