#ifndef DELETETHREAD_H
#define DELETETHREAD_H

#include <QThread>
#include <string>
#include <vector>
#include "SwitchData.h"
//删除交换机的线程
class DeleteSwitchThread : public QThread
{
    Q_OBJECT
public:
    DeleteSwitchThread();
    void setDeleteSwitchName(std::string& n);//设置删除交换机的名字
signals:
    void updateUIStaticAnimation(std::vector<SwitchData> switchesInfo);//更新主窗口ui的信号
protected:
    void run();
private:
    std::string name;//待删除的交换机名字
};



#endif // DELETETHREAD

