#include "imagewidget.h"
#include"utils.h"

//////////////////////////////////////////////////////////////////////////////////////
//窗口，输出一张图片
//QImage image(401, 301, QImage::Format_RGB32);//修改图片大小的时候记得回到multisample更改一下
ImageWidget::ImageWidget(QWidget *parent):QWidget(parent){

    light.position = Vector3(0, -200, 400);  // 光源位置
    light.color = Vector3(1.0, 1.0, 1.0);   // 白色光源
    light.intensity = 800.0;                // 更高强度
    // 建立地面球R0
    Sphere R0(
        Vector3(0, 1850, z),
        1800.0,
        Vector3(0.5,0.5,0.5),  // 基础灰色
        FABRIC,             // 布料材质
        true,
        Vector3(1.0,1.0,1.0),            // 纹理白色
        0.1
        );

    //建立球R1
    // 绿色金属球
    Sphere R1(
        Vector3(-70, -50, z),
        50.0,
        Vector3(0 / 255.0, 255.0 / 255.0, 0 / 255.0),  // 纯绿色（RGB: 0,255,0）
        METAL,                   // 金属材质
        false,                  // 关闭纹理（关键修改）
        Vector3(1.0,1.0,1.0),   //占位，无意义
        0.8
        );

    // 后景球R2
    //红色哑光球
    Sphere R2(
        Vector3(100, 0, z - 150),  // 位于z轴更近位置（前方）
        85.0,
        Vector3(1.0, 0.0, 0.0),  // 红色
        MATTE,
        false,
        Vector3(1.0,1.0,1.0),   //占位，无意义
        0.3
        );

    spheres = std::vector({R0, R1,R2});
}

void ImageWidget::freshParam(Light l, std::vector<Sphere> s)
{
    mtx.lock();
    light=l;
    spheres=s;
    mtx.unlock();
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    if(!flag)
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::black); // 填充整个小部件为黑色
        return;
    }
    mtx.lock();
    QPainter painter(this);

    // 创建一个 401x301 的 QImage，并初始化为白色
    QImage image(401, 301, QImage::Format_RGB32);//修改图片大小的时候记得回到multisample更改一下
    image.fill(Qt::white);  // 可选：设置初始背景颜色

    // 将每个像素点设置为渐变青
    for (int y = 0; y <image.height() ; ++y)
    {
        for (int x =0 ; x <image.width() ; ++x)
        {
            QColor color(135+120*y/image.height(),206+50*y/image.height(),255);  // 渐变青
            image.setPixel(x, y, color.rgb());
        }
    }


    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            // 世界坐标系转化
            double true_x = (double)camera.x - (double)(image.width() - 1)/2.0 + (double)x;
            double true_y = (double)camera.y - (double)(image.height() - 1)/2.0 + (double)y;

            Vector3 color = multisample(true_x, true_y, spheres, light);

            if(color.x != -1 )
            {
                image.setPixel(x, y, QColor(color.x, color.y, color.z).rgb());
            }

        }
    }

    // 绘制图像到窗口
    painter.drawImage(0, 0, image);

    mtx.unlock();
}










