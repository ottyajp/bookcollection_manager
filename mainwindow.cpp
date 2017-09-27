#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    ui->view->setUrl("https://www.amazon.co.jp/dp/product/"+isbn13to10(isbn13));
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
    ifstream errfs("./err.txt");
    string errstr;
    while(getline(errfs,errstr)){
        if(errstr.find("too quickly") != string::npos){
            ui->title->setText("Amazon API says 'too quickly'. please retry!");
            return;
        }
    }
    ifstream fs("./out.txt");
    QString title, author, image_url;
    string str;
    getline(fs, str); title = splitBracket(QString::fromStdString(str));
    getline(fs, str); author = splitBracket(QString::fromStdString(str));
    getline(fs, str); image_url = splitBracket(QString::fromStdString(str));
    ui->image->setUrl(QUrl(image_url));
    ui->sumbnail->setText(image_url);
    ui->title->setText(title);
    ui->author->setText(author);
}

QString MainWindow::splitBracket(QString str){
    return str.replace(0,2,"").replace(
                QRegularExpression("\"]$"),"");
}

void MainWindow::on_addButton_clicked()
{
    QString filename = ui->sumbnail->text().replace(
                QRegularExpression(".+/"),"");
    string com = "wget " + ui->sumbnail->text().toStdString() + " -O ./icon/" + filename.toStdString();
    system(com.c_str());
    auto *item = new QTreeWidgetItem(ui->tree);
    item->setText(1, ui->title->text());
    item->setText(2, ui->author->text());
    item->setText(0, ui->isbn->text());
    item->setIcon(0, QIcon("./icon/" + filename));
}

void MainWindow::on_getDetailAmazon_clicked()
{
    emit this->scrape();
}
