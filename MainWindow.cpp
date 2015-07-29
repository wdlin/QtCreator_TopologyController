#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox.h"
#include <iostream>
#include <string>
#include "SwitchStatus.h"
#include "Switch.h"
#include "RunningStatus.h"
#include <QDateTime>
#include "DeleteSwitchThread.h"
#include <QStringList>
#include "ModifySwitchDialog.h"
#include <QDesktopWidget>
#include "AddSwitchThread.h"
#include "RunSwitchThread.h"
#include "ModifySwitchThread.h"
#include <QPainter>
#include <fstream>
#include <sstream>
#include "SerialPortThread.h"
#include "SwitchData.h"
#include <cmath>
#include <QCloseEvent>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    if(serialPortThread->isRunning())
    {
        serialPortThread->setIsEnd(true);
        serialPortThread->wait();//qt的wait类似于join()
    }
    delete serialPortThread;
    if(runSwitchThread->isRunning())
    {
        runSwitchThread->setIsEnd(true);
        runSwitchThread->wait();//qt的wait类似于join()
    }
    delete runSwitchThread;
}

void MainWindow::init()
{
    deleteSwitchThread = new DeleteSwitchThread();
    addSwitchThread = new AddSwitchThread();
    runSwitchThread = new RunSwitchThread();
    modifySwitchDialog = new ModifySwitchDialog();
    serialPortThread = new SerialPortThread();


    readSetting("data.csv");
    initUI();


    connect(runSwitchThread,SIGNAL(updateUIAnimation(std::vector<SwitchData>)),this,SLOT(updateAnimation(std::vector<SwitchData>)), Qt::QueuedConnection);
    connect(modifySwitchDialog,SIGNAL(updateUIListWidget(SwitchData&)),this,SLOT(updateListWidget(SwitchData&)));
    connect(addSwitchThread,SIGNAL(updateUIStaticAnimation(std::vector<SwitchData>)),this,SLOT(updateStaticAnimation(std::vector<SwitchData>)));
    connect(deleteSwitchThread,SIGNAL(updateUIStaticAnimation(std::vector<SwitchData>)),this,SLOT(updateStaticAnimation(std::vector<SwitchData>)));
    connect(modifySwitchDialog->modifySwitchThread,SIGNAL(updateUIStaticAnimation(std::vector<SwitchData>)),this,SLOT(updateStaticAnimation(std::vector<SwitchData>)));
    connect(serialPortThread,SIGNAL(serialPortOpenStatus(int)),this,SLOT(serialPortOpenStatus(int)));   
    //菜单
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(actionAbout()));
    connect(ui->actionSaveSetting,SIGNAL(triggered()),this,SLOT(actionSaveSetting()));
    connect(ui->actionSaveSettingAs,SIGNAL(triggered()),this,SLOT(actionSaveSettingAs()));

}

QString MainWindow::getCommandStringFromSwitches()
{
    QString commandStr = "\rconfigure t\n";//第一个回车是唤醒连接器
    //首先处理之前删除的交换机
    std::vector<int> deleteSwitchPorts = SwitchStatus::getInstance()->getDeleteSwitchPorts();
    if(!deleteSwitchPorts.empty())
    {
        for(int i = 0; i<deleteSwitchPorts.size(); i++)
        {
            QString str = QString("no map 1.1.%1").arg(deleteSwitchPorts[i]) + "\n";
            commandStr.append(str);
        }
    }
    std::vector<SwitchData> switchesInfo = SwitchStatus::getInstance()->getStaticSwitchesInfo();
    if(switchesInfo.size()<=1)
        return commandStr;
    //std::set<std::pair<int,int>> pairs;
    double connectedDistance = SwitchStatus::getInstance()->getConnectedDistance();
    for(int i = 0; i<switchesInfo.size() - 1; i++)
    {
        for(int j = i+1; j<switchesInfo.size(); j++)
        {
            if(sqrt(pow(switchesInfo[i].x - switchesInfo[j].x , 2)+pow(switchesInfo[i].y - switchesInfo[j].y , 2)) < connectedDistance)
            {
                //pairs.insert(make_pair(switchesInfo[i].port,switchesInfo[j].port));
                QString str = QString("map 1.1.%1 with 1.1.%2").arg(switchesInfo[i].port).arg(switchesInfo[j].port) + "\n";
                commandStr.append(str);
            }
            else
            {
                QString str = QString("no map 1.1.%1 with 1.1.%2").arg(switchesInfo[i].port).arg(switchesInfo[j].port) + "\n";
                commandStr.append(str);
            }
        }
    }
    commandStr.append("end\n");
    return commandStr;
}

