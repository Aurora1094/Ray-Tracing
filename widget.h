#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"utils.h"
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
    void modify(Sphere R);
    void update1(int index);
private slots:
    void on_comboBox_3_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    int preIndex=0;
    int nowIndex=0;
    Sphere R1;
    Sphere R2;
    Sphere R3;
    Ui::Widget *ui;
};
#endif // WIDGET_H
