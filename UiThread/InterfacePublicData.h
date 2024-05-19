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
#include "UiThread/Movie/MovieIpWidgetItem.h"
#include "UiThread/Movie/MovieSeasonWidgetItem.h"

struct AnimeIpItemData
{
    QList<QListWidgetItem *> items;
    QList<AnimeIpWidgetItem *> widgets;
    QList<AnimeIpData> ips;
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

struct MovieIpItemData
{
    QList<QListWidgetItem *> items;
    QList<MovieIpWidgetItem *> widgets;
    QList<MovieIpData> ips;
};

struct MovieSeasonItemData
{
    QList<QListWidgetItem *> items;
    QList<MovieSeasonWidgetItem *> widgets;
    QList<MovieSeasonData> seasons;
};

//导航数据
struct IndexAnimeData
{
    int     pid {0};
    int     sid {0};
    int     eid {0};
    int     p_row {0};
    int     s_row {0};
    int     e_row {0};
    int     p_pos {0};
    int     s_pos {0};
    int     e_pos {0};
    int     s_page {1};
    int     e_page {1};
    bool    p_click {false};
    bool    s_click {false};
    bool    e_click {false};
};

//导航数据
struct IndexMovieData
{
    int     pid {0};
    int     sid {0};
    int     p_row {0};
    int     s_row {0};
    int     p_pos {0};
    int     s_pos {0};
    int     s_page {1};
    bool    p_click {false};
    bool    s_click {false};
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
    AnimeIpItemData         anime_ip;
    AnimeSeasonItemData     anime_season;
    AnimeEpisodeItemData    anime_ep;
    MovieIpItemData         movie_ip;
    MovieSeasonItemData     movie_season;
    IndexAnimeData          index_anime;    //导航数据
    IndexMovieData          index_movie;    //导航数据
};

extern InterfacePublicData gIPD;    //界面公共数据

#endif // INTERFACEPUBLICDATA_H
