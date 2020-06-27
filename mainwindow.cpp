#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(scrape()), this, SLOT(scrape_openbd()));
    ui->image->setAlignment(Qt::AlignCenter);
    qnam = new QNetworkAccessManager();
    // メイドインアビス1
    //ui->isbn->setText("9784812483800");
    // 「集合と位相」をなぜ学ぶのか
    //ui->isbn->setText("9784774196121");
    //ui->view->setUrl("https://api.openbd.jp/v1/get?isbn="+ui->isbn->text());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QSerialPort port;
    QString portName = "/dev/ttyACM0";
    port.setPortName(portName);
    int baudRate = QSerialPort::Baud9600;
    port.setBaudRate(baudRate);
    if (!port.open(QIODevice::ReadOnly)) {
        qDebug()<<QObject::tr("Failed to open port %1, error: %2")
               .arg(portName).arg(port.error());
    }
    QByteArray readData = port.readAll();
    while (port.waitForReadyRead(1000)) {
        readData.append(port.readAll());
    }
    if(port.error() == QSerialPort::ReadError) {
        qDebug()<<QObject::tr("Failed to read from port %1, error: %2")
                  .arg(portName).arg(port.errorString());
    } else if (port.error() == QSerialPort::TimeoutError && readData.isEmpty()) {
        qDebug()<<QObject::tr("No data was currently available for readin from port %1")
                  .arg(portName);
    }

    QString isbn13 = QString(readData).replace(QRegularExpression("\r"), "");
    if(isbn13.length() != 13){
        ui->isbn->setText("error");
        return;
    }
    ui->isbn->setText(isbn13);
    ui->view->setUrl("https://api.openbd.jp/v1/get?isbn=" + isbn13);
    emit this->scrape();
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

void MainWindow::scrape_openbd(){
    QUrl url = ui->view->url();
    QNetworkRequest req(ui->view->url());
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    detailReply = qnam->get(req);
    connect(detailReply, SIGNAL(finished()), this, SLOT(fetchFinished()));
}

void MainWindow::fetchFinished() {
    QJsonDocument json(QJsonDocument::fromJson(detailReply->readAll()));
    detailReply->deleteLater();
    QJsonObject baseObject = json.array().at(0).toObject().value("onix").toObject();
    QJsonObject descriptiveDetail = baseObject.value("DescriptiveDetail").toObject();

    QString title = descriptiveDetail.value("TitleDetail").toObject()
            .value("TitleElement").toObject()
            .value("TitleText").toObject()
            .value("content").toString();
    QString author = descriptiveDetail.value("Contributor").toArray().at(0).toObject()
            .value("PersonName").toObject()
            .value("content").toString();
    QString cover = baseObject.value("CollateralDetail").toObject()
            .value("SupportingResource").toArray().at(0).toObject()
            .value("ResourceVersion").toArray().at(0).toObject()
            .value("ResourceLink").toString();

    QNetworkRequest req(cover);
    coverReply = qnam->get(req);
    connect(coverReply, SIGNAL(finished()), this, SLOT(coverFetchFinished()));

    ui->thumbnail->setText(cover);
    ui->title->setText(title);
    ui->author->setText(author);
}

void MainWindow::coverFetchFinished()
{
    coverPixMap = new QPixmap;
    coverPixMap->loadFromData(coverReply->readAll());
    ui->image->setPixmap(*coverPixMap);
}

void MainWindow::on_addButton_clicked()
{
    auto *item = new QTreeWidgetItem(ui->tree);
    item->setText(1, ui->title->text());
    item->setText(2, ui->author->text());
    item->setText(0, ui->isbn->text());
    item->setIcon(0, QIcon(ui->image->pixmap(Qt::ReturnByValue)));
}

void MainWindow::on_getDetailOpenBD_clicked()
{
    emit this->scrape();
}
