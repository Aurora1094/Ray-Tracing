#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <cmath>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
};



enum MaterialType {
    MATTE,
    METAL,
    FABRIC,
    POLISHED
};

class Vector3 {
public:
    double x, y, z;

    Vector3();
    Vector3(double newX, double newY, double newZ);

    Vector3 operator+(const Vector3 &a) const;
    Vector3 operator-(const Vector3 &a) const;
    Vector3 operator*(double a) const;
    Vector3 operator/(double a) const;
    double operator*(const Vector3 &a) const;
    static double getlen(const Vector3 &a);
    static Vector3 unitization(const Vector3 &a);
};

struct Light {
    Vector3 position;
    Vector3 color;
    double intensity;
};

class Ray {
public:
    Vector3 A;
    Vector3 B;
    float t;

    Ray();
    Ray(Vector3 newA, Vector3 newB, float newt);
};

class Sphere {
public:
    Vector3 center;
    double radius;
    Vector3 color;
    MaterialType materialType;
    bool hasTexture;
    Vector3 textureColor;
    double reflectivity;

    Sphere();
    Sphere(Vector3 newCenter, double newRadius, Vector3 newColor,
           MaterialType newMaterialType = MATTE, bool newHasTexture = false,
           Vector3 newTextureColor = Vector3(0, 0, 0), double reflect = 0.0);
};



class ImageWidget : public QWidget
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *event) override;
};




extern double z;
extern Vector3 camera;

bool if_Hit(Vector3 center, double radius, Ray ray);
Vector3 where_Hit(Vector3 center, double radius, Ray ray);
bool isInShadow(const Vector3& hitPoint, const Light& light,
                const std::vector<Sphere>& spheres, bool isReflection = false);
Vector3 calculatePhongLighting(const Vector3& hitPoint, const Vector3& normal,
                               const Light& light, const Vector3& camera,
                               const Sphere& sphere, const std::vector<Sphere>& spheres,
                               bool isReflection);
Vector3 traceRay(const Ray &ray, const std::vector<Sphere> &spheres,
                 const Light &light, int depth = 0);
Vector3 multisample(double x, double y, const std::vector<Sphere> &spheres,
                    const Light &light);

#endif // WIDGET_H
