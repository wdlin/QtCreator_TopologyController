#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <vector>
#include "SwitchData.h"
//显示交换机动画的Widget
class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = 0);
    ~PaintWidget();
    void setPaintData(std::vector<SwitchData> si);//设置要绘制的交换机信息向量
protected:
    void paintEvent(QPaintEvent *event);//绘制函数
signals:

public slots:

private:
    std::vector<SwitchData> switchesInfo;//绘制的交换机信息向量
};

#endif // PAINTWIDGET_H
