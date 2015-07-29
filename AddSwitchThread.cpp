#include "AddSwitchThread.h"
#include "SwitchStatus.h"
#include "RunningStatus.h"
AddSwitchThread::AddSwitchThread()
{

}

AddSwitchThread::~AddSwitchThread()
{

}

void AddSwitchThread::setAddSwitchData(const SwitchData &sd)
{
    switchData = sd;
}

void AddSwitchThread::run()
{
    SwitchStatus::getInstance()->addSwitch(switchData.name,Switch(switchData.port,switchData.x,switchData.y));
    if(!RunningStatus::getInstance()->getIsSwitchRunning())
    {
        emit updateUIStaticAnimation(SwitchStatus::getInstance()->getStaticSwitchesInfo());
    }
}
