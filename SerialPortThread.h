#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialPortThread:public QThread
{
    Q_OBJECT
public:
    SerialPortThread();//构造函数
    ~SerialPortThread();//析构函数
    void setIsEnd(bool b);//设置线程是否停止标志
    bool getIsEnd();//获取线程是否停止的标志
    void setSerialPortNumber(const QString & portNum);//设置串口号
    void setSerialPortBaudRate(int r);//设置波特率
    void setSerialPortParity(QSerialPort::Parity p);//设置校验位
    void setSerialPortDataBits(QSerialPort::DataBits d);//设置数据位
    void setSerialPortStopBits(QSerialPort::StopBits s);//设置停止位

    void setSendFlag(bool b);//设置发送信息的标志
    bool getSendFlag();//获取发送信息的标志
    void setSendData(QByteArray send);//设置发送数据
    bool getSendData();//获取发送数据
signals:
    void serialPortOpenStatus(int status);//发送串口打开状态的信号
    void serialPortReceivedData(QByteArray receivedData);//发送串口接收到的数据的信号
protected:
    void run();//线程run函数
private:
    bool isEnd;//线程停止标志
    QByteArray receivedData;//接收的数据
    QByteArray sendData;//发送的数据
    bool sendFlag;//发送标志
    QString serialPortNumber;//串口号
    int baudRate;//波特率
    QSerialPort::Parity parity;//校验位
    QSerialPort::DataBits dataBits;//数据位
    QSerialPort::StopBits stopBits;//停止位
};

#endif // SERIALPORTTHREAD_H
