#ifndef MODIFYTHREAD_H
#define MODIFYTHREAD_H

#include <QThread>
#include "SwitchData.h"
#include <string>
//交换机信息修改线程
class ModifySwitchThread : public QThread
{
    Q_OBJECT
public:
    ModifySwitchThread();
    void setModifySwitchData(const std::string oname,const SwitchData& sd);//设置更改的交换机数据
signals:
    void updateUIStaticAnimation(std::vector<SwitchData> switchesInfo);//更新主窗口的动画
protected:
    void run();
private:
    SwitchData switchData;//要更改的交换机数据
    std::string originName;//原来交换机的名字

};

#endif // MODIFYTHREAD

