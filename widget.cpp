#include "widget.h"
#include "./ui_widget.h"
#include <QApplication>
Widget::~Widget() {}
Widget::Widget(QWidget *parent) : QWidget(parent)
{
    // 构造函数的实现
}
// 全局变量定义
double z = 500.0;
Vector3 camera(0.0, 0.0, 0.0);

// Vector3 成员函数实现
Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(double newX, double newY, double newZ) : x(newX), y(newY), z(newZ) {}

Vector3 Vector3::operator+(const Vector3 &a) const {
    return Vector3(x + a.x, y + a.y, z + a.z);
}

Vector3 Vector3::operator-(const Vector3 &a) const {
    return Vector3(x - a.x, y - a.y, z - a.z);
}

Vector3 Vector3::operator*(double a) const {
    return Vector3(x * a, y * a, z * a);
}

Vector3 Vector3::operator/(double a) const {
    return Vector3(x / a, y / a, z / a);
}

double Vector3::operator*(const Vector3 &a) const {
    return x * a.x + y * a.y + z * a.z;
}

double Vector3::getlen(const Vector3 &a) {
    return sqrt(a * a);
}

Vector3 Vector3::unitization(const Vector3 &a) {
    return a / getlen(a);
}

// Ray 成员函数实现
Ray::Ray() : A(), B(), t(0) {}
Ray::Ray(Vector3 newA, Vector3 newB, float newt) :
    A(newA), B(Vector3::unitization(newB)), t(newt) {}

// Sphere 成员函数实现
Sphere::Sphere() : center(), radius(0.0), color(0, 0, 0),
    materialType(MATTE), hasTexture(false),
    textureColor(0, 0, 0) {}

Sphere::Sphere(Vector3 newCenter, double newRadius, Vector3 newColor,
               MaterialType newMaterialType, bool newHasTexture,
               Vector3 newTextureColor, double reflect) :
    center(newCenter), radius(newRadius), color(newColor),
    materialType(newMaterialType), hasTexture(newHasTexture),
    textureColor(newTextureColor), reflectivity(reflect) {}

// 碰撞检测函数实现
bool if_Hit(Vector3 center, double radius, Ray ray) {
    double a = ray.B * ray.B;
    double b = 2.0 * (ray.B * (ray.A - center));
    double c = (ray.A - center) * (ray.A - center) - radius * radius;
    double delta = b * b - 4 * a * c;

    if (delta < 0) return false;

    double t1 = (-b + std::sqrt(delta)) / (2 * a);
    double t2 = (-b - std::sqrt(delta)) / (2 * a);
    double t = std::min(t1, t2);

    return t > 0 || (t1 > 0 || t2 > 0);
}

