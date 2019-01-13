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
    serial->setBaudRate(QSerialPort::Baud9600);
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

    if (serial->waitForReadyRead() && this->busy == false) {
        this->busy = true;
        RcvBuf.append( serial->readAll());
        this->busy = false;
    }
}
float hex2Int(char high,char low)
{
  float val;
  short int tmp = (short) high<<8 | low ;
  if(tmp & 0x8000){ //补码判断
      tmp = ~tmp;
      tmp +=1;
      tmp = -tmp;
    }
   val = (float)tmp/32768;
   return val;
}
void serialPort::parseFrame(void)
{
    float tmp;
    uint8_t data[FRAME_LENGTH];
    int j = 0;
    unsigned char Crc = 0;
    QString str;

    while(RcvBuf.length() >= FRAME_LENGTH){
        while((unsigned char)RcvBuf.at(0) == FRAME_HEAD ){
            str = RcvBuf.toHex();
            for(j=0; j<FRAME_LENGTH -1; j++){
                Crc += (unsigned char) RcvBuf.at(j);
                data[j] = (unsigned char) RcvBuf.at(j);
            }
            //unsigned char tmp = RcvBuf.at(i-1+FRAME_OFFSET_CRC);
            if(Crc){
            //if((unsigned char)RcvBuf.at(FRAME_OFFSET_CRC) == Crc) {
                switch(RcvBuf.at(FRAME_OFFSET_CMD)){
                    case FRAME_CMD_QUATER:
                         //tmp = (float)(((short) data[3]<<8) | data[2]);
                         Q4.setScalar(hex2Int(data[3],data[2]));

                         //tmp = (( data[5]<<8) | data[4]);
                         //qDebug("tmp1=%f",tmp);
                          Q4.setX(hex2Int(data[5],data[4]));
                          Q4.setY(hex2Int(data[7],data[6]));
                          Q4.setZ(hex2Int(data[9],data[8]));


/*
                         tmp = ( float) (data[7]<<8 | data[6] &0xffff);
                         Q4.setY(tmp/32768);

                         tmp = ( float) (data[9]<<8 | data[8] &0xffff);
                         Q4.setZ(tmp/32768);
           */
                         qDebug()<<"enter quater"<<endl;
                         qDebug("Qw=%f,Qx=%f,Qy=%f,Qz=%f",Q4.scalar(),Q4.x(),Q4.y(),Q4.z());
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
