#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<SwitchData>("SwitchData");
    qRegisterMetaType<SwitchData>("SwitchData&");
    qRegisterMetaType<std::vector<SwitchData>>("std::vector<SwitchData>");
    qRegisterMetaType<std::vector<SwitchData>>("std::vector<SwitchData>&");
    MainWindow w;  
    w.init();
    w.show();
    return a.exec();
}
