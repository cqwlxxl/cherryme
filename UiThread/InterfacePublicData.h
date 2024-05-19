#ifndef INTERFACEPUBLICDATA_H
#define INTERFACEPUBLICDATA_H

#include <QListWidgetItem>
#include <QObject>
#include <QVariant>

#include "PublicData.h"
#include "SqlThread/SqlDataDefine.h"
#include "UiThread/Anime/AnimeIpWidgetItem.h"
#include "UiThread/Anime/AnimeSeasonWidgetItem.h"
#include "UiThread/Anime/AnimeEpisodeWidgetItem.h"

struct AnimeItemData
{
    QList<QListWidgetItem *> items;
    QList<AnimeIpWidgetItem *> widgets;
    QList<AnimeData> animes;
};

struct AnimeSeasonItemData
{
    QList<QListWidgetItem *> items;
    QList<AnimeSeasonWidgetItem *> widgets;
    QList<AnimeSeasonData> seasons;
};

struct AnimeEpisodeItemData
{
    QList<QListWidgetItem *> items;
    QList<AnimeEpisodeWidgetItem *> widgets;
    QList<AnimeEpisodeData> eps;
};

//导航数据
struct IndexAnimeData
{
    int     aid {0};
    int     sid {0};
    int     eid {0};
    int     a_row {0};
    int     s_row {0};
    int     e_row {0};
    int     a_pos {0};
    int     s_pos {0};
    int     e_pos {0};
    int     s_page {1};
    int     e_page {1};
    bool    a_click {false};
    bool    s_click {false};
    bool    e_click {false};
};

class InterfacePublicData : public QObject
{
    Q_OBJECT
public:
    explicit InterfacePublicData(QObject *parent = nullptr);

signals:
    void SIGNALSendQuery(SqlOperateType operate, QVariant var);         //执行Sql
    void SIGNALReceiveQueryData(SqlOperateType operate, QVariant var);  //接收Sql结果信号
    void SIGNALAnimeEpisodeSee(AnimeEpisodeData episode, int row);      //动漫话看完信号

private slots:
    void slotSendQuery(SqlOperateType operate, QVariant var);   //发送到数据库前执行的操作
    void slotReceiveQueryData(SqlOperateType operate, QVariant var);    //接收sql结果槽函数

public:
    AnimeItemData           anime;
    AnimeSeasonItemData     anime_season;
    AnimeEpisodeItemData    anime_ep;
    IndexAnimeData          index_anime;    //导航数据
};

extern InterfacePublicData gIPD;    //界面公共数据

#endif // INTERFACEPUBLICDATA_H
