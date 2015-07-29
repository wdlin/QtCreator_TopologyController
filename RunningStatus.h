#ifndef RUNNINGSTATUS_H
#define RUNNINGSTATUS_H
/*
保存软件信息的单例类
*/
#include <QObject>

class RunningStatus:public QObject
{
    Q_OBJECT
public:
    ~RunningStatus();
    static RunningStatus* getInstance();//获取单例
    void init();//初始化函数
    bool getIsDynamicConfigure();//获取是否正在动态配置
    bool getIsSwitchRunning();//获取是否正在运动交换机
    bool getIsSerialPortOn();//获取串口是否打开
    void setIsDynamicConfigure(bool d);//设置是否正在动态配置标志
    void setIsSwitchRunning(bool r);//设置是否正在运动交换机标志
    void setIsSerialPortOn(bool s);//设置串口是否打开标志
private:
    RunningStatus();//私有构造函数
    RunningStatus(const RunningStatus&);//私有复制构造函数
    RunningStatus& operator=(const RunningStatus);//私有赋值运算符
    static RunningStatus* instance;//单例对象

    volatile bool isDynamicConfigure;//是否正在动态配置
    volatile bool isSwitchRunning;//交换机是否在运动
    volatile bool isSerialPortOn;//串口是否打开

};


#endif // RUNNINGSTATUS

