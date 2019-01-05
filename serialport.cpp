#include "serialport.h"
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QDebug>

serialPort::serialPort(QString port):com(port)
{
    serial = new QSerialPort(this);
    if(serial == NULL){
        qDebug()<<"new QserialPort failed"<<endl;
    }
    this->busy = false;

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
        //qDebug()<<"serial runing";
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

void serialPort::setBusy(bool val)
{
    this->busy = val;
}
bool serialPort::getBusy(void)
{
    return this->busy;
}
void serialPort::readData()
{
    // read data
    if (serial->waitForReadyRead(10) && this->busy == false) {
        this->busy = true;
        RcvBuf.append( serial->readAll());
        while (serial->waitForReadyRead(10))
             RcvBuf.append( serial->readAll());// data += serial->readAll();

        qDebug() << "UART:" << RcvBuf.data();

        this->busy = false;
        //char tx[]="abcd";
        //serial->write(tx,4);
    }
}
val serialPort::parseFrame(void)
{
    int i = 0;
    int j = 0;
    int Crc = 0;

    while(RcvBuf.length() >= FRAME_LENGTH)
        while(RcvBuf.at(i++) == FRAME_HEAD ){
            for(j=0; j<(FRAME_HEAD-1); j++){
                Crc +=  RcvBuf.at(j);
            }
            if( RcvBuf.at(i+FRAME_HEAD-1) == (Crc & 0xff)){
                switch(RcvBuf.at(i+FRAME_OFFSET_CMD)){
                    case FRAME_CMD_QUATER:
                         qDebug()<<"enter cmd_quater"<<endl;
                         break;
                    case FRAME_CMD_ACC:
                         qDebug()<<"enter acc"<<endl;
                         break;
                    case FRAME_CMD_MAG:
                         qDebug()<<"enter mag"<<endl;
                         break;
                    default: break;
                }
                RcvBuf.remove(0,FRAME_LENGTH);
                break;
            }else{
                RcvBuf.remove(0,1);
                break;
            }
        }//while
}
val serialPort::getQuternion(void)
{
    if(this->busy == false){
        RcvBuf.at(0)
        //QQuaternion quate();
    }

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
