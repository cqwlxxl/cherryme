#ifndef ANIMESEASONWIDGETITEM_H
#define ANIMESEASONWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class AnimeSeasonWidgetItem;
}

class AnimeSeasonWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit AnimeSeasonWidgetItem(AnimeSeasonData season, QWidget *parent = nullptr);
    ~AnimeSeasonWidgetItem();

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
    void setReleaseDate();    //设置发布日期

private:
    Ui::AnimeSeasonWidgetItem *ui;

private:
    AnimeSeasonData     mSeason;
};

#endif // ANIMESEASONWIDGETITEM_H
