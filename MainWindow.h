#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "SwitchData.h"
class QAction;
class DeleteSwitchThread;
class AddSwitchThread;
class ModifySwitchDialog;
class RunSwitchThread;
class SerialPortThread;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();//初始化函数
    void initUI();//初始化一些UI的属性
    QString getCommandStringFromSwitches();//获取交换机的状态并转化成字符串指令
    void closeEvent(QCloseEvent *event);//关闭响应函数
    void saveSetting(std::string fileName);//保存配置
    void readSetting(std::string fileName);//读取配置

private slots:
    void on_addSwitchButton_clicked();//添加交换机按钮响应槽
    void on_setMaxXButton_clicked();//设置maxX按钮响应槽
    void on_setMaxYButton_clicked();//设置maxY按钮响应槽
    void on_setConnectedDistanceButton_clicked();//设置连接距离按钮响应槽
    void on_setSpeedButton_clicked();//设置运动速度响应槽
    void on_onceConfigureButton_clicked();//静态配置按钮响应槽
    void on_dynamicConfigureButton_clicked();//动态配置按钮响应槽
    void on_runSwithButton_clicked();//运动交换机按钮响应槽
    void on_modifySwitchButton_clicked();//修改交换机按钮响应槽
    void on_deleteSwitchButton_clicked();//删除交换机按钮响应槽
    void on_onOffSerialPortButton_clicked();//开关串口按钮响应槽

    void updateAnimation(std::vector<SwitchData> switchesInfo);//动态配置中，更新动画、列表以及下发命令
    void updateListWidget(SwitchData& sd);//更新交换机列表的数据
    void updateStaticAnimation(std::vector<SwitchData> switchesInfo);//更新动画
    void serialPortOpenStatus(int status);//串口状态事件响应槽

    void actionAbout();//关于菜单
    void actionSaveSetting();//保存配置菜单
    void actionSaveSettingAs();//配置另存为菜单

private:
    Ui::MainWindow *ui;//
    DeleteSwitchThread* deleteSwitchThread;//删除交换机线程
    AddSwitchThread* addSwitchThread;//增加交换机线程
    ModifySwitchDialog* modifySwitchDialog;//修改交换机对话框
    RunSwitchThread* runSwitchThread;//运动交换机线程
    SerialPortThread* serialPortThread;//串口线程
};

#endif // MAINWINDOW_H
