#include "SwitchStatus.h"
#include <iostream>
#include <ctime>
SwitchStatus* SwitchStatus::instance = new SwitchStatus();

SwitchStatus::SwitchStatus():areaMaxX(600),areaMaxY(350),speed(30),connectedDistance(20)
{}

SwitchStatus::~SwitchStatus()
{}

SwitchStatus::SwitchStatus(const SwitchStatus&)
{}


SwitchStatus* SwitchStatus::getInstance()
{
    return instance;
}

bool SwitchStatus::addSwitch(std::string& name,Switch& s)
{
    switches_mutex.lock();
    auto res = switches.insert({name,s});
    switches_mutex.unlock();
    return res.second;

}

bool SwitchStatus::removeSwitch(std::string& name)
{
    if(switches.count(name)==0)
    {
        return false;
    }
    else
    {
        switches_mutex.lock();
        deleteSwitchPorts.push_back(switches[name].port);
        switches.erase(name);
        switches_mutex.unlock();
        return true;
    }
}

bool SwitchStatus::modifySwitch(std::string& originName,std::string& newName,Switch& s)
{
    if(switches.count(originName)==0)
    {
        return false;
    }
    switches_mutex.lock();
    if(originName.compare(newName) == 0)
    {
        switches[newName] = s;
    }
    else
    {
        switches.erase(originName);
        switches[newName] = s;       
    }
    switches_mutex.unlock();
    return true;
}

std::vector<SwitchData> SwitchStatus::randomRunSwitch(int second)
{

    double dx;
    double dy;
    double ds = speed * second/3600.0;
    std::vector<SwitchData> switchesInfo;
    srand(time(NULL));
    switches_mutex.lock();
    for(auto& p:switches)
    {
        Switch& s = p.second;
        dx = (rand()%200 - 100)/100.0;
        dy = (rand()%200 - 100)/100.0;
        s.x = s.x + ds*dx;
        s.y = s.y + ds*dy;
        if(s.x<0)s.x = 0;
        if(s.x>areaMaxX)s.x = areaMaxX;
        if(s.y<0)s.y = 0;
        if(s.y>areaMaxY)s.y = areaMaxY;

        SwitchData sd;
        sd.name = p.first;
        sd.port = s.port;
        sd.x = s.x;
        sd.y = s.y;
        switchesInfo.push_back(sd);
    }
    switches_mutex.unlock();
    return switchesInfo;
}
std::vector<SwitchData> SwitchStatus::getStaticSwitchesInfo()
{
    std::vector<SwitchData> switchesInfo;
    switches_mutex.lock();
    for(auto& p:switches)
    {
        Switch& s = p.second;
        SwitchData sd;
        sd.name = p.first;
        sd.port = s.port;
        sd.x = s.x;
        sd.y = s.y;
        switchesInfo.push_back(sd);
    }
    switches_mutex.unlock();
    return switchesInfo;
}

std::vector<int> SwitchStatus::getDeleteSwitchPorts()
{
    std::vector<int> dsp = deleteSwitchPorts;
    deleteSwitchPorts.clear();
    return dsp;
}

void SwitchStatus::print()
{
    for(auto& p:switches)
    {
        Switch s = p.second;
        std::cout<<p.first<<" :port:"<<s.port<<" x:"<<s.x<<" y:"<<s.y<<std::endl;
    }
}

double SwitchStatus::getAreaMaxX()
{
    return areaMaxX;
}

double SwitchStatus::getAreaMaxY()
{
    return areaMaxY;
}

double SwitchStatus::getSpeed()
{
    return speed;
}


double SwitchStatus::getConnectedDistance()
{
    return connectedDistance;
}

bool SwitchStatus::setAreaMaxX(double x)
{
    areaMaxX = x;
    bool flag = false;
    for(auto& p:switches)
    {
        if(p.second.x > x)
        {
            p.second.x = x;
            flag = true;
        }
    }
    return flag;
}

bool SwitchStatus::setAreaMaxY(double y)
{
    areaMaxY = y;
    bool flag = false;
    for(auto& p:switches)
    {
        if(p.second.y > y)
        {
            p.second.y = y;
            flag = true;
        }
    }
    return flag;
}

void SwitchStatus::setSpeed(double s)
{
    speed = s;
}

void SwitchStatus::setConnectedDistance(double d)
{
    connectedDistance = d;
}

bool SwitchStatus::isPortUsed(int port)
{
    bool isUsed = false;
    for(auto& p:switches)
    {
        if(port == p.second.port)
        {
            isUsed = true;
            break;
        }
    }
    return isUsed;
}

bool SwitchStatus::isNameUsed(std::string& name)
{
    if(switches.count(name)==1)
        return true;
    else
        return false;
}