Vector3 where_Hit(Vector3 center, double radius, Ray ray) {
    double a = ray.B * ray.B;
    double b = 2.0 * (ray.B * (ray.A - center));
    double c = (ray.A - center) * (ray.A - center) - radius * radius;
    double delta = b * b - 4 * a * c;

    if (delta < 0) return Vector3(-1, -1, -1);

    double t1 = (-b + std::sqrt(delta)) / (2 * a);
    double t2 = (-b - std::sqrt(delta)) / (2 * a);
    double t = std::max(t1, t2);

    if (t < 0) t = std::min(t1, t2);
    if (t < 0) return Vector3(-1, -1, -1);

    return ray.A + ray.B * t;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 检查点是否在阴影中
bool isInShadow(const Vector3& hitPoint, const Light& light, const std::vector<Sphere>& spheres, bool isReflection)
{
    Vector3 lightDir = Vector3::unitization(light.position - hitPoint);
    Ray shadowRay(hitPoint + lightDir * 1e-3, lightDir, 0);
    for (const auto& sphere : spheres) {
        // 排除自身球体的阴影检测（避免误判自身导致阴影）
        Vector3 t=hitPoint - lightDir * 1e-3;
        if (sphere.center.x ==t.x&&sphere.center.y ==t.y&&sphere.center.z ==t.z ) continue;

        // 若为反射追踪且是地面球，跳过阴影检测
        if (isReflection && sphere.materialType == FABRIC) continue;

        if (if_Hit(sphere.center, sphere.radius, shadowRay)) {
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//引入冯氏光照模型 phong(返回计算出的物体表面光照效果颜色)
Vector3 calculatePhongLighting(const Vector3& hitPoint, const Vector3& normal, const Light& light,const Vector3& camera, const Sphere& sphere, const std::vector<Sphere>& spheres,bool isReflection) {
    // 全局光照系数
    const double globalLightCoefficient = 0.1;

    // 混合颜色
    Vector3 mixcol;
    mixcol.x = light.color.x * sphere.color.x;
    mixcol.y = light.color.y * sphere.color.y;
    mixcol.z = light.color.z * sphere.color.z;

    // 条纹纹理
    if (sphere.hasTexture && sphere.materialType != FABRIC) {
        double stripe = std::fmod(hitPoint.y, 30); // 假设条纹周期为20
        if (stripe < 15) { // 一半区域为纹理颜色
            mixcol = sphere.textureColor;
        } else {
            mixcol = sphere.color; // 另一半区域为球体颜色
        }
    }

    // 地面球FABRIC材质处理部分
    if (sphere.materialType == FABRIC) {
        const double gridSize = 80.0;  // 格子尺寸
        Vector3 localPos = hitPoint - sphere.center;  // 转换为局部坐标

        // 使用周期函数处理正负坐标
        double gridX = localPos.x - gridSize * std::floor(localPos.x / gridSize);
        double gridZ = localPos.z - gridSize * std::floor(localPos.z / gridSize);

        // 判断是否在格子前半部分
        bool xPart = (gridX < gridSize / 2);
        bool zPart = (gridZ < gridSize / 2);

        // XOR操作生成棋盘格
        bool isPattern = (xPart == zPart);

        mixcol = isPattern ?
                     Vector3(0.95, 0.85, 0.75) :  // 米白色
                     Vector3(0.3, 0.2, 0.1);      // 深棕色
    }

    // 全局光照
    Vector3 globalLight = mixcol * globalLightCoefficient;

    // 环境光
    Vector3 ambient;
    switch (sphere.materialType) {
    case FABRIC:
        ambient = mixcol * 0.02 + globalLight; // 降低布料的环境光强度
        break;
    default:
        ambient = mixcol * 0.1 + globalLight;
        break;
    }

    // 漫反射
    Vector3 lightDir = Vector3::unitization(light.position - hitPoint);
    double diff = std::max(normal * lightDir, 0.0);
    Vector3 diffuse = mixcol * diff;

    // 镜面反射
    Vector3 viewDir = Vector3::unitization(camera - hitPoint);
    Vector3 reflectDir = Vector3::unitization(normal * 2.0 * (normal * lightDir) - lightDir);
    double spec = std::pow(std::max(viewDir * reflectDir, 0.0), 32);
    Vector3 specular = light.color * 0.5 * spec;

    // 检查是否在阴影中，传递isReflection
    bool inShadow = isInShadow(hitPoint, light, spheres, isReflection);

    // 根据材质类型调整光照效果
    switch (sphere.materialType) {
    case MATTE:  // 哑光：高漫反射，低镜面反射
        diffuse = diffuse * 0.9;  // 增强漫反射
        specular = specular * 0.1;  // 减弱镜面反射
        ambient = mixcol * 0.15 + globalLight;  // 中等环境光
        break;
    case METAL:  // 金属：低漫反射，高镜面反射
        diffuse = diffuse * 0.1;  // 减弱漫反射
        specular = specular * 3.0;  // 增强镜面反射
        ambient = mixcol * 0.05 + globalLight;  // 低环境光
        break;
    case FABRIC: // 布料：高漫反射，低镜面反射，柔和颜色
        if (inShadow) {
            diffuse = diffuse * 0.02; // 阴影区域漫反射降低至2%
        } else {
            diffuse = diffuse * 2.5;  // 光照区域漫反射增强至250%
        }
        ambient = mixcol * 0.005 + globalLight; // 环境光进一步降低
        break;
    case POLISHED: // 抛光：中等漫反射，高镜面反射
        specular = specular * 1.5;  // 增强镜面反射
        ambient = ambient + globalLight;
        break;
    }

    // 合并颜色并限制范围
    Vector3 result = ambient + diffuse + specular;
    result.x = std::clamp(result.x, 0.0, 1.0);
    result.y = std::clamp(result.y, 0.0, 1.0);
    result.z = std::clamp(result.z, 0.0, 1.0);

    return result * 255.0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 递归光线追踪实现全局光照
Vector3 traceRay(const Ray &ray, const std::vector<Sphere> &spheres, const Light &light, int depth ) {
    const int MAX_DEPTH = 3;
    if (depth > MAX_DEPTH) return Vector3(0, 0, 0);

    double closestDist = std::numeric_limits<double>::max();
    const Sphere *closestSphere = nullptr;
    Vector3 hitPoint;
    Vector3 normal;

    // 找到最近的相交球体
    for (const auto &sphere : spheres)
    {
        if (if_Hit(sphere.center, sphere.radius, ray))
        {
            Vector3 currentHit = where_Hit(sphere.center, sphere.radius, ray);
            double dist = Vector3::getlen(currentHit - ray.A);
            if (dist < closestDist)
            {
                closestDist = dist;
                closestSphere = &sphere;
                hitPoint = currentHit;
                normal = Vector3::unitization(hitPoint - closestSphere->center);
            }
        }
    }

    if (!closestSphere) {
        return Vector3(135, 206, 255); // 背景颜色
    }

    // 计算直接光照，传递isReflection参数
    bool isReflection = (depth > 0);
    Vector3 directLight = calculatePhongLighting(hitPoint, normal, light, camera, *closestSphere, spheres, isReflection);

    // 修复反射光线的阴影关联问题
    if (closestSphere->materialType == FABRIC && depth == 0) {
        return directLight; // 地面球不递归反射（避免倒影错误阴影）
    }

    Vector3 reflectDir = ray.B - normal * 2.0 * (ray.B * normal);
    Ray reflectRay(hitPoint + normal * 1e-4, reflectDir, 0);
    Vector3 reflectedLight = traceRay(reflectRay, spheres, light, depth + 1) * closestSphere->reflectivity;

    if (closestSphere->materialType == FABRIC) {
        reflectedLight = reflectedLight * 0.05;
    }

    return directLight + reflectedLight;
}
// 多重取样 抗锯齿
Vector3 multisample(double x, double y, const std::vector<Sphere> &spheres, const Light &light) {
    const int sample = 2;
    Vector3 sample_color[sample * sample];
    int n = 0;

    for (int i = 0; i < sample; i++) {
        for (int j = 0; j < sample; j++) {
            double newx = x - 0.5 + (i + 0.5) / sample;
            double newy = y - 0.5 + (j + 0.5) / sample;
            Ray ray(camera, Vector3(newx, newy, z), 0);
            sample_color[n++] = traceRay(ray, spheres, light);
        }
    }

    Vector3 average_color = sample_color[0];
    for (int i = 1; i < sample * sample; i++) {
        average_color = average_color + sample_color[i];
    }
    average_color = average_color / (sample * sample);

    average_color.x = std::clamp(average_color.x, 0.0, 255.0);
    average_color.y = std::clamp(average_color.y, 0.0, 255.0);
    average_color.z = std::clamp(average_color.z, 0.0, 255.0);

    return average_color;
}
//////////////////////////////////////////////////////////////////////////////////////
//窗口，输出一张图片
void ImageWidget::paintEvent(QPaintEvent *event)
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
    light.position = Vector3(0, -200, 400);  // 光源位置上移，增强方向性
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
        Vector3(0.0 / 255.0, 255.0 / 255.0, 0.0 / 255.0),  // 纯绿色（RGB: 0,255,0）
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

    std::vector<Sphere> spheres = {R0, R1,R2};

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

}















