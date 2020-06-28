#include "config.h"
#include "ui_config.h"

config::config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::config)
{
    ui->setupUi(this);
    ui->baudRateCombo->addItem("1200", QSerialPort::Baud1200);
    ui->baudRateCombo->addItem("2400", QSerialPort::Baud2400);
    ui->baudRateCombo->addItem("4800", QSerialPort::Baud4800);
    ui->baudRateCombo->addItem("9600", QSerialPort::Baud9600);
    ui->baudRateCombo->addItem("19200", QSerialPort::Baud19200);
    ui->baudRateCombo->addItem("38400", QSerialPort::Baud38400);
    ui->baudRateCombo->addItem("57600", QSerialPort::Baud57600);
    ui->baudRateCombo->addItem("115200", QSerialPort::Baud115200);

    set = new QSettings("settings.ini", QSettings::IniFormat);
    if (set->value("baudRate").toString().length() == 0) {
        // device port
        ui->devicePort->setText("/dev/ttyACM0");
        // baud rate
        int index = ui->baudRateCombo->findData(QSerialPort::Baud9600);
        ui->baudRateCombo->setCurrentIndex(index);
        // db file path
        ui->dbFilePath->setText("./data.sqlite3");
    } else {
        // device port
        ui->devicePort->setText(set->value("devicePort").toString());
        // baud rate
        int index = ui->baudRateCombo->findData(set->value("baudRate"));
        ui->baudRateCombo->setCurrentIndex(index);
        // db file path
        ui->dbFilePath->setText(set->value("dbFilePath").toString());
    }
}

config::~config()
{
    delete ui;
}

void config::on_buttonBox_accepted()
{
    set->setValue("devicePort", ui->devicePort->text());
    set->setValue("baudRate", ui->baudRateCombo->currentData());
    set->setValue("dbFilePath", ui->dbFilePath->text());
    this->close();
}

void config::on_buttonBox_rejected()
{
    this->close();
}

void config::on_browseButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(
                this,
                tr("Choose DB file path"),
                "~/",
                tr("sqlite3 file (*.sqlite3)"));
    if (path.size() != 0) {
        ui->dbFilePath->setText(path);
    }
}
