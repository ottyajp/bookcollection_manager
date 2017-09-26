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
    image = new QWebEngineView(this);
    ui->detailLayout->addWidget(image);
    connect(this, SIGNAL(scrape()), this, SLOT(scrape_amazon()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    system("timeout 1 cat /dev/ttyACM0 > out.txt");
    ifstream fs("./out.txt");
    string str;
    getline(fs, str);
    QString isbn13 = QString::fromStdString(str).replace(QRegularExpression("\r"),"");
    if(isbn13.length() != 13){
        ui->isbn->setText("error");
        return;
    }
    ui->isbn->setText(isbn13);
    emit this->scrape();
    this->view->setUrl("https://www.amazon.co.jp/dp/product/"+isbn13to10(isbn13));
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
    system(com.c_str());
    ifstream fs("./out.txt");
    QString title, author, image_url;
    string str;
    getline(fs, str); title = splitBracket(QString::fromStdString(str));
    getline(fs, str); author = splitBracket(QString::fromStdString(str));
    getline(fs, str); image_url = splitBracket(QString::fromStdString(str));
    qDebug()<<image_url;
    image->setUrl(image_url);
    ui->title->setText(title);
    ui->author->setText(author);
}

QString MainWindow::splitBracket(QString str){
    return str.replace(0,2,"").replace(
                QRegularExpression("\"]$"),"");
}
