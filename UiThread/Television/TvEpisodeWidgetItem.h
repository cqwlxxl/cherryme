#ifndef TVEPISODEWIDGETITEM_H
#define TVEPISODEWIDGETITEM_H

#include <QWidget>

#include "SqlThread/SqlDataDefine.h"

namespace Ui {
class TvEpisodeWidgetItem;
}

class TvEpisodeWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit TvEpisodeWidgetItem(TvEpisodeData episode, int row, QWidget *parent = nullptr);
    ~TvEpisodeWidgetItem();

private:
    Ui::TvEpisodeWidgetItem *ui;

private slots:
    void on_pushButton_See_clicked(bool checked);   //电视剧集看完

private:
    void setTag1(bool tag1);
    void setTag2(bool tag2);
    void setTag3(bool tag3);

private:
    TvEpisodeData mEpisode;
    int mRow {-1};
};

#endif // TVEPISODEWIDGETITEM_H
