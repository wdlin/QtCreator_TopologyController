#include "DeleteSwitchThread.h"
#include "SwitchStatus.h"
#include "RunningStatus.h"
#include <iostream>
DeleteSwitchThread::DeleteSwitchThread()
{
}

void DeleteSwitchThread::setDeleteSwitchName(std::string& n)
{
    name = n;
}
void DeleteSwitchThread::run()
{
    SwitchStatus::getInstance()->removeSwitch(name);
    std::cout<<"delete "<<name<<"..."<<std::endl;
    if(!RunningStatus::getInstance()->getIsSwitchRunning())
    {
        emit updateUIStaticAnimation(SwitchStatus::getInstance()->getStaticSwitchesInfo());
    }
}
