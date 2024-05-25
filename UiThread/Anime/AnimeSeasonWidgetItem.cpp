#include "AnimeSeasonWidgetItem.h"
#include "ui_AnimeSeasonWidgetItem.h"

#include <QTimer>

AnimeSeasonWidgetItem::AnimeSeasonWidgetItem(AnimeSeasonData season, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimeSeasonWidgetItem)
{
    ui->setupUi(this);

    mSeason = season;
    setType(season.type);
    ui->label_Type->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#f57ea0;margin-left:3px;padding-left:2px;padding-right:2px;}");
    ui->label_Name->setText(season.name);
    ui->label_Name->setStyleSheet(QString("QLabel{color:%1;}").arg(season.display?"black":"red"));
    ui->label_NameOrigin->setText(season.origin);
    ui->label_NameOrigin->setStyleSheet(QString("QLabel{color:%1;}").arg(season.display?"black":"red"));
    ui->pushButton_Collect->setVisible(season.collect==0 ? false : true);
    ui->pushButton_Collect->setChecked(season.collect==2);
    setPoint(season.point);
    setTag1(season.tag1);
    setTag2(season.tag2);
    setTag3(season.tag3);
    setReleaseDate();
    ui->progressBar_Radio->setRange(0, season.total_episode);
    ui->progressBar_Radio->setValue(season.see_episode);
    ui->progressBar_Radio->setFormat(QString("%1/%2").arg(season.see_episode).arg(season.total_episode));
    this->adjustSize();
    QTimer::singleShot(0, this, [this, season]{
        ui->widget->setStyleSheet(QString("#widget{border-left:5px solid %1;}").arg(season.see?"#99cc99":"#ffb050"));
        ui->progressBar_Radio->setStyleSheet(QString("QProgressBar::chunk{background-color:%1;width:1px;}QProgressBar{border:1px solid grey;}")
                                             .arg(season.see_episode==season.total_episode?"#99cc99":"#ffb050"));
    });
}

AnimeSeasonWidgetItem::~AnimeSeasonWidgetItem()
{
    delete ui;
}

///设置类型
void AnimeSeasonWidgetItem::setType(int type)
{
    QString str {""};
    bool show {true};
    switch(type)
    {
    case AnimeMovie:        str = tr("剧场版");     break;
    case AnimeTv:           str = tr("TV");         break;
    case AnimeSep:          str = tr("特别篇");     break;
    case AnimeCollection:   str = tr("总集篇");     break;
    case AnimeOva:          str = tr("OVA");        break;
    case AnimeOad:          str = tr("OAD");        break;
    case AnimeSp:           str = tr("特典");       break;
    case AnimeWeb:          str = tr("WEB");        break;
    default:
        show = false;
        break;
    }
    ui->label_Type->setText(str);
    ui->label_Type->setVisible(show);
}

///设置评分
void AnimeSeasonWidgetItem::setPoint(int point)
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

void AnimeSeasonWidgetItem::setTag1(bool tag1)
{
    ui->label_Tag1->setVisible(tag1);
    if(tag1)
    {
        ui->label_Tag1->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff00ff;margin-left:3px;}");
    }
}

void AnimeSeasonWidgetItem::setTag2(bool tag2)
{
    ui->label_Tag2->setVisible(tag2);
    if(tag2)
    {
        ui->label_Tag2->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff4500;margin-left:3px;}");
    }
}

void AnimeSeasonWidgetItem::setTag3(bool tag3)
{
    ui->label_Tag3->setVisible(tag3);
    if(tag3)
    {
        ui->label_Tag3->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#008000;margin-left:3px;}");
    }
}

///设置发布日期
void AnimeSeasonWidgetItem::setReleaseDate()
{
    if(mSeason.release_date_valid)
    {
        ui->label_ReleaseDate->setText(mSeason.release_date.toString("yyyy-MM-dd"));
        ui->label_ReleaseDate->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#cccccc;margin-left:3px;padding-left:2px;padding-right:2px;}");
    }
    else
    {
        ui->label_ReleaseDate->setVisible(false);
        ui->label_ReleaseDate->setStyleSheet("");
    }
}
