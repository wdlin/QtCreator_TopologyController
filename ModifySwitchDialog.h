#ifndef MODIFYSWITCHDIALOG_H
#define MODIFYSWITCHDIALOG_H

#include <QDialog>
#include "SwitchData.h"

class ModifySwitchThread;
namespace Ui {
class ModifySwitchDialog;
}
//修改交换机信息的对话框
class ModifySwitchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifySwitchDialog(QWidget *parent = 0);
    ~ModifySwitchDialog();
    void setOriginalSwitchData(const SwitchData& sd);//设置原始的交换机信息
    ModifySwitchThread* modifySwitchThread;//修改交换机信息的线程
signals:
    void updateUIListWidget(SwitchData& sd);//更新主窗口交换机列表信息的信号
private slots:

    void on_cancelButton_clicked();//取消按钮的响应槽
    void on_modifySwitchButton_clicked();//修改按钮的响应槽

private:
    Ui::ModifySwitchDialog *ui;//
    SwitchData originSwitchData;//修改前的交换机信息

};

#endif // MODIFYSWITCHDIALOG_H
