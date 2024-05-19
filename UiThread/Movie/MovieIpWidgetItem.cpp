#include "MovieIpWidgetItem.h"
#include "ui_MovieIpWidgetItem.h"

#include <QTimer>

MovieIpWidgetItem::MovieIpWidgetItem(MovieIpData ip, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MovieIpWidgetItem)
{
    ui->setupUi(this);

    ui->label_Name->setText(ip.name);
    ui->label_Name->setStyleSheet(QString("QLabel{color:%1;}").arg(ip.display?"black":"red"));
    ui->pushButton_Collect->setVisible(ip.collect==0 ? false : true);
    ui->pushButton_Collect->setChecked(ip.collect==2);
    setPoint(ip.point);
    setTag1(ip.tag1);
    setTag2(ip.tag2);
    setTag3(ip.tag3);
    ui->progressBar_Radio->setRange(0, ip.total_season);
    ui->progressBar_Radio->setValue(ip.see_season);
    ui->progressBar_Radio->setFormat(QString("%1/%2").arg(ip.see_season).arg(ip.total_season));
    this->adjustSize();
    QTimer::singleShot(0, this, [this, ip]{
        ui->widget->setStyleSheet(QString("#widget{border-left:5px solid %1;}").arg(ip.see?"#99cc99":"#ffb050"));
        ui->progressBar_Radio->setStyleSheet(QString("QProgressBar::chunk{background-color:%1;width:1px;}QProgressBar{border:1px solid grey;}")
                                             .arg(ip.see_season==ip.total_season?"#99cc99":"#ffb050"));
    });
}

MovieIpWidgetItem::~MovieIpWidgetItem()
{
    delete ui;
}

///设置评分
void MovieIpWidgetItem::setPoint(int point)
{
    if(point == 0)
    {
        ui->label_Point->setVisible(false);
        return;
    }
    QString str;
    QString color;
    bool bold {false};
    switch(point)
    {
    case 1:  str = tr("烂作 1");  color = "#000000"; break;
    case 2:  str = tr("不行 2");  color = "#808000"; break;
    case 3:  str = tr("极差 3");  color = "#696969"; break;
    case 4:  str = tr("很差 4");  color = "#778899"; break;
    case 5:  str = tr("略差 5");  color = "#a9a9a9"; break;
    case 6:  str = tr("及格 6");  color = "#dda0dd"; break;
    case 7:  str = tr("海星 7");  color = "#87cefa"; break;
    case 8:  str = tr("一般 8");  color = "#6495ed"; bold = true; break;
    case 9:  str = tr("很好 9");  color = "#9370db"; bold = true; break;
    case 10: str = tr("完美 10"); color = "#008080"; bold = true; break;
    case 11: str = tr("极好 11"); color = "#32cd32"; bold = true; break;
    case 12: str = tr("神作 12"); color = "#ff8c00"; bold = true; break;
    default: break;
    }
    ui->label_Point->setText(str);
    QFont font = ui->label_Point->font();
    font.setBold(bold);
    ui->label_Point->setFont(font);
    ui->label_Point->setStyleSheet(QString("QLabel{border:1px solid grey;color:white;background-color:%1;margin-left:3px;}").arg(color));
}

void MovieIpWidgetItem::setTag1(bool tag1)
{
    ui->label_Tag1->setVisible(tag1);
    if(tag1)
    {
        ui->label_Tag1->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff00ff;margin-left:3px;}");
    }
}

void MovieIpWidgetItem::setTag2(bool tag2)
{
    ui->label_Tag2->setVisible(tag2);
    if(tag2)
    {
        ui->label_Tag2->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff4500;margin-left:3px;}");
    }
}

void MovieIpWidgetItem::setTag3(bool tag3)
{
    ui->label_Tag3->setVisible(tag3);
    if(tag3)
    {
        ui->label_Tag3->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#008000;margin-left:3px;}");
    }
}
