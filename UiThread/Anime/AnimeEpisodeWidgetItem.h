#ifndef ANIMEEPISODEWIDGETITEM_H
#define ANIMEEPISODEWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class AnimeEpisodeWidgetItem;
}

class AnimeEpisodeWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit AnimeEpisodeWidgetItem(AnimeEpisodeData episode, int row, QWidget *parent = nullptr);
    ~AnimeEpisodeWidgetItem();

private:
    Ui::AnimeEpisodeWidgetItem *ui;

private slots:
    void on_pushButton_See_clicked(bool checked);   //动漫话看完

private:
    void setType(int type);     //设置类型
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);

private:
    AnimeEpisodeData mEpisode;
    int mRow {-1};
};

#endif // ANIMEEPISODEWIDGETITEM_H
