#include "RunSwitchThread.h"
#include <string>
#include "SwitchStatus.h"i"
RunSwitchThread::RunSwitchThread():isEnd(false)
{

}

RunSwitchThread::~RunSwitchThread()
{

}

void RunSwitchThread::setIsEnd(bool b)
{
    isEnd = b;
}

bool RunSwitchThread::getIsEnd()
{
    return isEnd;
}
void RunSwitchThread::run()
{
    while(!isEnd)
    {
        std::vector<SwitchData> switchesInfo;
        switchesInfo = SwitchStatus::getInstance()->randomRunSwitch(3600);
        emit updateUIAnimation(switchesInfo);
        sleep(3);
    }

}

