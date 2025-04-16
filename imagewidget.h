#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QColor>
#include"utils.h"
#include<mutex>
class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    ImageWidget(QWidget*parent=nullptr);
    Light light;
    std::vector<Sphere> spheres;
    void freshParam(Light l,std::vector<Sphere> s);
    bool flag=false;
protected:
    void paintEvent(QPaintEvent *event) override;
    std::mutex mtx;
};


#endif // IMAGEWIDGET_H
