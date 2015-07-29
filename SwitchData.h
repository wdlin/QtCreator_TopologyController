#ifndef SWITCHDATA_H
#define SWITCHDATA_H
#include <QMetaType>
#include <string>
//交换机信息类
class SwitchData
{
public:
    std::string name;//名字
    int port;//端口
    double x;//x坐标
    double y;//y坐标
};
Q_DECLARE_METATYPE(SwitchData)
#endif // SWITCHDATA

