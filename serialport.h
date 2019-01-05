#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <queue>
#include <QThread>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息

//head cmd  x                             CRC
//0x55 0x51 AxL AxH AyL AyH AzL AzH TL TH SUM
//0x55 0x54 HxL HxH HyL HyH HzL HzH TL TH SUM
#define  FRAME_HEAD          0x55
#define  FRAME_CMD_ACC       0x51
#define  FRAME_CMD_MAG       0x54
#define  FRAME_CMD_QUATER    0x59
#define  FRAME_LENGTH        11
#define  FRAME_OFFSET_CMD    1
class serialPort : public QThread
{
public:
    explicit serialPort(QString port);
    ~serialPort();

    int openSerialPort(QString port);
    void closeSerialPort();
    void setBusy(bool val);
    bool getBusy(void);
    QByteArray RcvBuf;
private:
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);
    QSerialPort *serial;

protected:
    void run();
    QString com;
    bool busy;
};

#endif // SERIALPORT_H
