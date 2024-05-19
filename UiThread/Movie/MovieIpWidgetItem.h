#ifndef MOVIEIPWIDGETITEM_H
#define MOVIEIPWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class MovieIpWidgetItem;
}

class MovieIpWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit MovieIpWidgetItem(MovieIpData ip, QWidget *parent = nullptr);
    ~MovieIpWidgetItem();

private:
    Ui::MovieIpWidgetItem *ui;

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
};

#endif // MOVIEIPWIDGETITEM_H
