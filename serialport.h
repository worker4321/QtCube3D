 #ifndef SERIALPORT_H
#define SERIALPORT_H

#include <queue>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QQuaternion>

//head cmd  x                             CRC
//0x55 0x51 AxL AxH AyL AyH AzL AzH TL TH SUM
//0x55 0x54 HxL HxH HyL HyH HzL HzH TL TH SUM
#define  FRAME_HEAD          0x55
#define  FRAME_CMD_ACC       0x51
#define  FRAME_CMD_MAG       0x54
#define  FRAME_CMD_QUATER    0x59
#define  FRAME_LENGTH        11
#define  FRAME_OFFSET_CMD    1
#define  FRAME_OFFSET_CRC    10
class serialPort : public QThread
{
public:
    explicit serialPort(QString port);
    ~serialPort();

    QByteArray RcvBuf;
    QQuaternion Q4;
    int openSerialPort(QString port);
    void closeSerialPort();
    void setBusy(bool val);
    bool getBusy(void);
    void parseFrame(void);
    void getQuternion(void);

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
