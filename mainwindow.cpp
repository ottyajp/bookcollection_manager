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

    // DB初期化
    connectionName = "db";
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(set->value("dbFilePath").toString());
    if (!db.open()) {
        qDebug()<<db.lastError();
    }
    // テーブル作成
    QSqlQuery query(db);
    if (query.prepare("create table data("
                      "  id integer primary key autoincrement"
                      ", isbn text"
                      ", title text"
                      ", author text"
                      ", cover blob"
                      ")")) {
        if (!query.exec()) {
            qDebug()<<query.lastError();
            qDebug()<<query.lastQuery()<<query.boundValues();
        }
    } else {
        qDebug()<<query.lastError();
    }
    loadItems();
}

MainWindow::~MainWindow()
{
    db.close();
    QSqlDatabase::removeDatabase(connectionName);
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
    ui->image->setPixmap(*coverPixMap);
}

void MainWindow::on_addButton_clicked()
{
    auto *item = new QTreeWidgetItem(ui->tree);
    item->setText(1, ui->title->text());
    item->setText(2, ui->author->text());
    item->setText(0, ui->isbn->text());
    item->setIcon(0, QIcon(ui->image->pixmap(Qt::ReturnByValue)));
    item->setTextAlignment(0, Qt::AlignRight);

    QSqlQuery query(db);
    if (query.prepare("insert into data (isbn, title, author, cover)"
                      " values(?, ?, ?, ?)")) {
        query.addBindValue(ui->isbn->text());
        query.addBindValue(ui->title->text());
        query.addBindValue(ui->author->text());
        QByteArray bArray;
        QBuffer buffer(&bArray);
        buffer.open(QIODevice::WriteOnly);
        ui->image->pixmap(Qt::ReturnByValue).save(&buffer, "PNG");
        query.addBindValue(bArray);
        if (query.exec()) {
            qDebug()<<query.lastInsertId().toULongLong() << "added";
            ui->registeredLabel->setText(tr("registered"));
        } else {
            qDebug()<<query.lastError();
            qDebug()<<query.lastQuery()<<query.boundValues();
        }
    } else {
        qDebug()<<query.lastError();
    }
}

void MainWindow::loadItems()
{
    QSqlQuery query(db);
    if (query.prepare("select id, isbn, title, author, cover"
                      " from data")) {
        if (query.exec()) {
            while (query.next()) {
                auto *item = new QTreeWidgetItem(ui->tree);
                item->setText(1, query.value("title").toString());
                item->setText(2, query.value("author").toString());
                item->setText(0, query.value("isbn").toString());
                item->setTextAlignment(0, Qt::AlignRight);
                QPixmap pixmap;
                pixmap.loadFromData(query.value("cover").toByteArray());
                item->setIcon(0, QIcon(pixmap));
            }
        }
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
    QSqlQuery query(db);
    if (query.prepare("select cover"
                      " from data where isbn=?")) {
        query.addBindValue(item->text(0));
        if (query.exec()) {
            while (query.next()) {
                if (query.value("cover").toByteArray().size() != 0) {
                    pixmap->loadFromData(query.value("cover").toByteArray());
                } else {
                    QMessageBox box;
                    box.setText(tr("no cover image saved."));
                    box.setStandardButtons(QMessageBox::Ok);
                    box.exec();
                    return;
                }
            }
        }
    }
    coverViewer *cv = new class coverViewer(this);
    cv->setWindowModality(Qt::ApplicationModal);
    cv->setPixmap(pixmap);
    cv->show();
}