//点击增加交换机按钮的响应
void MainWindow::on_addSwitchButton_clicked()
{
    std::string name = ui->switchNameLineEdit->text().trimmed().toStdString();
    if(name.compare("")==0)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机名字不能为空！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if(SwitchStatus::getInstance()->isNameUsed(name))
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机名字已经被占用！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    int port = ui->switchPortComboBox->currentIndex() + 1;
    if(SwitchStatus::getInstance()->isPortUsed(port))
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("端口已经被占用！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    double x = ui->switchXLineEdit->text().toDouble();
    if(x < 0 || x > SwitchStatus::getInstance()->getAreaMaxX())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("X坐标不能为负数或者大于区域Max X！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    double y = ui->switchYLineEdit->text().toDouble();
    if(y < 0 || y > SwitchStatus::getInstance()->getAreaMaxY())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("Y坐标不能为负数或者大于区域Max Y！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    SwitchData sd;
    sd.name = name;
    sd.port =port;
    sd.x = x;
    sd.y = y;
    if(addSwitchThread->isRunning())
       ;
    else
    {
        addSwitchThread->setAddSwitchData(sd);
        addSwitchThread->start();

        QString item ="name:" + QString::fromStdString(name)+" port:"
                +QString::number(port)+" x:"+ QString::number(x)+ " y:"+QString::number(y);
        ui->switchListWidget->addItem(item);
    }

}
//点击设置连接距离按钮的响应
void MainWindow::on_setConnectedDistanceButton_clicked()
{
    double distance = ui->connectedDistanceLineEdit->text().toDouble();
    if(distance <= 0)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("连接距离不能为空或者小等于0！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    SwitchStatus::getInstance()->setConnectedDistance(distance);
    ui->showCurrentDistanceLabel->setText(QString::number(distance));
    ui->paintWidget->update();
}
//点击设置max X按钮的响应
void MainWindow::on_setMaxXButton_clicked()
{
    if(RunningStatus::getInstance()->getIsSwitchRunning())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机正在运动，不能设置Max X！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    double maxX = ui->maxXLineEdit->text().toDouble();
    if(maxX <= 0)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("Max X不能为空或者小等于0！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    bool flag = SwitchStatus::getInstance()->setAreaMaxX(maxX);
    ui->showCurrentMaxXLabel->setText(QString::number(maxX));
    if(flag)
    {
        auto switchesInfo = SwitchStatus::getInstance()->getStaticSwitchesInfo();
        int count = ui->switchListWidget->count();
        int i = 0;
        for(auto it = switchesInfo.begin();it!=switchesInfo.end() && i<count;++it,i++)
        {
            QString item ="name:" + QString::fromStdString(it->name)+" port:"
                    +QString::number(it->port)+" x:"+ QString::number(it->x)+ " y:"+QString::number(it->y);
            ui->switchListWidget->item(i)->setText(item);
        }
        ui->paintWidget->setPaintData(switchesInfo);
        ui->paintWidget->update();
    }
    else
    {
        ui->paintWidget->update();
    }
}

//点击设置max Y按钮的响应
void MainWindow::on_setMaxYButton_clicked()
{
    if(RunningStatus::getInstance()->getIsSwitchRunning())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机正在运动，不能设置Max Y！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    double maxY = ui->maxYLineEdit->text().toDouble();
    if(maxY <= 0)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("Max Y不能为空或者小等于0！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    bool flag = SwitchStatus::getInstance()->setAreaMaxY(maxY);
    ui->showCurrentMaxYLabel->setText(QString::number(maxY));
    if(flag)
    {
        auto switchesInfo = SwitchStatus::getInstance()->getStaticSwitchesInfo();
        int count = ui->switchListWidget->count();
        int i = 0;
        for(auto it = switchesInfo.begin();it!=switchesInfo.end() && i<count;++it,i++)
        {
            QString item ="name:" + QString::fromStdString(it->name)+" port:"
                    +QString::number(it->port)+" x:"+ QString::number(it->x)+ " y:"+QString::number(it->y);
            ui->switchListWidget->item(i)->setText(item);
        }
        ui->paintWidget->setPaintData(switchesInfo);
        ui->paintWidget->update();
    }
    else
    {
        ui->paintWidget->update();
    }
}
//点击设置速度按钮的响应
void MainWindow::on_setSpeedButton_clicked()
{
/*    if(RunningStatus::getInstance()->getIsSwitchRunning())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机正在运动，不能设置速度！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    */
    double speed = ui->speedLineEdit->text().toDouble();
    if(speed <= 0)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("速度不能为空或者小等于0！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    SwitchStatus::getInstance()->setSpeed(speed);
    ui->showCurrentSpeedLabel->setText(QString::number(speed));
}
//点击静态配置按钮的响应
void MainWindow::on_onceConfigureButton_clicked()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    //QString str = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    QString timeStr = time.toString("MM-dd hh:mm:ss");
    if(!RunningStatus::getInstance()->getIsSerialPortOn())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("串口没有打开！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if(RunningStatus::getInstance()->getIsDynamicConfigure())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("正在动态配置，静态配置失败！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QString commandStr = getCommandStringFromSwitches();
    serialPortThread->setSendData(commandStr.toUtf8());
    serialPortThread->setSendFlag(true);
    std::cout<<commandStr.toStdString()<<std::endl;
    ui->showOnceConfigureStatusLabel->setText(timeStr+" "+QStringLiteral("配置了一次。"));
}
//点击动态配置按钮的响应
void MainWindow::on_dynamicConfigureButton_clicked()
{
    if(!RunningStatus::getInstance()->getIsSerialPortOn())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("串口没有打开！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if(!RunningStatus::getInstance()->getIsSwitchRunning())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("请先使交换机开始运动！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = time.toString("MM-dd hh:mm:ss");//设置显示格式
    if(RunningStatus::getInstance()->getIsDynamicConfigure())
    {
        RunningStatus::getInstance()->setIsDynamicConfigure(false);
        ui->dynamicConfigureButton->setText(QStringLiteral("开始动态配置"));
        ui->showDynamicConfigureStatusLabel->setText(str + QStringLiteral("停止了配置。"));
    }
    else
    {
        RunningStatus::getInstance()->setIsDynamicConfigure(true);
        ui->dynamicConfigureButton->setText(QStringLiteral("停止动态配置"));
        ui->showDynamicConfigureStatusLabel->setText(str +" " + QStringLiteral("开始了配置。"));
    }

}
//点击开始（停止）运动按钮的响应
void MainWindow::on_runSwithButton_clicked()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = time.toString("MM-dd hh:mm:ss");//设置显示格式
    if(RunningStatus::getInstance()->getIsSwitchRunning())
    {
        if(RunningStatus::getInstance()->getIsDynamicConfigure())
        {
            on_dynamicConfigureButton_clicked();//先停止动态配置
        }
        RunningStatus::getInstance()->setIsSwitchRunning(false);
        ui->runSwithButton->setText(QStringLiteral("开始运动"));
        ui->showRunStatusLabel->setText(str +" " + QStringLiteral("停止了运动。"));
        runSwitchThread->setIsEnd(true);  
    }
    else
    {
        RunningStatus::getInstance()->setIsSwitchRunning(true);
        ui->runSwithButton->setText(QStringLiteral("停止运动"));
        ui->showRunStatusLabel->setText(str +" " + QStringLiteral("开始了运动。"));

        runSwitchThread->setIsEnd(false);
        runSwitchThread->start();
    }

}

void MainWindow::on_modifySwitchButton_clicked()
{
    int index = ui->switchListWidget->currentRow();
    if(index == -1)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("请先选择要修改的交换机！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString str = ui->switchListWidget->item(index)->text();
    QStringList strList = str.split(QRegExp("[ :]"));//以空格或者冒号分割字符串
    SwitchData sd;
    sd.name = strList[1].toStdString();
    sd.port = strList[3].toInt();
    sd.x = strList[5].toDouble();
    sd.y = strList[7].toDouble();

    modifySwitchDialog->setOriginalSwitchData(sd);
    QDesktopWidget *pDesk = QApplication::desktop();//移到屏幕中间
    modifySwitchDialog->move((pDesk->width() - modifySwitchDialog->width()) / 2, (pDesk->height() - modifySwitchDialog->height()) / 2);
    modifySwitchDialog->exec();

}

void MainWindow::on_deleteSwitchButton_clicked()
{
    int index = ui->switchListWidget->currentRow();
    if(index == -1)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("请先选择要删除的交换机！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString str = ui->switchListWidget->item(index)->text();
    QStringList strList = str.split(QRegExp("[ :]"));//以空格或者冒号分割字符串

    if(deleteSwitchThread->isRunning())
       ;
    else
    {
        deleteSwitchThread->setDeleteSwitchName(strList.at(1).toStdString());
        deleteSwitchThread->start();

        ui->switchListWidget->takeItem(index);
    }

}

void MainWindow::updateAnimation(std::vector<SwitchData> switchesInfo)
{

    int count = ui->switchListWidget->count();
    int i = 0;
    for(auto it = switchesInfo.begin();it!=switchesInfo.end() && i<count;++it,i++)
    {
        QString item ="name:" + QString::fromStdString(it->name)+" port:"
                +QString::number(it->port)+" x:"+ QString::number(it->x)+ " y:"+QString::number(it->y);
        //ui->switchListWidget->addItem(item);
        ui->switchListWidget->item(i)->setText(item);
        std::cout<<it->name<<": port :"<<it->port<<" x:"<<it->x<<" y:"<<it->y<<std::endl;

    }
    ui->paintWidget->setPaintData(switchesInfo);
    ui->paintWidget->update();

    if(RunningStatus::getInstance()->getIsDynamicConfigure())
    {
        QString commandStr = "\rconfigure t\n";//第一个回车是唤醒连接器
        //首先处理之前删除的交换机
        std::vector<int> deleteSwitchPorts = SwitchStatus::getInstance()->getDeleteSwitchPorts();
        if(!deleteSwitchPorts.empty())
        {
            for(int i = 0; i<deleteSwitchPorts.size(); i++)
            {
                QString str = QString("no map 1.1.%1").arg(deleteSwitchPorts[i]) + "\n";
                commandStr.append(str);
            }
        }
        if(switchesInfo.size()<=1)
            return ;
        double connectedDistance = SwitchStatus::getInstance()->getConnectedDistance();
        for(int i = 0; i<switchesInfo.size() - 1; i++)
        {
            for(int j = i+1; j<switchesInfo.size(); j++)
            {
                if(sqrt(pow(switchesInfo[i].x - switchesInfo[j].x , 2)+pow(switchesInfo[i].y - switchesInfo[j].y , 2)) < connectedDistance)
                {
                    QString str = QString("map 1.1.%1 with 1.1.%2").arg(switchesInfo[i].port).arg(switchesInfo[j].port) + "\n";
                    commandStr.append(str);
                }
                else
                {
                    QString str = QString("no map 1.1.%1 with 1.1.%2").arg(switchesInfo[i].port).arg(switchesInfo[j].port) + "\n";
                    commandStr.append(str);
                }
            }
        }
        commandStr.append("end\n");
        serialPortThread->setSendData(commandStr.toUtf8());
        serialPortThread->setSendFlag(true);
        std::cout<<commandStr.toStdString()<<"****"<<std::endl;
    }

}

void MainWindow::updateStaticAnimation(std::vector<SwitchData> switchesInfo)
{
    ui->paintWidget->setPaintData(switchesInfo);
    ui->paintWidget->update();
}

//void MainWindow::readDefaultData()
void MainWindow::readSetting(std::string fileName)
{
    using namespace std;
    ifstream input(fileName);
    if(!input.fail())
    {
        string line;
        getline(input,line);
        QString qLine = QString::fromStdString(line);
        QStringList strList = qLine.split(",");
        bool ok;
        double maxX = strList[1].toDouble(&ok);
        if(!ok)maxX = 710;
        double maxY = strList[3].toDouble(&ok);
        if(!ok)maxY = 410;

        getline(input,line);
        qLine = QString::fromStdString(line);
        strList = qLine.split(",");
        double connectedDistance = strList[1].toDouble(&ok);
        if(!ok)connectedDistance = 50;
        double speed = strList[3].toDouble(&ok);
        if(!ok)speed = 60;

        SwitchStatus::getInstance()->setAreaMaxX(maxX);
        SwitchStatus::getInstance()->setAreaMaxY(maxY);
        SwitchStatus::getInstance()->setConnectedDistance(connectedDistance);
        SwitchStatus::getInstance()->setSpeed(speed);

        getline(input,line);
        vector<SwitchData> switchesInfo;
        while(input.good())
        {
            getline(input,line);
            qLine = QString::fromStdString(line);
            strList = qLine.split(",");

            string name = strList[0].toStdString();
            if(name.compare("") == 0)break;
            if(SwitchStatus::getInstance()->isNameUsed(name))break;

            int port = strList[1].toInt(&ok);
            if(!ok)break;
            if(SwitchStatus::getInstance()->isPortUsed(port))break;

            double x = strList[2].toDouble(&ok);
            if(!ok)break;
            if(x<0)x = 0;
            if(x>maxX)x = maxX;

            double y = strList[3].toDouble(&ok);
            if(!ok)break;
            if(y<0)y = 0;
            if(y>maxY)y = maxY;

            SwitchStatus::getInstance()->addSwitch(name,Switch(port,x,y));
            QString item ="name:" + QString::fromStdString(name)+" port:"
                    +QString::number(port)+" x:"+ QString::number(x)+ " y:"+QString::number(y);
            ui->switchListWidget->addItem(item);

            SwitchData sd;
            sd.name = name;
            sd.port = port;
            sd.x = x;
            sd.y =y;
            switchesInfo.push_back(sd);
        }
        ui->paintWidget->setPaintData(switchesInfo);
        ui->paintWidget->update();
    }
    else
    {
        SwitchStatus::getInstance()->setAreaMaxX(710);
        SwitchStatus::getInstance()->setAreaMaxY(410);
        SwitchStatus::getInstance()->setConnectedDistance(60);
        SwitchStatus::getInstance()->setSpeed(60);
    }
}

void MainWindow::initUI()
{
    //设置输入的格式
    QRegExp regExp("[A-Za-z][A-Za-z0-9]*");//以字母开头，只能输入字母和数字
    ui->switchNameLineEdit->setValidator(new QRegExpValidator(regExp));
    ui->switchXLineEdit->setValidator(new QDoubleValidator);//后面这些都是只能输入浮点数格式
    ui->switchYLineEdit->setValidator(new QDoubleValidator);
    ui->connectedDistanceLineEdit->setValidator(new QDoubleValidator);
    ui->maxXLineEdit->setValidator(new QDoubleValidator);
    ui->maxYLineEdit->setValidator(new QDoubleValidator);
    ui->speedLineEdit->setValidator(new QDoubleValidator);

    auto p = SwitchStatus::getInstance();
    ui->showCurrentMaxXLabel->setText(QString::number(p->getAreaMaxX()));
    ui->showCurrentMaxYLabel->setText(QString::number(p->getAreaMaxY()));
    ui->showCurrentDistanceLabel->setText(QString::number(p->getConnectedDistance()));
    ui->showCurrentSpeedLabel->setText(QString::number(p->getSpeed()));
}

void MainWindow::updateListWidget(SwitchData &sd)
{
    int index = ui->switchListWidget->currentRow();
    QString str ="name:" + QString::fromStdString(sd.name)+" port:"
            +QString::number(sd.port)+" x:"+ QString::number(sd.x)+ " y:"+QString::number(sd.y);
    ui->switchListWidget->item(index)->setText(str);
}

void MainWindow::on_onOffSerialPortButton_clicked()
{
    if(RunningStatus::getInstance()->getIsSerialPortOn())
    {
        if(RunningStatus::getInstance()->getIsDynamicConfigure())
            on_dynamicConfigureButton_clicked();//关闭动态配置
        serialPortThread->setIsEnd(true);//关闭串口
    }
    else
    {
        serialPortThread->setSerialPortNumber(ui->serialPortComboBox->currentText());
        serialPortThread->setSerialPortBaudRate(ui->baudRateComboBox->currentText().toInt());
        serialPortThread->setIsEnd(false);//打开串口
        serialPortThread->start();
    }
}

void MainWindow::serialPortOpenStatus(int status)
{
    switch(status)
    {
    case -1:
    {
        ui->onOffSerialPortButton->setText(QStringLiteral("打开串口"));
        ui->baudRateComboBox->setEnabled(true);
        ui->serialPortComboBox->setEnabled(true);
        ui->showSerialPortLabel->setText(QStringLiteral("串口没有打开！"));
        RunningStatus::getInstance()->setIsSerialPortOn(false);
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("串口号不存在或者被其他程序占用！"), QMessageBox::Ok, QMessageBox::Ok);
        break;
    }
    case 0:
    {
        ui->onOffSerialPortButton->setText(QStringLiteral("打开串口"));
        ui->baudRateComboBox->setEnabled(true);
        ui->serialPortComboBox->setEnabled(true);
        ui->showSerialPortLabel->setText(QStringLiteral("串口没有打开！"));
        RunningStatus::getInstance()->setIsSerialPortOn(false);
        break;
    }
    case 1:
    {
        ui->onOffSerialPortButton->setText(QStringLiteral("关闭串口"));
        ui->baudRateComboBox->setEnabled(false);
        ui->serialPortComboBox->setEnabled(false);
        ui->showSerialPortLabel->setText(QStringLiteral("串口已经打开！"));
        RunningStatus::getInstance()->setIsSerialPortOn(true);
        break;
    }
    default:
        break;

    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //event->accept();//关闭程序
    //event->ignore();//忽略关闭事件
    int res = QMessageBox::question(NULL,QStringLiteral("问题"),QStringLiteral("是否保存数据？"),QMessageBox::Cancel|QMessageBox::Yes|QMessageBox::No,QMessageBox::Cancel);
    if(res == QMessageBox::Yes)
    {
        if(serialPortThread->isRunning())
        {
            serialPortThread->setIsEnd(true);
            serialPortThread->wait();//qt的wait类似于join()
        }
        if(runSwitchThread->isRunning())
        {
            runSwitchThread->setIsEnd(true);
            runSwitchThread->wait();//qt的wait类似于join()
        }
        saveSetting("data.csv");
        event->accept();
    }
    else if(res == QMessageBox::No)
    {
        if(serialPortThread->isRunning())
        {
            serialPortThread->setIsEnd(true);
            serialPortThread->wait();//qt的wait类似于join()
        }
        if(runSwitchThread->isRunning())
        {
            runSwitchThread->setIsEnd(true);
            runSwitchThread->wait();//qt的wait类似于join()
        }
        event->accept();
    }
    else
    {
        event->ignore();
    }

}

void MainWindow::saveSetting(std::string fileName)
{
    using namespace std;
    ofstream output(fileName);

    QString str1 = QString("Max X(km),%1,Max Y(km),%2\n").arg(SwitchStatus::getInstance()->getAreaMaxX()).arg(SwitchStatus::getInstance()->getAreaMaxY());
    QString str2 = QString("Connected Distance,%1,Speed(km/h),%2\n").arg(SwitchStatus::getInstance()->getConnectedDistance()).arg(SwitchStatus::getInstance()->getSpeed());
    QString str;
    str.append(str1).append(str2).append("name,port,x,y\n");
    vector<SwitchData> switchesInfo = SwitchStatus::getInstance()->getStaticSwitchesInfo();
    for(auto& s:switchesInfo)
    {
        QString str3 = QString("%1,%2,%3,%4\n").arg(QString::fromStdString(s.name)).arg(s.port).arg(s.x).arg(s.y);
        str.append(str3);
    }

    string outputStr = str.toStdString();
    output.write(outputStr.c_str(), outputStr.size());
    output.close();
}

void MainWindow::actionAbout()
{
    QMessageBox::about(NULL, QStringLiteral("关于"), QStringLiteral("By wdLin @USTC 2015"));
}

void MainWindow::actionSaveSetting()
{
    saveSetting("data.csv");
}
void MainWindow::actionSaveSettingAs()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,QStringLiteral("保存配置文件"), "", QStringLiteral("配置文件 (*.csv)"));
    if(!fileName.isEmpty())
    {
        saveSetting(fileName.toStdString());
    }
    else
    {
    }
}
