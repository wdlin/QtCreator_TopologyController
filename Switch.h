#ifndef SWITCH_H
#define SWITCH_H
//交换机类
class Switch
{
public:
    Switch(int mp,double mx,double my);//构造函数，输入端口号、x坐标、y坐标
    Switch();
    ~Switch();
    int port;//端口号
    double x;//x坐标
    double y;//y坐标
};


#endif // SWITCH

