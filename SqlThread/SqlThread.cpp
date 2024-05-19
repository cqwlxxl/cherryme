#include "SqlThread.h"

#include <QTimer>
#include <QtMath>

Q_DECLARE_METATYPE(AnimeData)
Q_DECLARE_METATYPE(AnimeSeasonData)
Q_DECLARE_METATYPE(AnimeEpisodeData)
Q_DECLARE_METATYPE(AnimeRecentData)

SqlThread::SqlThread(QObject *parent)
    : QObject{parent}
{
    //加载MySQL驱动
    mDB = QSqlDatabase::addDatabase("QMYSQL");
}

SqlThread::~SqlThread()
{
    if(mDB.isOpen())
    {
        mDB.close();
    }

    QSqlDatabase::removeDatabase("QMYSQL");
}

///解析数据语句
void SqlThread::SLOTReceiveQuery(SqlOperateType operate, QVariant var)
{
    QString cmd;
    switch(operate)
    {
    case SOT_LOGIN_ANIME:
        emit SIGNALSendQueryData(SOT_LOGIN_ANIME, var.toBool()?login():logout());
        break;
    case SOT_GET_ANIME_ANIME:
    {
        QStringList strs = var.toStringList();
        int pagesize = 20;
        int page = strs[0].toInt();
        int offset = (page-1)*pagesize;
        cmd = QString("SELECT aid, name, keywords, see, see_season, "
                      "total_season, zhuifan, collect, point, display, "
                      "tag1, tag2, tag3 FROM `%1`").arg(TABLE_ANIME_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        cmd += QString(" ORDER BY CONVERT(name using gbk) LIMIT %1, %2").arg(offset).arg(pagesize);
        if(mQuery.exec(cmd))
        {
            QList<AnimeData> animes;
            while(mQuery.next())
            {
                AnimeData anime;
                anime.aid = mQuery.value(0).toInt();
                anime.name = mQuery.value(1).toString();
                anime.keywords = mQuery.value(2).toString();
                anime.see = (mQuery.value(3).toInt() == 1);
                anime.see_season = mQuery.value(4).toInt();
                anime.total_season = mQuery.value(5).toInt();
                anime.zhuifan = (mQuery.value(6).toInt() == 1);
                anime.collect = mQuery.value(7).toInt();
                anime.point = mQuery.value(8).toInt();
                anime.display = (mQuery.value(9).toInt() == 1);
                anime.tag1 = (mQuery.value(10).toInt() == 1);
                anime.tag2 = (mQuery.value(11).toInt() == 1);
                anime.tag3 = (mQuery.value(12).toInt() == 1);
                animes.append(anime);
            }

            QVariant sendVar;
            sendVar.setValue(animes);
            emit SIGNALSendQueryData(SOT_GET_ANIME_ANIME, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_ANIME_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)pagesize);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_ANIME_ANIME_PAGE, strs_send);
    }
        break;
    case SOT_GET_ANIME_RECENT:
    {
        bool limit = var.toBool();
        QList<AnimeRecentData> recents;
        cmd = QString("SELECT id,aid,sid,name,display FROM `%1`%2 ORDER BY id DESC LIMIT 7").arg(TABLE_ANIME_RECENT, limit?" WHERE display=1":"");
        mQuery.exec(cmd);
        while(mQuery.next())
        {
            AnimeRecentData recent;
            recent.id = mQuery.value(0).toInt();
            recent.aid = mQuery.value(1).toInt();
            recent.sid = mQuery.value(2).toInt();
            recent.name = mQuery.value(3).toString();
            recent.display = (mQuery.value(4).toInt()==1);
            recents.append(recent);
        }

        QVariant var_send;
        var_send.setValue(recents);
        emit SIGNALSendQueryData(SOT_GET_ANIME_RECENT, var_send);
    }
        break;
    case SOT_GET_ANIME_SEASON:
    {
        QStringList strs = var.toStringList();
        int sid = strs[3].toInt();
        int pagesize = 20;
        int page = strs[1].toInt();
        int offset = (page-1)*pagesize;
        bool limit = (strs[2].toInt() == 1);
        cmd = QString("SELECT sid, aid, name, release_date, see, "
                      "see_episode, total_episode, collect, point, display, "
                      "tag1, tag2, tag3 FROM `%2` WHERE aid = %1").arg(strs[0]).arg(TABLE_ANIME_SEASON);
        if(sid != -1)
        {   //最近模式
            cmd += QString(" AND sid=%1").arg(sid);
        }
        if(limit)
        {
            cmd += " AND display = 1";
        }
        cmd += QString(" LIMIT %1, %2").arg(offset).arg(pagesize);
        if(mQuery.exec(cmd))
        {
            QList<AnimeSeasonData> seasons;
            while(mQuery.next())
            {
                AnimeSeasonData season;
                season.sid = mQuery.value(0).toInt();
                season.aid = mQuery.value(1).toInt();
                season.name = mQuery.value(2).toString();
                season.release_date_valid = !((mQuery.value(3).toString() == "0000-00-00") || (mQuery.value(3).toString() == ""));
                season.release_date = season.release_date_valid ? QDate::fromString(mQuery.value(3).toString(), "yyyy-MM-dd") : QDate::currentDate();
                season.see = (mQuery.value(4).toInt() == 1);
                season.see_episode = mQuery.value(5).toInt();
                season.total_episode = mQuery.value(6).toInt();
                season.collect = mQuery.value(7).toInt();
                season.point = mQuery.value(8).toInt();
                season.display = (mQuery.value(9).toInt() == 1);
                season.tag1 = (mQuery.value(10).toInt() == 1);
                season.tag2 = (mQuery.value(11).toInt() == 1);
                season.tag3 = (mQuery.value(12).toInt() == 1);
                seasons.append(season);
            }

            QVariant sendVar;
            sendVar.setValue(seasons);
            emit SIGNALSendQueryData(SOT_GET_ANIME_SEASON, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1").arg(strs[0]).arg(TABLE_ANIME_SEASON);
        if(limit)
        {
            cmd += " AND display = 1";
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)pagesize);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_ANIME_SEASON_PAGE, strs_send);
    }
        break;
    case SOT_GET_ANIME_EPISODE:
    {
        QStringList strs = var.toStringList();
        int pagesize = 24;
        int page = strs[2].toInt();
        int offset = (page-1)*pagesize;
        cmd = QString("SELECT eid, aid, sid, episode, title, "
                      "see, tag1, tag2, tag3 FROM `%3` WHERE aid = %4 AND sid = %5 LIMIT %1, %2").arg(offset).arg(pagesize)
                .arg(TABLE_ANIME_EPISODE, strs[0], strs[1]);
        if(mQuery.exec(cmd))
        {
            QList<AnimeEpisodeData> eps;
            while(mQuery.next())
            {
                AnimeEpisodeData ep;
                ep.eid = mQuery.value(0).toInt();
                ep.aid = mQuery.value(1).toInt();
                ep.sid = mQuery.value(2).toInt();
                ep.episode = mQuery.value(3).toString();
                ep.title = mQuery.value(4).toString();
                ep.see = (mQuery.value(5).toInt() == 1);
                ep.tag1 = (mQuery.value(6).toInt() == 1);
                ep.tag2 = (mQuery.value(7).toInt() == 1);
                ep.tag3 = (mQuery.value(8).toInt() == 1);
                eps.append(ep);
            }
            QVariant sendVar;
            sendVar.setValue(eps);
            emit SIGNALSendQueryData(SOT_GET_ANIME_EPISODE, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1` WHERE aid = %2 AND sid = %3").arg(TABLE_ANIME_EPISODE, strs[0], strs[1]);
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)pagesize);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_ANIME_EPISODE_PAGE, strs_send);
    }
        break;
    case SOT_UPDATE_ANIME_ANIME_NAME:
    {
        AnimeData anime = var.value<AnimeData>();
        anime.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name = '%3' WHERE aid = %1").arg(anime.aid).arg(TABLE_ANIME_IP, anime.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_ANIME_KEYWORDS:
    {
        AnimeData anime = var.value<AnimeData>();
        anime.keywords.replace("'", "''");
        cmd = QString("UPDATE `%2` SET keywords = '%3' WHERE aid = %1").arg(anime.aid).arg(TABLE_ANIME_IP, anime.keywords);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_ANIME_ZHUIFAN:
    {
        AnimeData anime = var.value<AnimeData>();
        cmd = QString("UPDATE `%3` SET zhuifan = %1 WHERE aid = %2").arg(anime.zhuifan?1:0).arg(anime.aid).arg(TABLE_ANIME_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_ANIME_DISPLAY:
    {
        AnimeData anime = var.value<AnimeData>();
        cmd = QString("UPDATE `%3` SET display = %1 WHERE aid = %2").arg(anime.display?1:0).arg(anime.aid).arg(TABLE_ANIME_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_COLLECT:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        cmd = QString("UPDATE `%3` SET collect = %1 WHERE sid = %2").arg(season.collect).arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        calcAnimeCollect(season.aid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_NAME:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name = '%3' WHERE sid = %1").arg(season.sid).arg(TABLE_ANIME_SEASON, season.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_RELEASE_DATE:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        QString date = "0000-00-00";
        if(season.release_date_valid)
        {
            date = season.release_date.toString("yyyy-MM-dd");
        }
        cmd = QString("UPDATE `%2` SET release_date = '%3' WHERE sid = %1").arg(season.sid).arg(TABLE_ANIME_SEASON, date);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_POINT:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        cmd = QString("UPDATE `%3` SET point = %1 WHERE sid = %2").arg(season.point).arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        calcAnimePoint(season.aid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_DISPLAY:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        cmd = QString("UPDATE `%3` SET display = %1 WHERE sid = %2").arg(season.display).arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_SEE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET see = %1 WHERE eid = %2").arg(episode.see?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeSee(episode.aid, episode.sid);
        calcAnimeRecent(episode.aid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME_EPISODE_SEE, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_EPISODE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        episode.episode.replace("'", "''");
        cmd = QString("UPDATE `%2` SET episode = '%3' WHERE eid = %1").arg(episode.eid).arg(TABLE_ANIME_EPISODE, episode.episode);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TITLE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        episode.title.replace("'", "''");
        cmd = QString("UPDATE `%2` SET title = '%3' WHERE eid = %1").arg(episode.eid).arg(TABLE_ANIME_EPISODE, episode.title);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TAG1:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag1 = %1 WHERE eid = %2").arg(episode.tag1?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeTag1(episode.aid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TAG2:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag2 = %1 WHERE eid = %2").arg(episode.tag2?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeTag2(episode.aid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TAG3:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag3 = %1 WHERE eid = %2").arg(episode.tag3?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeTag3(episode.aid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_INSERT_ANIME_ANIME:
    {
        AnimeData anime = var.value<AnimeData>();
        anime.name.replace("'", "''");
        anime.keywords.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (name, keywords, see, see_season, total_season, zhuifan, collect, point, display, tag1, tag2, tag3)"
                      " VALUES ('%2', '%3', 0, 0, 0, %4, 0, 0, %5, 0, 0, 0)").arg(TABLE_ANIME_IP, anime.name, anime.keywords, anime.zhuifan?"1":"0", anime.display?"1":"0");
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_INSERT_ANIME_SEASON:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (aid, name, release_date, see, see_episode, total_episode, collect, point, display, tag1, tag2, tag3)"
                      " VALUES (%2, '%3', '%4', 0, 0, 0, %5, %6, %7, 0, 0, 0)")
                .arg(TABLE_ANIME_SEASON, QString::number(season.aid), season.name,
                     season.release_date_valid?season.release_date.toString("yyyy-MM-dd"):"0000-00-00",
                     QString::number(season.collect), QString::number(season.point), season.display?"1":"0");
        mQuery.exec(cmd);

        calcAnimeSee(season.aid);
        calcAnimePoint(season.aid);
        calcAnimeCollect(season.aid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_INSERT_ANIME_EPISODE:
    {
        QList<AnimeEpisodeData> eps = var.value<QList<AnimeEpisodeData> >();
        int aid = eps.first().aid;
        int sid = eps.first().sid;
        cmd.clear();
        foreach(AnimeEpisodeData ep, eps)
        {
            cmd += QString("INSERT INTO `%6` (aid, sid, episode, title, see, tag1, tag2, tag3)"
                           " VALUES (%1, %2, '%7', '%8', 0, %3, %4, %5);")
                    .arg(aid).arg(sid).arg(ep.tag1).arg(ep.tag2).arg(ep.tag3).arg(TABLE_ANIME_EPISODE, ep.episode.replace("'", "''"), ep.title.replace("'", "''"));
        }
        mDB.transaction();
        mQuery.exec(cmd);
        mDB.commit();

        calcAnimeSee(aid, sid);
        calcAnimeTag1(aid, sid);
        calcAnimeTag2(aid, sid);
        calcAnimeTag3(aid, sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_ANIME:
    {
        int aid = var.toInt();
        //删除关联话
        cmd = QString("DELETE FROM `%2` WHERE aid = %1;").arg(aid).arg(TABLE_ANIME_EPISODE);
        //删除关联季
        cmd += QString("DELETE FROM `%2` WHERE aid = %1;").arg(aid).arg(TABLE_ANIME_SEASON);
        //删除动漫
        cmd += QString("DELETE FROM `%2` WHERE aid = %1;").arg(aid).arg(TABLE_ANIME_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_SEASON:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        //删除关联话
        cmd = QString("DELETE FROM `%2` WHERE sid = %1;").arg(season.sid).arg(TABLE_ANIME_EPISODE);
        //删除季
        cmd += QString("DELETE FROM `%2` WHERE sid = %1;").arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        calcAnimeSee(season.aid);
        calcAnimePoint(season.aid);
        calcAnimeCollect(season.aid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_EPISODE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("DELETE FROM `%2` WHERE eid = %1").arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeSee(episode.aid, episode.sid);
        calcAnimeTag1(episode.aid, episode.sid);
        calcAnimeTag2(episode.aid, episode.sid);
        calcAnimeTag3(episode.aid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_RECENT:
    {
        cmd = QString("DELETE FROM `%2` WHERE id=%1").arg(var.toInt()).arg(TABLE_ANIME_RECENT);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_DELETE_ANIME_RECENT, QVariant());
    }
        break;
    default:
        break;
    }
}

///登录
bool SqlThread::login()
{
    //连接数据库
    mDB.setHostName("localhost");
    mDB.setDatabaseName("cherryme");
    mDB.setPort(3306);
    mDB.setUserName("cherryme");
    mDB.setPassword("cherryme");
    mQuery = QSqlQuery(mDB);
    if(!mDB.open())
    {
        return false;
    }
    return true;
}

///注销
bool SqlThread::logout()
{
    if(mDB.isOpen())
    {
        mDB.close();
    }
    return false;
}

///计算话
void SqlThread::calcAnimeSee(int aid, int sid)
{
    //已看话
    QString cmd = QString("SELECT count(*) FROM `%3` WHERE aid = %1 AND sid = %2 AND see = 1").arg(aid).arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int see_ep = mQuery.value(0).toInt();
    //总话
    cmd = QString("SELECT count(*) FROM `%3` WHERE aid = %1 AND sid = %2").arg(aid).arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int ep = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%6` SET see_episode = %1, total_episode = %2, see = %3 WHERE aid = %4 AND sid = %5")
            .arg(see_ep).arg(ep).arg(see_ep==ep?1:0).arg(aid).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //已看季
    cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1 AND see = 1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();
    //总季
    cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int seasea = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%5` SET see_season = %1, total_season = %2, see = %3 WHERE aid = %4").arg(see_season).arg(seasea).arg(see_season==seasea?1:0).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算进度，季变化
void SqlThread::calcAnimeSee(int aid)
{
    //更新动漫表
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1 AND see = 1").arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();   //看了的季
    cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int total_season = mQuery.value(0).toInt(); //总的季
    cmd = QString("UPDATE `%5` SET see = %1, see_season = %2, total_season = %3 WHERE aid = %4")
            .arg(see_season==total_season?1:0).arg(see_season).arg(total_season).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算评分
void SqlThread::calcAnimePoint(int aid)
{
    //查找季最高评分
    QString cmd = QString("SELECT max(point) FROM `%2` WHERE aid = %1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int max_point = mQuery.value(0).toInt();
    //更新动漫最高评分
    cmd = QString("UPDATE `%3` SET point = %1 WHERE aid = %2").arg(max_point).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算收藏
void SqlThread::calcAnimeCollect(int aid)
{
    //查看季要收藏数
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1 AND collect = 1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_it = mQuery.value(0).toInt();
    //查看季已收藏数
    cmd = QString("SELECT count(*) FROM `%2` WHERE aid = %1 AND collect = 2").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_ok = mQuery.value(0).toInt();
    //更新动漫收藏
    int collect = 0;    //初始为无收藏
    if(count_collect_it > 0)
    {   //有要收藏，则标记为要收藏
        collect = 1;
    }
    else if(count_collect_ok > 0)
    {   //都是已收藏，则标记为已收藏
        collect = 2;
    }
    cmd = QString("UPDATE `%3` SET collect = %1 WHERE aid = %2").arg(collect).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算tag1
void SqlThread::calcAnimeTag1(int aid, int sid)
{
    //话总tag1
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag1 = 1 AND sid = %1").arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag1 = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%3` SET tag1 = %1 WHERE sid = %2").arg(count_tag1>0?1:0).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //季总tag1
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag1 = 1 AND aid = %1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag1 = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%3` SET tag1 = %1 WHERE aid = %2").arg(count_tag1>0?1:0).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算tag2
void SqlThread::calcAnimeTag2(int aid, int sid)
{
    //话总tag2
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag2 = 1 AND sid = %1").arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag2 = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%3` SET tag2 = %1 WHERE sid = %2").arg(count_tag2>0?1:0).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //季总tag2
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag2 = 1 AND aid = %1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag2 = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%3` SET tag2 = %1 WHERE aid = %2").arg(count_tag2>0?1:0).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算tag3
void SqlThread::calcAnimeTag3(int aid, int sid)
{
    //话总tag3
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag3 = 1 AND sid = %1").arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag3 = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%3` SET tag3 = %1 WHERE sid = %2").arg(count_tag3>0?1:0).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //季总tag3
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag3 = 1 AND aid = %1").arg(aid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag3 = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%3` SET tag3 = %1 WHERE aid = %2").arg(count_tag3>0?1:0).arg(aid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///更新最近观看
void SqlThread::calcAnimeRecent(int aid, int sid)
{
    QString cmd;

    bool need {false};  //是否插入记录
    cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_ANIME_RECENT);
    mQuery.exec(cmd);
    mQuery.next();
    int count_recent = mQuery.value(0).toInt();
    if(count_recent == 0)
    {   //无记录，直接插入
        need = true;
    }
    else
    {   //有记录
        cmd = QString("SELECT sid FROM `%1` WHERE id=(SELECT MAX(id) FROM `%1`)").arg(TABLE_ANIME_RECENT);
        mQuery.exec(cmd);
        mQuery.next();
        int sid_pre = mQuery.value(0).toInt();
        if(sid_pre != sid)
        {   //与上一条不同才插入
            need = true;
        }
    }

    if(need)
    {
        //查看动漫季显示、名称
        cmd = QString("SELECT display,name FROM `%2` WHERE sid=%1").arg(sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);
        mQuery.next();
        int display = mQuery.value(0).toInt();
        QString name = mQuery.value(1).toString();
        //插入
        cmd = QString("INSERT INTO `%4` (aid,sid,name,display) VALUES (%1,%2,'%5',%3)").arg(aid).arg(sid).arg(display).arg(TABLE_ANIME_RECENT, name);
        mQuery.exec(cmd);
    }
}
