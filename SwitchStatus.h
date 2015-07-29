#ifndef SWITCHSTATUS_H
#define SWITCHSTATUS_H

/*
 保存交换机信息的单例类
*/
#include <QObject>
#include <string>
#include "Switch.h"
#include <map>
#include <vector>
#include "SwitchData.h"
#include <QMutex>
#include <utility>
#include <set>
//交换机状态信息单例类
class SwitchStatus:public QObject
{
    Q_OBJECT
public:
    static SwitchStatus* getInstance();//获取单例类
    ~SwitchStatus();
    bool addSwitch(std::string& name,Switch& s);//增加交换机
    bool removeSwitch(std::string& name);//删除交换机
    bool modifySwitch(std::string& originName,std::string& newName,Switch& s);//修改交换机
    std::vector<SwitchData> randomRunSwitch(int second);//随机运动交换机，输入时间间隔
    void print();//输出交换机信息
    double getAreaMaxX();//获取区域maxX
    double getAreaMaxY();//获取区域maxY
    double getSpeed();//获取运动速度
    double getConnectedDistance();//获取连接距离
    std::vector<SwitchData> getStaticSwitchesInfo();//获取交换机目前的状态信息
    std::vector<int> getDeleteSwitchPorts();//获取删除了的交换机端口（还没有更新的连接器的）
    bool setAreaMaxX(double x);//设置区域maxX
    bool setAreaMaxY(double y);//设置区域maxY
    void setSpeed(double s);//设置运动速度
    void setConnectedDistance(double d);//设置连接距离
    bool isPortUsed(int port);//检测端口是否已经被使用
    bool isNameUsed(std::string& name);//检测交换机名字是否已经被使用
private:
    SwitchStatus();//私有构造函数
    SwitchStatus(const SwitchStatus&);//私有复制构造函数
    SwitchStatus& operator=(const SwitchStatus&);//私有赋值运算符
    static SwitchStatus* instance;//单例对象
    std::map<std::string,Switch> switches;//交换机信息
    std::vector<int> deleteSwitchPorts;//删除的交换机端口
    double areaMaxX;//区域maxX
    double areaMaxY;//区域maxY
    double speed;//速度
    double connectedDistance;//连接距离

    QMutex switches_mutex;//交换机读写互斥类
};

#endif // SWITCHSTATUS

