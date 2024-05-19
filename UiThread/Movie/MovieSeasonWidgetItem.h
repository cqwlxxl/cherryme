#ifndef MOVIESEASONWIDGETITEM_H
#define MOVIESEASONWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class MovieSeasonWidgetItem;
}

class MovieSeasonWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit MovieSeasonWidgetItem(MovieSeasonData season, int row, QWidget *parent = nullptr);
    ~MovieSeasonWidgetItem();

private:
    Ui::MovieSeasonWidgetItem *ui;

private slots:
    void on_pushButton_See_clicked(bool checked);   //电影部看完

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
    void setReleaseDate();    //设置发布日期

private:
    MovieSeasonData     mSeason;
    int mRow {-1};
};

#endif // MOVIESEASONWIDGETITEM_H
