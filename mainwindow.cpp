#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set = new QSettings("settings.ini", QSettings::IniFormat);
    connect(this, SIGNAL(scrape()), this, SLOT(scrape_openbd()));
    ui->image->setAlignment(Qt::AlignCenter);
    qnam = new QNetworkAccessManager();
    ui->tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tree, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(openCoverViewer(QPoint)));
    ui->tree->setColumnWidth(0, 140);
    ui->tree->setColumnWidth(1, 500);
    // メイドインアビス1
    //ui->isbn->setText("9784812483800");
    // 「集合と位相」をなぜ学ぶのか
    //ui->isbn->setText("9784774196121");
    //ui->view->setUrl("https://api.openbd.jp/v1/get?isbn="+ui->isbn->text());

    db = new class Db(set->value("dbFilePath").toString());
    loadItems();
}

MainWindow::~MainWindow()
{
    delete db;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QSerialPort port;
    QString portName = set->value("devicePort").toString();
    port.setPortName(portName);
    int baudRate = set->value("baudRate").toInt();
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
    ui->registeredLabel->setText(tr("unregistered"));
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

    if (cover.length() == 0) {
        ui->image->setText("No Image.");
        ui->thumbnail->setText(tr("no image url provided."));
    } else {
        QNetworkRequest req(cover);
        coverReply = qnam->get(req);
        connect(coverReply, SIGNAL(finished()), this, SLOT(coverFetchFinished()));
        ui->thumbnail->setText(cover);
    }

    ui->title->setText(title);
    ui->author->setText(author);
}

void MainWindow::coverFetchFinished()
{
    coverPixMap = new QPixmap;
    coverPixMap->loadFromData(coverReply->readAll());
    coverReply->deleteLater();
    ui->image->setPixmap(*coverPixMap);
}

void MainWindow::on_addButton_clicked()
{
    Entity e(ui->isbn->text(),
             ui->title->text(),
             ui->author->text(),
             this->uiImageToByteArray());
    int insertRet = db->insert(e);
    if (insertRet == 0) {
        // treeへの追加
        auto *item = new QTreeWidgetItem(ui->tree);
        item->setText(0, e.getIsbn());
        QPixmap pixmap;
        pixmap.loadFromData(e.getCover());
        item->setIcon(0, QIcon(pixmap));
        item->setText(1, e.getTitle());
        item->setText(2, e.getAuthor());
    } else if (insertRet == 19) {
        QMessageBox box;
        box.setText(tr("The ISBN code you attempted is already registered.\n"
                       "Overwrite it?"));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = box.exec();
        if (ret == QMessageBox::Yes) {
            int updateRet = db->update(e);
            if (updateRet == 0) {
                // treeの再読込
                QTreeWidgetItemIterator it(ui->tree);
                while (*it) {
                    delete *it;
                    ++it;
                }
                ui->tree->clear();
                loadItems();
            }
        }
        return;
    }
}

QByteArray MainWindow::uiImageToByteArray()
{
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    ui->image->pixmap(Qt::ReturnByValue).save(&buffer, "PNG");
    return bArray;
}

void MainWindow::loadItems()
{
    auto list = db->selectAll();
    for (auto it = list.begin(); it != list.end(); ++it) {
        auto *item = new QTreeWidgetItem(ui->tree);
        item->setText(0, (*it).getIsbn());
        QPixmap pixmap;
        pixmap.loadFromData((*it).getCover());
        item->setIcon(0, QIcon(pixmap));
        item->setText(1, (*it).getTitle());
        item->setText(2, (*it).getAuthor());
    }
}

void MainWindow::on_getDetailOpenBD_clicked()
{
    emit this->scrape();
}

void MainWindow::on_tree_itemActivated(QTreeWidgetItem *item)
{
    ui->isbn->setText(item->text(0));
    ui->title->setText(item->text(1));
    ui->author->setText(item->text(2));
    QIcon icon = item->icon(0);
    if (!icon.isNull()) {
        ui->image->setPixmap(icon.pixmap(QSize(300, 600)));
    } else {
        ui->image->setText("No Image.");
    }
    ui->registeredLabel->setText(tr("registered"));
}

void MainWindow::on_action_Config_triggered()
{
    config *config = new class config(this);
    config->setWindowModality(Qt::ApplicationModal);
    config->show();
}

void MainWindow::openCoverViewer(QPoint pos)
{
    QTreeWidgetItem *item = ui->tree->itemAt(pos);
    QPixmap *pixmap = new QPixmap();
    QByteArray ba = db->getCover(item->text(0));
    if (ba.size() != 0) {
        pixmap->loadFromData(ba);
    } else {
        QMessageBox box;
        box.setText(tr("no cover image saved."));
        box.setStandardButtons(QMessageBox::Ok);
        box.exec();
        return;
    }

    coverViewer *cv = new class coverViewer(this);
    cv->setWindowModality(Qt::ApplicationModal);
    cv->setPixmap(pixmap);
    cv->show();
}

void MainWindow::on_thumbnail_editingFinished()
{
    QNetworkRequest req(ui->thumbnail->text());
    coverReply = qnam->get(req);
    connect(coverReply, SIGNAL(finished()), this, SLOT(coverFetchFinished()));
}
