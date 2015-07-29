#include "ModifySwitchDialog.h"
#include "ui_modifyswitchdialog.h"
#include "MainWindow.h"
#include "SwitchData.h"
#include "RunningStatus.h"
#include "SwitchStatus.h"
#include "ModifySwitchThread.h"
#include <QMessageBox>
ModifySwitchDialog::ModifySwitchDialog(QWidget *parent) :
    QDialog(parent),modifySwitchThread(new ModifySwitchThread()),
    ui(new Ui::ModifySwitchDialog)
{
    ui->setupUi(this);

    //设置输入的格式
    QRegExp regExp("[A-Za-z][A-Za-z0-9]*");//以字母开头，只能输入字母和数字
    ui->switchNameLineEdit->setValidator(new QRegExpValidator(regExp));
    ui->switchXLineEdit->setValidator(new QDoubleValidator);//后面这些都是只能输入浮点数格式
    ui->switchYLineEdit->setValidator(new QDoubleValidator);

    setWindowFlags(Qt::WindowCloseButtonHint);
    //connect(modifySwitchThread,SIGNAL(updateUIAnimation(std::vector<SwitchData>)),this,SLOT(updateAnimation(std::vector<SwitchData>)), Qt::QueuedConnection);
}

ModifySwitchDialog::~ModifySwitchDialog()
{
    delete ui;
    delete modifySwitchThread;
}
//ModifySwitchDialog::switchData = SwitchData();
void ModifySwitchDialog::setOriginalSwitchData(const SwitchData &sd)
{
    originSwitchData = sd;
    ui->switchNameLineEdit->setText(QString::fromStdString(sd.name));
    ui->switchXLineEdit->setText(QString::number(sd.x));
    ui->switchYLineEdit->setText(QString::number(sd.y));
}

void ModifySwitchDialog::on_cancelButton_clicked()
{
    this->hide();
}

void ModifySwitchDialog::on_modifySwitchButton_clicked()
{
    std::string name = ui->switchNameLineEdit->text().trimmed().toStdString();
    if(name.compare("")==0)
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机名字不能为空！"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if(name.compare(originSwitchData.name) != 0)
    {
        if(SwitchStatus::getInstance()->isNameUsed(name))
        {
            QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("交换机名字已经被占用!"), QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }

    double x = ui->switchXLineEdit->text().toDouble();
    if(x < 0 || x > SwitchStatus::getInstance()->getAreaMaxX())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("X坐标不能为负数或者大于区域Max X!"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    double y = ui->switchYLineEdit->text().toDouble();
    if(y < 0 || y > SwitchStatus::getInstance()->getAreaMaxY())
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("Y坐标不能为负数或者大于区域Max Y!"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    SwitchData sd;
    sd.name = name;
    sd.port = originSwitchData.port;
    sd.x = x;
    sd.y = y;
    modifySwitchThread->setModifySwitchData(originSwitchData.name,sd);
    modifySwitchThread->start();

    this->hide();
    emit updateUIListWidget(sd);
}
