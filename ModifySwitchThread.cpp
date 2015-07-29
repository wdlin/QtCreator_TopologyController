#include "ModifySwitchThread.h"
#include "SwitchStatus.h"
#include "Switch.h"
#include "RunningStatus.h"
ModifySwitchThread::ModifySwitchThread()
{}

void ModifySwitchThread::setModifySwitchData(const std::string oname,const SwitchData &sd)
{
    switchData = sd;
    originName = oname;
}

void ModifySwitchThread::run()
{
    SwitchStatus::getInstance()->modifySwitch(originName,switchData.name,Switch(switchData.port,switchData.x,switchData.y));
    if(!RunningStatus::getInstance()->getIsSwitchRunning())
    {
        emit updateUIStaticAnimation(SwitchStatus::getInstance()->getStaticSwitchesInfo());
    }
}
