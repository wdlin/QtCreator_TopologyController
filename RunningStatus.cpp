#include "RunningStatus.h"

RunningStatus::RunningStatus():isDynamicConfigure(false),isSwitchRunning(false),isSerialPortOn(false)
{}
RunningStatus::~RunningStatus()
{}

RunningStatus* RunningStatus::instance = new RunningStatus();

RunningStatus* RunningStatus::getInstance()
{
    return instance;
}

bool RunningStatus::getIsDynamicConfigure()
{
    return isDynamicConfigure;
}

bool RunningStatus::getIsSwitchRunning()
{
    return isSwitchRunning;
}

bool RunningStatus::getIsSerialPortOn()
{
    return isSerialPortOn;
}

void RunningStatus::setIsDynamicConfigure(bool d)
{
    isDynamicConfigure = d;
}

void RunningStatus::setIsSwitchRunning(bool r)
{
    isSwitchRunning = r;
}

void RunningStatus::setIsSerialPortOn(bool s)
{
    isSerialPortOn = s;
}
