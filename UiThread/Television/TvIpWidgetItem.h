#ifndef TVIPWIDGETITEM_H
#define TVIPWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class TvIpWidgetItem;
}

class TvIpWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit TvIpWidgetItem(TvIpData ip, QWidget *parent = nullptr);
    ~TvIpWidgetItem();

private:
    Ui::TvIpWidgetItem *ui;

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
    void setZhuiju(bool zhuiju);    //设置追剧
};

#endif // TVIPWIDGETITEM_H
