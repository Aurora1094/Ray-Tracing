#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <cmath>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//向量类的创建（运算法则）
class Vector3
{
public:
    double x, y, z;

    Vector3() : x(0), y(0), z(0) {}//构造函数，防垃圾值

    Vector3(double newX, double newY,double newZ)
    {
        x = newX;
        y = newY;
        z = newZ;
    }

    //向量加法
    Vector3 operator+(const Vector3 &a)const
    {
        return Vector3(x + a.x, y + a.y, z + a.z);
    }
    //向量减法
    Vector3 operator-(const Vector3& a)const
    {
        return Vector3(x - a.x, y - a.y, z - a.z);
    }
    //向量数乘
    Vector3 operator*(double a)const
    {
        return Vector3(x * a, y * a, z * a);
    }
    //向量数除
    Vector3 operator/(double a)const
    {
        return Vector3(x / a, y / a, z / a);
    }
    //向量点积
    double operator*(const Vector3& a)const
    {
        return ((x * a.x)+(y * a.y)+( z * a.z));
    }
    //向量模长
    static double getlen(const Vector3& a)
    {
        return  sqrt( a * a);
    }
    //向量单位化
    static Vector3 unitization(const Vector3& a)
    {
        return a / getlen(a);
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光屏平面的位置
static double z=500.0;
//摄像机位置(A)
static Vector3 camera(0.0,0.0,0.0);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sphere类的创建
class Sphere
{
public:
    Vector3 center;
    double radius;
    Vector3 color; // 球体颜色

    Sphere() : center(), radius(0.0), color(0, 0, 0) {} // 构造函数，防垃圾值

    Sphere(Vector3 newCenter, double newRadius, Vector3 newColor) :
        center(newCenter), radius(newRadius), color(newColor) {} // 赋值构造函数
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//light（光源）类的建立
struct Light {
    Vector3 position; // 光源位置
    Vector3 color;    // 光源颜色（通常为 RGB 值）
    double intensity; // 光源强度（可选，用于控制光照强度）
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//引入冯氏光照模型 phong(返回计算出的物体表面光照效果颜色)
Vector3 calculatePhongLighting(const Vector3& hitPoint, const Vector3& normal, const Light& light, const Vector3& camera, const Sphere& sphere)
{
    Vector3 mixcol;
    mixcol.x=light.color.x * sphere.color.x;
    mixcol.y=light.color.y * sphere.color.y;
    mixcol.z=light.color.z * sphere.color.z;

    Vector3 ambient = mixcol * 0.15; // 环境光（0.1是环境光系数）

    // 漫反射
    Vector3 lightDir = Vector3::unitization(light.position - hitPoint);
    double diff = std::max(normal * lightDir, 0.0);//漫反射系数
    Vector3 diffuse =mixcol * diff;

    // 镜面反射
    Vector3 viewDir = Vector3::unitization(camera - hitPoint);
    Vector3 reflectDir = Vector3::unitization(normal * 2.0 * (normal * lightDir) - lightDir);
    double spec = std::pow(std::max(viewDir * reflectDir, 0.0), 32);//32是镜面高光指数   spec是镜面发射因子
    Vector3 specular = light.color * 0.5 * spec; //0.5镜面反射系数 （颜色）

    // 合并颜色并限制范围
    Vector3 result = ambient + diffuse + specular;
    result.x = std::clamp(result.x, 0.0, 1.0);
    result.y = std::clamp(result.y, 0.0, 1.0);
    result.z = std::clamp(result.z, 0.0, 1.0);

    // 映射到 0-255
    return result * 255.0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ray类的建立 A+t*B
class Ray
{
public:
    Vector3 A=camera;
    Vector3 B;
    float t;

    Ray() : A(), B(), t(0) {}//构造函数，防垃圾值

    Ray(Vector3 newA, Vector3 newB, float newt) : A(newA), B(newB), t(newt) {
        A = newA;
        B = newB;
        B = Vector3::unitization(B);
        t = newt;
    }
    // Vector3 ProjectivePoint(float t)const//得到射影点
    // {
    //     return A +  B * t;
    // }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 平面地面的光照计算
Vector3 calculateGroundLighting(double y, const Light &light)
{
    Vector3 groundColor(100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0); // 地面颜色
    Vector3 normal(0, 1, 0); // 地面法线始终向上

    Vector3 mixcol;
    mixcol.x = light.color.x * groundColor.x;
    mixcol.y = light.color.y * groundColor.y;
    mixcol.z = light.color.z * groundColor.z;

    Vector3 ambient = mixcol * 0.15; // 环境光

    // 漫反射
    Vector3 lightDir = Vector3::unitization(light.position - Vector3(0, y, z));
    double diff = std::max(normal * lightDir, 0.0);
    Vector3 diffuse = mixcol * diff;

    // 地面没有镜面反射
    Vector3 result = ambient + diffuse;
    result.x = std::clamp(result.x, 0.0, 1.0);
    result.y = std::clamp(result.y, 0.0, 1.0);
    result.z = std::clamp(result.z, 0.0, 1.0);

    return result * 255.0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光线撞击相关函数
//是否撞击的判断
static bool if_Hit(Vector3 center, double radius, Ray ray)
{
    double a = ray.B * ray.B;
    double b = 2.0 * (ray.B * (ray.A-center));
    double c = (ray.A-center) *(ray.A- center) - radius * radius;

    double delta = b * b - 4 * a * c;

    float x1=(-b+sqrt(delta))/(2*a);
    float x2=(-b-sqrt(delta))/(2*a);

    float t=(x1<x2?x1:x2);
    if(t<=0&&(x1>x2?x1:x2)>0)
    {
        t=(x1>x2?x1:x2);
    }

    return (delta >= 0)&&(t>0);
}
//求解撞击法向量，并线性变换得到rgb编码(但是我就不判断撞不撞击了，使用时需要在前面加一个if（bool if_Hit）判断)
static Vector3 where_Hit(Vector3 center, double radius, Ray ray)
{
    ray.B=Vector3:: unitization(ray.B);

    float a = ray.B * ray.B;
    float b = 2.0 * (ray.B * (ray.A-center));
    float c = (ray.A-center) *(ray.A- center) - radius * radius;

    float delta = b * b - 4 * a * c;

    float x1=(-b+sqrt(delta))/(2*a);
    float x2=(-b-sqrt(delta))/(2*a);

    float t=(x1<x2?x1:x2);

    if(t<=0&&(x1>x2?x1:x2)>0)
    {
        t=(x1>x2?x1:x2);
    }

    if(t>=0)
    {
        Vector3 Lawline=ray.A+ray.B*t-center;

        //引入phong应该不需要rgb转化了，这是之前为了便于验证的代码

        // Lawline=Vector3::unitization(Lawline);//单位化

        // Lawline = (Lawline + Vector3(1, 1, 1)) / 2;
        // Lawline = Lawline * 255;//向0~255作线性映射(为了颜色好看，改了一下，可能以后变数据会出问题，注意一下)

        return Lawline;
    }
    else
    {
        Vector3 black=Vector3(0,0,0);
        return black;


        // Vector3 Lawline=ray.A+ray.B*t-center;

        // Lawline=Vector3::unitization(Lawline);//单位化

        // Vector3 Unit(1,1,1);
        // Lawline=(Lawline+Unit)/2*255;//向0~255作线性映射(为了颜色好看，改了一下，可能以后变数据会出问题，注意一下)

        // return Lawline;
    }

}
//多重取样 抗锯齿（采用phong后）【对于地面球】
Vector3 multisample(double x, double y, const Sphere& sphere, const Light& light) {
    const int sample = 2; // 采样数量
    Vector3 sample_color[sample * sample];
    int n = 0;
    int count=0;

    for (int i = 0; i < sample; i++)
    {
        for (int j = 0; j < sample; j++)
        {
            float newx = x - 0.5 + (i + 0.5) / sample;
            float newy = y - 0.5 + (j + 0.5) / sample;

            Ray ray(camera, Vector3(newx, newy, z), 0);

            if (if_Hit(sphere.center, sphere.radius, ray))
            {
                Vector3 hitPoint = where_Hit(sphere.center, sphere.radius, ray);
                Vector3 normal = Vector3::unitization(hitPoint - sphere.center);
                sample_color[n++] = calculatePhongLighting(hitPoint, normal, light, camera, sphere);
            }
            else
            {
                    count++;
                    sample_color[n++] = Vector3(135 + 120 * y / 301, 206 + 50 * y / 301, 255); // 背景颜色

            }

        }
    }



    if(count==sample*sample)
    {
        return Vector3(-1,-1,-1);
    }
    else
    {
        Vector3 average_color = sample_color[0];

        for (int i = 1; i < sample * sample; i++) {
            average_color = average_color + sample_color[i];
        }
        average_color = average_color / (sample * sample);

        // 平均颜色后(ai生成的限制范围的方式)
        average_color.x = std::clamp(average_color.x, 0.0, 255.0);
        average_color.y = std::clamp(average_color.y, 0.0, 255.0);
        average_color.z = std::clamp(average_color.z, 0.0, 255.0);
        return average_color;
    }

// static Vector3 multisample(double x,double y,Sphere R)
// {
//     const int sample=2;
//     Vector3 sample_point[sample*sample];
//     int n=0;
//     for(int i=0;i<sample;i++)
//     {
//         for(int j=0;j<sample;j++)
//         {
//             // float newx=x-0.5+i;
//             // float newy=y-0.5+j;

//             float newx = x - 0.5 + (i + 0.5) / sample;
//             float newy = y - 0.5 + (j + 0.5) / sample;


//             // float newx=x-0.5+4/sample/sample*i;
//             // float newy=y-0.5+4/sample/sample*j;

//             Ray Hit_R(camera, Vector3(newx, newy, z),0);//0是随意赋的值，无实义

//             bool flag=if_Hit(R.center,R.radius,Hit_R);

//             if(flag)
//             {
//                 sample_point[n]=where_Hit(R.center,R.radius,Hit_R);
//                 n++;
//             }
//             else

//             {
//                 double px=(x-camera.x+800/2);
//                 double py=(y-camera.y+600/2);
//                 sample_point[n]=Vector3(135+120*py/601,206+50*py/601,255);
//                 n++;
//             }//对2*2个样本点实现了采集
//         }
//     }//对2*2个样本点实现了采集
//     Vector3 average=sample_point[0];
//     for(int i=1;i<sample*sample;i++)
//     {
//         average=average+sample_point[i];
//     }
//     average=average/sample/sample;

//     //ai搜到的，防止越界的函数
//     average.x = std::clamp((float)average.x, 0.0f, 255.0f);
//     average.y = std::clamp((float)average.y, 0.0f, 255.0f);
//     average.z = std::clamp((float)average.z, 0.0f, 255.0f);

//     return average;
// }
}
//多重取样 抗锯齿（采用phong后）【对于地上球】
Vector3 multisample2(double x, double y, const Sphere& sphere, const Light& light,const Sphere& groundsphere) {
    const int sample = 2; // 采样数量
    Vector3 sample_color[sample * sample];
    int n = 0;
    int count1=0;
    int count2=0;

    for (int i = 0; i < sample; i++)
    {
        for (int j = 0; j < sample; j++)
        {
            float newx = x - 0.5 + (i + 0.5) / sample;
            float newy = y - 0.5 + (j + 0.5) / sample;

            Ray ray(camera, Vector3(newx, newy, z), 0);

            if (if_Hit(sphere.center, sphere.radius, ray))
            {
                Vector3 hitPoint = where_Hit(sphere.center, sphere.radius, ray);
                Vector3 normal = Vector3::unitization(hitPoint - sphere.center);
                sample_color[n++] = calculatePhongLighting(hitPoint, normal, light, camera, sphere);
            }
            else
            {
                if(if_Hit(groundsphere.center, groundsphere.radius, ray))
                {
                    count1++;
                    Vector3 hitPoint = where_Hit(groundsphere.center, groundsphere.radius, ray);
                    Vector3 normal = Vector3::unitization(hitPoint - groundsphere.center);
                    sample_color[n++] = calculatePhongLighting(hitPoint, normal, light, camera, groundsphere);
                }

                else
                {
                    count2++;
                    sample_color[n++] = Vector3(135 + 120 * y / 301, 206 + 50 * y / 301, 255); // 背景颜色
                }
            }

        }
    }


    if(count1==sample*sample||count2==sample*sample)
    {
        return Vector3(-1,-1,-1);
    }
    else
    {
        Vector3 average_color = sample_color[0];

        for (int i = 1; i < sample * sample; i++) {
            average_color = average_color + sample_color[i];
        }
        average_color = average_color / (sample * sample);

        // 平均颜色后(ai生成的限制范围的方式)
        average_color.x = std::clamp(average_color.x, 0.0, 255.0);
        average_color.y = std::clamp(average_color.y, 0.0, 255.0);
        average_color.z = std::clamp(average_color.z, 0.0, 255.0);

        return average_color;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//窗口，输出一张图片
class ImageWidget : public QWidget
{
protected:
    void paintEvent(QPaintEvent *event) override
    {
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


        Light light;
        light.position = Vector3(0,-300, 600);
        light.color = Vector3(255.0 / 255.0, 200.0 / 255.0, 100.0 / 255.0); // 暖色调光源
        light.intensity=500.0;

        // 建立地面球R0（
        Sphere R0(Vector3(0, 1800, z), 1800.0, Vector3(100.0/255.0, 100.0/255.0, 100.0/255.0));

        for (int y = 0; y < image.height(); ++y)
        {
            for (int x = 0; x < image.width(); ++x)
            {
                // 世界坐标系转化
                double true_x = (double)camera.x - (double)(image.width() - 1)/2.0 + (double)x;
                double true_y = (double)camera.y - (double)(image.height() - 1)/2.0 + (double)y;

                Vector3 color0 = multisample(true_x, true_y, R0, light);
                if(color0.x != -1 && color0.y != -1 && color0.z != -1)
                {
                    image.setPixel(x, y, QColor(color0.x, color0.y, color0.z).rgb());
                }
            }
        }

        // // 地面颜色计算
        // for (int y =image.height()*3/5; y <image.height() ; ++y)
        // {
        //     Vector3 groundColor = calculateGroundLighting(y, light);
        //     for (int x = 0; x < image.width(); ++x)
        //     {
        //         image.setPixel(x, y, QColor(groundColor.x, groundColor.y, groundColor.z).rgb());
        //     }
        // }


        //建立球R1
        Sphere R1(Vector3(0, 0, z), 50.0, Vector3(0.0/255.0, 255.0/255.0, 0.0/255.0)); // 绿色球

        for (int y = 0; y <image.height() ; y++)
        {
            for (int x =0 ; x <image.width() ; x++)
            {
                //世界坐标系转化
                double true_x=(double)camera.x-(double)(image.width() - 1)/2.0f+(double)x;
                double true_y=(double)camera.y-(double)(image.height() - 1)/2.0f+(double)y;

                // Ray Hit_R1(camera,Vector3(true_x,true_y,z),0);//0是随意赋的值，无实义
                // bool flag=if_Hit(R1.center,R1.radius,Hit_R1);
                // if(flag)
                // {
                //     Vector3 point=multisample(true_x,true_y,R1);

                //     QColor color2(point.x,point.y,point.z);

                Vector3 color = multisample2(true_x, true_y, R1, light,R0);
                if(color.x!=-1&&color.y!=-1&&color.z!=-1)
                {
                    image.setPixel(x, y, QColor(color.x, color.y, color.z).rgb());//渲染球体
                }
            }

        }
        // 绘制图像到窗口
        painter.drawImage(0, 0, image);

    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ImageWidget widget;
    widget.resize(401, 301);// 设置窗口大小
    widget.show();

    return app.exec();

}
