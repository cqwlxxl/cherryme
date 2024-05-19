#include "AnimeEpisodeWidgetItem.h"
#include "ui_AnimeEpisodeWidgetItem.h"

#include <QTimer>

#include "UiThread/InterfacePublicData.h"

AnimeEpisodeWidgetItem::AnimeEpisodeWidgetItem(AnimeEpisodeData episode, int row, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimeEpisodeWidgetItem)
{
    ui->setupUi(this);

    mEpisode = episode;
    mRow = row;
    ui->label_Name->setText(QString("[%1] %2").arg(episode.episode, episode.title));
    ui->label_Name->setStyleSheet("QLabel{margin-left:3px;}");
    ui->pushButton_See->setChecked(episode.see);
    setTag1(episode.tag1);
    setTag2(episode.tag2);
    setTag3(episode.tag3);
    this->adjustSize();
    QTimer::singleShot(0, this, [this, episode]{
        ui->widget->setStyleSheet(QString("#widget{border-left:5px solid %1;}").arg(episode.see?"#99cc99":"#ffb050"));
    });
}

AnimeEpisodeWidgetItem::~AnimeEpisodeWidgetItem()
{
    delete ui;
}

///动漫话看完
void AnimeEpisodeWidgetItem::on_pushButton_See_clicked(bool checked)
{
    mEpisode.see = checked;
    emit gIPD.SIGNALAnimeEpisodeSee(mEpisode, mRow);
}

void AnimeEpisodeWidgetItem::setTag1(bool tag1)
{
    ui->label_Tag1->setVisible(tag1);
    if(tag1)
    {
        ui->label_Tag1->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff00ff;margin-left:3px;}");
    }
}

void AnimeEpisodeWidgetItem::setTag2(bool tag2)
{
    ui->label_Tag2->setVisible(tag2);
    if(tag2)
    {
        ui->label_Tag2->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#ff4500;margin-left:3px;}");
    }
}

void AnimeEpisodeWidgetItem::setTag3(bool tag3)
{
    ui->label_Tag3->setVisible(tag3);
    if(tag3)
    {
        ui->label_Tag3->setStyleSheet("QLabel{border:1px solid grey;color:white;background-color:#008000;margin-left:3px;}");
    }
}

