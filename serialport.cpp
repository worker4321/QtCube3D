#include "serialport.h"
#include <QDebug>

serialPort::serialPort(QString port):com(port)
{
    serial = new QSerialPort(this);
    if(serial == NULL){
        qDebug()<<"new QserialPort failed"<<endl;
    }

}
int serialPort::openSerialPort(QString port)
{
    //QSerialPortInfo portToUse;


//    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
//    {
//        QString s = QObject::tr("Port:") + info.portName() + "\n"
//                    + QObject::tr("Location:") + info.systemLocation() + "\n"
//                    + QObject::tr("Description:") + info.description() + "\n"
//                    + QObject::tr("Manufacturer:") + info.manufacturer() + "\n"
//                    + QObject::tr("Serial number:") + info.serialNumber() + "\n"
//                    + QObject::tr("Vendor Identifier:") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
//                    + QObject::tr("Product Identifier:") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
//                    + QObject::tr("Busy:") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

//        if(!info.isBusy() && (info.description().contains("Arduino") || info.manufacturer().contains("Arduino")))
//            portToUse = info;
//        qDebug() << s;
//    }

//    if(portToUse.isNull() || !portToUse.isValid())
//    {
//        qDebug() << "port is not valid:" << portToUse.portName();
//        return;
//    }

    // Open it if it isn't busy
    if(com.length() < 1){
        qDebug() << "port is not valid:"<<com;
        return -1;
    }
    serial->setPortName(com);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->setReadBufferSize(100);
    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Connected to" << com;
    } else {
        qCritical()<<com << " :Serial Port error:" << serial->errorString();
        qDebug() << tr("Open error");
    }
}
void serialPort::run()
{
    qDebug()<<"serial open com:"<<com<<endl;
    openSerialPort(com);
    while(true){
        readData();
        msleep(10);
        qDebug()<<"serial runing";
        //exec();
    }
}
void serialPort::closeSerialPort()
{
    serial->close();
    qDebug() << tr("closed");
}
void serialPort::writeData(const QByteArray &data)
{
    serial->write(data);
}



void serialPort::readData()
{
    char tx[]="abcd";
    QByteArray data = serial->readLine(10);
    //console->putData(data);
    qDebug() << "UART:" << data;
    serial->write(tx,4);
}



void serialPort::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        // QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        qCritical() << "Serial Port error:" << serial->errorString();
        closeSerialPort();
    }
}
serialPort::~serialPort()
{
    closeSerialPort();
}
