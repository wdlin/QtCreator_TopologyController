#ifndef ADDSWITCHTHREAD_H
#define ADDSWITCHTHREAD_H

#include <QThread>
#include <string>
#include "SwitchData.h"
//增加交换机的线程
class AddSwitchThread:public QThread
{
    Q_OBJECT
public:
    AddSwitchThread();
    ~AddSwitchThread();
    void setAddSwitchData(const SwitchData& sd);//设置增加的交换机信息
signals:
    void updateUIStaticAnimation(std::vector<SwitchData> switchesInfo);//更新主窗口ui的信号
protected:
    void run();
private:
    SwitchData switchData;//待增加的交换机信息
};

#endif // ADDSWITCHTHREAD_H
