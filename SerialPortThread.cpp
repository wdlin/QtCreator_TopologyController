#include "SerialPortThread.h"
#include <iostream>
SerialPortThread::SerialPortThread():isEnd(false),serialPortNumber("COM1"),
    baudRate(9600),parity(QSerialPort::NoParity),dataBits(QSerialPort::Data8),
    stopBits(QSerialPort::OneStop)
{

}

SerialPortThread::~SerialPortThread()
{

}
//设置串口号
void SerialPortThread::setSerialPortNumber(const QString & portNum)
{
    serialPortNumber = portNum;
}
//设置波特率
void SerialPortThread::setSerialPortBaudRate(int r)
{
    baudRate = r;
}
//设置校验位
void SerialPortThread::setSerialPortParity(QSerialPort::Parity p)
{
    parity = p;
}
//设置数据位
void SerialPortThread::setSerialPortDataBits(QSerialPort::DataBits d)
{
    dataBits = d;
}
//设置停止位
void SerialPortThread::setSerialPortStopBits(QSerialPort::StopBits s)
{
    stopBits = s;
}

void SerialPortThread::run()
{
    //打开串口
    QSerialPort *serialPort = new QSerialPort;
    serialPort->setPortName(serialPortNumber);
    serialPort->setBaudRate(baudRate);
    serialPort->setParity(parity);
    serialPort->setDataBits(dataBits);
    serialPort->setStopBits(stopBits);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(!serialPort->open(QIODevice::ReadWrite))
    {
        //串口打开不成功
        isEnd = true;
        emit serialPortOpenStatus(-1);
        delete serialPort;//清理资源
        return;
    }
    else
    {//打开成功
        isEnd = false;
        emit serialPortOpenStatus(1);
    }

    while(!isEnd)
    {//读串口
        if(serialPort->waitForReadyRead(50))
        {
           receivedData = serialPort->readAll();
           std::cout<<"receive data:"<<receivedData.toStdString()<<std::endl;
           emit serialPortReceivedData(receivedData);
        }
        //如果有数据要发送
        if(sendFlag)
        {//发送数据
            serialPort->write(sendData);
            sendFlag = false;
            if(serialPort->waitForBytesWritten(50))//接收回应
            {//将回应全部接收
                receivedData = serialPort->readAll();
                while (serialPort->waitForReadyRead(100))
                    receivedData += serialPort->readAll();
                emit serialPortReceivedData(receivedData);
                std::cout<<"receive data:"<<receivedData.toStdString()<<std::endl;
            }
        }
    }
    //关闭串口
    if(serialPort->isOpen())
        serialPort->close();
    isEnd = true;
    emit serialPortOpenStatus(0);
    delete serialPort;//清理资源
}
//获取线程是否停止的标志
bool SerialPortThread::getIsEnd()
{
    return isEnd;
}
//设置线程是否停止标志
void SerialPortThread::setIsEnd(bool b)
{
    isEnd = b;
}
//设置发送信息的标志
void SerialPortThread::setSendFlag(bool b)
{
    sendFlag = b;
}
//获取发送信息的标志
bool SerialPortThread::getSendFlag()
{
    return sendFlag;
}
//设置发送数据
void SerialPortThread::setSendData(QByteArray send)
{
    sendData = send;
}
