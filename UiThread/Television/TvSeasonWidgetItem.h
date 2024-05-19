#ifndef TVSEASONWIDGETITEM_H
#define TVSEASONWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class TvSeasonWidgetItem;
}

class TvSeasonWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit TvSeasonWidgetItem(TvSeasonData season, QWidget *parent = nullptr);
    ~TvSeasonWidgetItem();

private:
    Ui::TvSeasonWidgetItem *ui;

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
    void setReleaseDate();      //设置发布日期

private:
    TvSeasonData    mSeason;
};

#endif // TVSEASONWIDGETITEM_H
