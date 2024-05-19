#ifndef ANIMEANIMEWIDGETITEM_H
#define ANIMEANIMEWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class AnimeAnimeWidgetItem;
}

class AnimeAnimeWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit AnimeAnimeWidgetItem(AnimeData anime, QWidget *parent = nullptr);
    ~AnimeAnimeWidgetItem();

private:
    void setPoint(int point);   //设置评分
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);
    void setZhuifan(bool zhuifan);  //设置追番

private:
    Ui::AnimeAnimeWidgetItem *ui;
};

#endif // ANIMEANIMEWIDGETITEM_H
