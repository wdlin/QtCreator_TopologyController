#include "PaintWidget.h"
#include <QPainter>
#include "SwitchStatus.h"
PaintWidget::PaintWidget(QWidget *parent) : QWidget(parent)
{
}

PaintWidget::~PaintWidget()
{

}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QColor(240,240,240));
    painter.drawRect(0,0,width()-1,height()-1);
    //画区域
    painter.setBrush(QColor(38,168,240));
    double width = this->width();
    double height = this->height();
    double maxX = SwitchStatus::getInstance()->getAreaMaxX();
    double maxY = SwitchStatus::getInstance()->getAreaMaxY();
    double connectedDistance = SwitchStatus::getInstance()->getConnectedDistance();
    double drawWidth;
    double drawHeight;
    double drawDistance;
    double offsetX;
    double offsetY;
    if(maxX/maxY > width/height)
    {
        drawWidth = width;
        drawHeight = width*maxY / maxX;
        drawDistance = width*connectedDistance / maxX;
        offsetX = 0;
        offsetY = (height - drawHeight)/2;
    }
    else
    {
        drawHeight = height;
        drawWidth = height*maxX/maxY;
        drawDistance = height*connectedDistance / maxY;
        offsetY = 0;
        offsetX = (width - drawWidth)/2;
    }
    painter.drawRect((width - drawWidth)/2, (height - drawHeight)/2, drawWidth, drawHeight);

    if(switchesInfo.empty())
    {
    }
    else
    {
       //painter.setBrush(QColor(38,255,124));
        painter.setBrush(Qt::black);
        painter.setBrush(Qt::NoBrush);
       for(auto& sd:switchesInfo)
       {
           double cx = sd.x*drawWidth / maxX + offsetX;
           double cy = sd.y*drawHeight / maxY + offsetY;
           painter.drawEllipse(QPoint(cx,cy),(int)(drawDistance/2),(int)(drawDistance/2));
       }
       painter.setBrush(Qt::black);

       for(auto& sd:switchesInfo)
       {
           double cx = sd.x*drawWidth / maxX + offsetX;
           double cy = sd.y*drawHeight / maxY + offsetY;
           painter.drawText(QPoint(cx,cy),QString::fromStdString(sd.name));
       }
    }

}

void PaintWidget::setPaintData(std::vector<SwitchData> si)
{
    switchesInfo = si;
}
