#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QThread>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息

class serialPort : public QThread
{
public:
    explicit serialPort(QString port);
    ~serialPort();

    int openSerialPort(QString port);
    void closeSerialPort();
private:
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);
    QSerialPort *serial;

protected:
    void run();
    QString com;
};

#endif // SERIALPORT_H
