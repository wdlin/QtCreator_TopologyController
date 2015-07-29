#ifndef RUNSWITCHTHREAD_H
#define RUNSWITCHTHREAD_H
#include <QThread>
#include <vector>
#include "SwitchData.h"
#include <QVector>
class RunSwitchThread:public QThread
{
    Q_OBJECT
public:
    RunSwitchThread();
    ~RunSwitchThread();
    void setIsEnd(bool b);//设置线程结束标志
    bool getIsEnd();//获取线程结束标志
signals:
    void updateUIAnimation(std::vector<SwitchData> switchesInfo);//更新主窗口的信号
protected:
    void run();
private:
    bool isEnd;//线程结束标志
};

#endif // RUNSWITCHTHREAD_H
