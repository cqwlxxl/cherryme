#include "AnimeAnimeWidgetItem.h"
#include "ui_AnimeAnimeWidgetItem.h"

#include <QTimer>

AnimeAnimeWidgetItem::AnimeAnimeWidgetItem(AnimeData anime, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimeAnimeWidgetItem)
{
    ui->setupUi(this);

    ui->label_Name->setText(anime.name);
    ui->label_Name->setStyleSheet(QString("QLabel{color:%1;}").arg(anime.display?"black":"red"));
    //
    ui->pushButton_Collect->setVisible(anime.collect==0 ? false : true);
    ui->pushButton_Collect->setChecked(anime.collect==2);
    setPoint(anime.point);
    setTag1(anime.tag1);
    setTag2(anime.tag2);
    setTag3(anime.tag3);
    setZhuifan(anime.zhuifan);
    ui->progressBar_Radio->setRange(0, anime.total_season);
    ui->progressBar_Radio->setValue(anime.see_season);
    ui->progressBar_Radio->setFormat(QString("%1/%2").arg(anime.see_season).arg(anime.total_season));
    this->adjustSize();
    QTimer::singleShot(0, this, [this, anime]{
        ui->widget->setStyleSheet(QString("#widget{border-left:5px solid %1;}").arg(anime.see?"#99cc99":"#ffb050"));
        ui->progressBar_Radio->setStyleSheet(QString("QProgressBar::chunk{background-color:%1;width:1px;}QProgressBar{border:1px solid grey;}")
                                             .arg(anime.see_season==anime.total_season?"#99cc99":"#ffb050"));
    });
}

AnimeAnimeWidgetItem::~AnimeAnimeWidgetItem()
{
    delete ui;
}

///设置评分
void AnimeAnimeWidgetItem::setPoint(int point)
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

void AnimeAnimeWidgetItem::setTag1(bool tag1)
{
    ui->label_A->setVisible(tag1);
    if(tag1)
    {
        ui->label_A->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff00ff;margin-left:3px;}");
    }
}

void AnimeAnimeWidgetItem::setTag2(bool tag2)
{
    ui->label_B->setVisible(tag2);
    if(tag2)
    {
        ui->label_B->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff4500;margin-left:3px;}");
    }
}

void AnimeAnimeWidgetItem::setTag3(bool tag3)
{
    ui->label_C->setVisible(tag3);
    if(tag3)
    {
        ui->label_C->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#008000;margin-left:3px;}");
    }
}

///设置追番
void AnimeAnimeWidgetItem::setZhuifan(bool zhuifan)
{
    ui->label_Zhuifan->setVisible(zhuifan);
    if(zhuifan)
    {
        ui->label_Zhuifan->setStyleSheet("QLabel{color:red;margin-left:3px;}");
    }
}
