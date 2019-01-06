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
    return 1;
}
void serialPort::run()
{
    qDebug()<<"serial open com:"<<com<<endl;
    openSerialPort(com);
    while(true){
        readData();
        msleep(10);
        parseFrame();


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

// read data
void serialPort::readData()
{

    if (serial->waitForReadyRead(10) && this->busy == false) {
        this->busy = true;
        RcvBuf.append( serial->readAll());
        this->busy = false;
    }
}
void serialPort::parseFrame(void)
{
    //int i = 0;
    int j = 0;
    unsigned char Crc = 0;
    QString str;

    while(RcvBuf.length() >= FRAME_LENGTH){
        while((unsigned char)RcvBuf.at(0) == FRAME_HEAD ){
            str = RcvBuf.toHex();
            for(j=0; j<FRAME_LENGTH -1; j++){
                Crc += (unsigned char) RcvBuf.at(j);
            }
            //unsigned char tmp = RcvBuf.at(i-1+FRAME_OFFSET_CRC);
            if(Crc){
            //if((unsigned char)RcvBuf.at(FRAME_OFFSET_CRC) == Crc) {
                switch(RcvBuf.at(FRAME_OFFSET_CMD)){
                    case FRAME_CMD_QUATER:
                         Q4.setScalar( ((unsigned int)((unsigned int) (RcvBuf.at(3)<<8) \
                                          | (unsigned char)RcvBuf.at(2)) &0xffff)/32768);
                         Q4.setX( ( (unsigned int)((unsigned int) (RcvBuf.at(5)<<8) \
                                          | (unsigned char)RcvBuf.at(4)) &0xffff)/32768);

                         Q4.setY( ( (unsigned int)((unsigned int) (RcvBuf.at(7)<<8) \
                                          | (unsigned char)RcvBuf.at(6)) &0xffff)/32768);
                         Q4.setZ( (  (unsigned int)((unsigned int) (RcvBuf.at(9)<<8) \
                                          | (unsigned char)RcvBuf.at(8)) &0xffff)/32768);
                         qDebug()<<"enter quater"<<endl;
                         qDebug("Q1=0x%f,Q2=0x%f,Q3=0x%f,Q4=0x%f",Q4.scalar(),Q4.x(),Q4.y(),Q4.z());
                         break;
                    case FRAME_CMD_ACC:
                         qDebug()<<"enter acc"<<endl;
                         break;
                    case FRAME_CMD_MAG:
                         qDebug()<<"enter mag"<<endl;
                         break;
                    default: break;
                }
                int len = RcvBuf.length();
                if(len>=FRAME_LENGTH){
                  RcvBuf.remove(0,FRAME_LENGTH -1);
                }else{
                  Q_ASSERT("remove failed");
                }
                break;
            }else{
                //RcvBuf.remove(0,1);
                break;
            }

        }//while2
        RcvBuf.remove(0,1);
        Crc = 0;
      }//while1
}
void serialPort::getQuternion(void)
{
    if(this->busy == false){
       // RcvBuf.at(0);
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
