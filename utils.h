#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <vector>
#include <limits>
#include<algorithm>

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

enum MaterialType
{
    MATTE=0,       // 哑光
    METAL,       // 金属
    FABRIC,      // 布料
    POLISHED     // 抛光
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sphere类的创建
class Sphere
{
public:
    Vector3 center;
    double radius;
    Vector3 color;          // 球体颜色
    MaterialType materialType;  // 材质类型
    bool hasTexture;        // 是否有纹理
    Vector3 textureColor;   // 纹理颜色（条纹颜色）
    double reflectivity;    // 反射率

    Sphere() : center(), radius(0.0), color(0, 0, 0), materialType(MATTE), hasTexture(false), textureColor(0, 0, 0) {}

    Sphere(Vector3 newCenter, double newRadius, Vector3 newColor, MaterialType newMaterialType = MATTE, bool newHasTexture = false, Vector3 newTextureColor = Vector3(0, 0, 0), double reflect = 0.0)
        : center(newCenter), radius(newRadius), color(newColor), materialType(newMaterialType), hasTexture(newHasTexture), textureColor(newTextureColor), reflectivity(reflect) {}
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//light（光源）类的建立
struct Light {
    Vector3 position; // 光源位置
    Vector3 color;    // 光源颜色（通常为 RGB 值）
    double intensity; // 光源强度（可选，用于控制光照强度）
};
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
//是否撞击的判断
static bool if_Hit(Vector3 center, double radius, Ray ray);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 检查点是否在阴影中
bool isInShadow(const Vector3& hitPoint, const Light& light, const std::vector<Sphere>& spheres, bool isReflection = false);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//引入冯氏光照模型 phong(返回计算出的物体表面光照效果颜色)
Vector3 calculatePhongLighting(const Vector3& hitPoint, const Vector3& normal, const Light& light,const Vector3& camera, const Sphere& sphere, const std::vector<Sphere>& spheres,bool isReflection);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光线撞击相关函数
//是否撞击的判断
static bool if_Hit(Vector3 center, double radius, Ray ray);
//求解撞击点，并线性变换得到rgb编码(但是我就不判断撞不撞击了，使用时需要在前面加一个if（bool if_Hit）判断)
static Vector3 where_Hit(Vector3 center, double radius, Ray ray);
// 递归光线追踪实现全局光照
Vector3 traceRay(const Ray &ray, const std::vector<Sphere> &spheres, const Light &light, int depth = 0);
// 多重取样 抗锯齿
Vector3 multisample(double x, double y, const std::vector<Sphere> &spheres, const Light &light);

#endif // UTILS_H
