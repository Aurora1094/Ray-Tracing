#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->doubleSpinBox_12->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_13->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_14->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_25->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_26->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_27->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_16->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_17->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_18->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_10->setRange(-10000.0, 10000.0);
   // ui->doubleSpinBox_15->setRange(-1000.0, 1000.0);
    ui->doubleSpinBox_3->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_19->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_20->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_21->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_22->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_23->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_24->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_7->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_8->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_9->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_4->setRange(-10000.0, 10000.0);
    ui->doubleSpinBox_7->setValue(camera.x);
    ui->doubleSpinBox_8->setValue(camera.y);
    ui->doubleSpinBox_9->setValue(camera.z);
    //光源位置
    ui->doubleSpinBox_12->setValue(0);
    ui->doubleSpinBox_13->setValue(-200);
    ui->doubleSpinBox_14->setValue(400);
    //光强
    ui->doubleSpinBox_10->setValue(800);
    //光种类
    ui->comboBox_2->setCurrentIndex(1);

    //颜色
    ui->doubleSpinBox_16->setValue(1);
    ui->doubleSpinBox_17->setValue(1);
    ui->doubleSpinBox_18->setValue(1);
    //反射率
    ui->doubleSpinBox_3->setValue(0);
    R1=Sphere (
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
    R2=Sphere(
        Vector3(100, 0, z - 150),  // 位于z轴更近位置（前方）
        85.0,
        Vector3(1.0, 0.0, 0.0),  // 红色
        MATTE,
        false,
        Vector3(1.0,1.0,1.0),   //占位，无意义
        0.3
        );
    modify(R1);
    R3=Sphere (
        Vector3(0, 1850, z),
        1800.0,
        Vector3(0.5,0.5,0.5),  // 基础灰色
        FABRIC,             // 布料材质
        true,
        Vector3(1.0,1.0,1.0),            // 纹理白色
        0.1
        );



}

Widget::~Widget()
{
    delete ui;
}

void Widget::modify(Sphere R)
{
    //颜色
    ui->doubleSpinBox_19->setValue(R.color.x);
    ui->doubleSpinBox_20->setValue(R.color.y);
    ui->doubleSpinBox_21->setValue(R.color.z);
    //材质
    ui->comboBox->setCurrentIndex((int)R.materialType);
    //是否有纹理
    ui->checkBox->setChecked(R.hasTexture);
    //纹理颜色
    ui->doubleSpinBox_22->setValue(R.textureColor.x);
    ui->doubleSpinBox_23->setValue(R.textureColor.y);
    ui->doubleSpinBox_24->setValue(R.textureColor.z);
    //反射率
    ui->doubleSpinBox_3->setValue(R.reflectivity);
    ui->doubleSpinBox_4->setValue(R.radius);
    ui->doubleSpinBox_25->setValue(R.center.x);
    ui->doubleSpinBox_26->setValue(R.center.y);
    ui->doubleSpinBox_27->setValue(R.center.z);
}

void Widget::update1(int index)
{
    Vector3 color1;
    color1.x=ui->doubleSpinBox_19->value();
    color1.y=ui->doubleSpinBox_20->value();
    color1.z=ui->doubleSpinBox_21->value();

    // 材质类型
    int materialType = ui->comboBox->currentIndex();

    // 是否有纹理
    bool hasTexture = ui->checkBox->isChecked();

    // 纹理颜色
    Vector3 color;
    color.x=ui->doubleSpinBox_22->value();
    color.y=ui->doubleSpinBox_23->value();
    color.z=ui->doubleSpinBox_24->value();
    // 反射率
    double reflectivity = ui->doubleSpinBox_3->value();
    //位置
    double x=ui->doubleSpinBox_25->value();
    double y=ui->doubleSpinBox_26->value();
    double z=ui->doubleSpinBox_27->value();
    Sphere R;
    R.color=color1;
    R.textureColor=color;
    R.hasTexture=hasTexture;
    //反射率
    R.reflectivity=reflectivity;
    R.materialType=(MaterialType)materialType;
    //半径
    R.radius=ui->doubleSpinBox_4->value();
    R.center=Vector3(x,y,z);
    switch(index){
    case 0:{//球一
        R1=R;
        break;
    }
    case 1:{//球二
        R2=R;
        break;
    }
    case 2:{//地面
        R3=R;
        break;
    }
    default:{
        break;
    }
    }
}

void Widget::on_comboBox_3_currentIndexChanged(int index)
{
    preIndex=nowIndex;
    nowIndex=index;
    update1(preIndex);
    switch(index){
    case 0:{//球一
        modify(R1);
        break;
    }
    case 1:{//球二
        modify(R2);
        break;
    }
    case 2:{//地面
        modify(R3);
        break;
    }
    default:{

        break;
    }
    }
}

//渲染
void Widget::on_pushButton_clicked()
{
    ui->widget_5->flag=true;
    update1(nowIndex);
    Light l;
    //光源位置
    double light_x=ui->doubleSpinBox_12->value(),light_y=ui->doubleSpinBox_13->value(),light_z=ui->doubleSpinBox_14->value();
    l.position=Vector3(light_x,light_y,light_z);
    //光强
    l.intensity=ui->doubleSpinBox_10->value();
    //颜色
    double r=ui->doubleSpinBox_16->value();
    double g=ui->doubleSpinBox_17->value();
    double b=ui->doubleSpinBox_18->value();
    l.color=Vector3(r,g,b);
    //相机位置
    int x=ui->doubleSpinBox_7->value();
    int y=ui->doubleSpinBox_8->value();
    int z=ui->doubleSpinBox_9->value();
    camera=Vector3(x,y,z);
    //反射率
    //ui->doubleSpinBox_15->setValue(0);
    std::vector<Sphere> vec({R3,R1,R2});
    ui->widget_5->freshParam(l,vec);
    ui->widget_5->update();
}

