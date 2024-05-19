#ifndef ANIMEIPWIDGETITEM_H
#define ANIMEIPWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class AnimeIpWidgetItem;
}

class AnimeIpWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit AnimeIpWidgetItem(AnimeIpData ip, QWidget *parent = nullptr);
    ~AnimeIpWidgetItem();

private:
    Ui::AnimeIpWidgetItem *ui;

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
    void setZhuifan(bool zhuifan);  //设置追番
};

#endif // ANIMEIPWIDGETITEM_H
