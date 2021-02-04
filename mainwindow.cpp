#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->device = new QSerialPort(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonSearch_clicked()
{
    qDebug() << "Search devices...";
    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();
    //If list not empty
    if((devices.count() - 1) >= 0)
        for(int i = 0; i < devices.count(); i++)
        {
            qDebug() << devices.at(i).portName() << devices.at(i).description();
            this->addToLogs("Found devices: " + devices.at(i).portName() + " " + devices.at(i).description());
            ui->comboBoxDevices->addItem(devices.at(i).portName() + " " + devices.at(i).description());
        }
}

void MainWindow::addToLogs(QString message)
{
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    ui->textEditLogs->append(currentDateTime + "\t" + message);
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if(ui->comboBoxDevices->count() == 0)
    {
        this->addToLogs("Don't find any devices!");
        return;
    }

    QString portName = ui->comboBoxDevices->currentText().split(" ").first();
    this->device->setPortName(portName);

    //OPEN AND CONFIGURE PORT
    if(!device->isOpen())
    {
        if(device->open(QSerialPort::ReadWrite))
        {
            this->device->setBaudRate(QSerialPort::Baud9600);
            this->device->setDataBits(QSerialPort::Data8);
            this->device->setParity(QSerialPort::NoParity);
            this->device->setStopBits(QSerialPort::OneStop);
            this->device->setFlowControl(QSerialPort::NoFlowControl);

            this->addToLogs("Open serial port");

            //CONNECT
            connect(this->device, SIGNAL(readyRead()), this, SLOT(readFromPort()));
        }
        else
        {
            this->addToLogs("Opening serial ports is fail!");
            qDebug() << device->error();
        }
    }
    else
    {
        this->addToLogs("Port is open!");
        return;
    }
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    if(this->device->isOpen())
    {
        this->device->close();
        this->addToLogs("Closed connection.");
    }
    else
    {
        this->addToLogs("Port is not open!");
    }
}

void MainWindow::readFromPort()
{
    while(this->device->canReadLine())
    {
        QString line = this->device->readLine();
        //qDebug() << line;

        QString terminator = "\r";
        int pos = line.lastIndexOf(terminator);
        //qDebug() << line.left(pos);

        this->addToLogs(line.left(pos));
    }
}
