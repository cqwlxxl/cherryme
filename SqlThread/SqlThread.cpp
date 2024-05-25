#include "SqlThread.h"

#include <QTimer>
#include <QtMath>

Q_DECLARE_METATYPE(AnimeIpData)
Q_DECLARE_METATYPE(AnimeSeasonData)
Q_DECLARE_METATYPE(AnimeEpisodeData)
Q_DECLARE_METATYPE(AnimeRecentData)
Q_DECLARE_METATYPE(MovieIpData)
Q_DECLARE_METATYPE(MovieSeasonData)
Q_DECLARE_METATYPE(MovieRecentData)
Q_DECLARE_METATYPE(TvIpData)
Q_DECLARE_METATYPE(TvSeasonData)
Q_DECLARE_METATYPE(TvEpisodeData)
Q_DECLARE_METATYPE(TvRecentData)

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
    case SOT_CONNECT_MYSQL:
        emit SIGNALSendQueryData(SOT_CONNECT_MYSQL, var.toBool()?login():logout());
        break;
    case SOT_SELECT_ANIME_IP:
    {
        QStringList strs = var.toStringList();
        int page_size = 20;
        int page = strs[0].toInt();
        int offset = (page-1)*page_size;
        cmd = QString("SELECT pid,name,keywords,see,see_season,"
                      "total_season,zhuifan,collect,point,display,"
                      "tag1,tag2,tag3 FROM `%1`").arg(TABLE_ANIME_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        cmd += QString(" ORDER BY CONVERT(name using gbk) LIMIT %1,%2").arg(offset).arg(page_size);
        if(mQuery.exec(cmd))
        {
            QList<AnimeIpData> ips;
            while(mQuery.next())
            {
                AnimeIpData ip;
                ip.pid = mQuery.value(0).toInt();
                ip.name = mQuery.value(1).toString();
                ip.keywords = mQuery.value(2).toString();
                ip.see = (mQuery.value(3).toInt() == 1);
                ip.see_season = mQuery.value(4).toInt();
                ip.total_season = mQuery.value(5).toInt();
                ip.zhuifan = (mQuery.value(6).toInt() == 1);
                ip.collect = mQuery.value(7).toInt();
                ip.point = mQuery.value(8).toInt();
                ip.display = (mQuery.value(9).toInt() == 1);
                ip.tag1 = (mQuery.value(10).toInt() == 1);
                ip.tag2 = (mQuery.value(11).toInt() == 1);
                ip.tag3 = (mQuery.value(12).toInt() == 1);
                ips.append(ip);
            }

            QVariant sendVar;
            sendVar.setValue(ips);
            emit SIGNALSendQueryData(SOT_SELECT_ANIME_IP, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_ANIME_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)page_size);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_ANIME_IP_PAGE, strs_send);
    }
        break;
    case SOT_SELECT_ANIME_RECENT:
    {
        bool limit = var.toBool();
        QList<AnimeRecentData> recents;
        cmd = QString("SELECT id,pid,sid,name,display FROM `%1`%2 ORDER BY id DESC LIMIT 7").arg(TABLE_ANIME_RECENT, limit?" WHERE display=1":"");
        mQuery.exec(cmd);
        while(mQuery.next())
        {
            AnimeRecentData recent;
            recent.id = mQuery.value(0).toInt();
            recent.pid = mQuery.value(1).toInt();
            recent.sid = mQuery.value(2).toInt();
            recent.name = mQuery.value(3).toString();
            recent.display = (mQuery.value(4).toInt()==1);
            recents.append(recent);
        }

        QVariant var_send;
        var_send.setValue(recents);
        emit SIGNALSendQueryData(SOT_SELECT_ANIME_RECENT, var_send);
    }
        break;
    case SOT_SELECT_ANIME_SEASON:
    {
        QStringList strs = var.toStringList();
        int sid = strs[3].toInt();
        int page_size = 20;
        int page = strs[1].toInt();
        int offset = (page-1)*page_size;
        bool limit = (strs[2].toInt() == 1);
        cmd = QString("SELECT sid,pid,name,release_date,see,"
                      "see_episode,total_episode,collect,point,display,"
                      "tag1,tag2,tag3 FROM `%1` WHERE pid=%2").arg(TABLE_ANIME_SEASON, strs[0]);
        if(sid != -1)
        {   //最近模式
            cmd += QString(" AND sid=%1").arg(sid);
        }
        if(limit)
        {
            cmd += " AND display=1";
        }
        cmd += " ORDER BY release_date";
        cmd += QString(" LIMIT %1,%2").arg(offset).arg(page_size);
        if(mQuery.exec(cmd))
        {
            QList<AnimeSeasonData> seasons;
            while(mQuery.next())
            {
                AnimeSeasonData season;
                season.sid = mQuery.value(0).toInt();
                season.pid = mQuery.value(1).toInt();
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

            QVariant send_var;
            send_var.setValue(seasons);
            emit SIGNALSendQueryData(SOT_SELECT_ANIME_SEASON, send_var);
        }
        cmd = QString("SELECT count(*) FROM `%1` WHERE pid=%2").arg(TABLE_ANIME_SEASON, strs[0]);
        if(limit)
        {
            cmd += " AND display = 1";
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)page_size);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_ANIME_SEASON_PAGE, strs_send);
    }
        break;
    case SOT_SELECT_ANIME_EPISODE:
    {
        QStringList strs = var.toStringList();
        int pagesize = 24;
        int page = strs[2].toInt();
        int offset = (page-1)*pagesize;
        cmd = QString("SELECT eid,pid,sid,episode,title,origin,"
                      "see,tag1,tag2,tag3 FROM `%3` WHERE pid=%4 AND sid=%5 LIMIT %1,%2").arg(offset).arg(pagesize)
                .arg(TABLE_ANIME_EPISODE, strs[0], strs[1]);
        if(mQuery.exec(cmd))
        {
            QList<AnimeEpisodeData> eps;
            while(mQuery.next())
            {
                AnimeEpisodeData ep;
                ep.eid = mQuery.value(0).toInt();
                ep.pid = mQuery.value(1).toInt();
                ep.sid = mQuery.value(2).toInt();
                ep.episode = mQuery.value(3).toString();
                ep.title = mQuery.value(4).toString();
                ep.origin = mQuery.value(5).toString();
                ep.see = (mQuery.value(6).toInt() == 1);
                ep.tag1 = (mQuery.value(7).toInt() == 1);
                ep.tag2 = (mQuery.value(8).toInt() == 1);
                ep.tag3 = (mQuery.value(9).toInt() == 1);
                eps.append(ep);
            }
            QVariant sendVar;
            sendVar.setValue(eps);
            emit SIGNALSendQueryData(SOT_SELECT_ANIME_EPISODE, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1` WHERE pid=%2 AND sid=%3").arg(TABLE_ANIME_EPISODE, strs[0], strs[1]);
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)pagesize);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_ANIME_EPISODE_PAGE, strs_send);
    }
        break;
    case SOT_UPDATE_ANIME_IP_NAME:
    {
        AnimeIpData ip = var.value<AnimeIpData>();
        ip.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name='%3' WHERE pid=%1").arg(ip.pid).arg(TABLE_ANIME_IP, ip.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_IP_KEYWORDS:
    {
        AnimeIpData ip = var.value<AnimeIpData>();
        ip.keywords.replace("'", "''");
        cmd = QString("UPDATE `%2` SET keywords='%3' WHERE pid=%1").arg(ip.pid).arg(TABLE_ANIME_IP, ip.keywords);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_IP_ZHUIFAN:
    {
        AnimeIpData ip = var.value<AnimeIpData>();
        cmd = QString("UPDATE `%3` SET zhuifan=%1 WHERE pid=%2").arg(ip.zhuifan?1:0).arg(ip.pid).arg(TABLE_ANIME_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_IP_DISPLAY:
    {
        AnimeIpData ip = var.value<AnimeIpData>();
        cmd = QString("UPDATE `%3` SET display=%1 WHERE pid=%2").arg(ip.display?1:0).arg(ip.pid).arg(TABLE_ANIME_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_COLLECT:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        cmd = QString("UPDATE `%3` SET collect=%1 WHERE sid=%2").arg(season.collect).arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        calcAnimeCollect(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_NAME:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name='%3' WHERE sid=%1").arg(season.sid).arg(TABLE_ANIME_SEASON, season.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
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
        cmd = QString("UPDATE `%2` SET release_date='%3' WHERE sid=%1").arg(season.sid).arg(TABLE_ANIME_SEASON, date);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_POINT:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        cmd = QString("UPDATE `%3` SET point=%1 WHERE sid=%2").arg(season.point).arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        calcAnimePoint(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_SEASON_DISPLAY:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        cmd = QString("UPDATE `%3` SET display=%1 WHERE sid=%2").arg(season.display).arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_SEE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET see=%1 WHERE eid=%2").arg(episode.see?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeSee(episode.pid, episode.sid);
        calcAnimeRecent(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_ANIME_EPISODE_SEE, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_EPISODE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        episode.episode.replace("'", "''");
        cmd = QString("UPDATE `%2` SET episode='%3' WHERE eid=%1").arg(episode.eid).arg(TABLE_ANIME_EPISODE, episode.episode);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TITLE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        episode.title.replace("'", "''");
        cmd = QString("UPDATE `%2` SET title='%3' WHERE eid=%1").arg(episode.eid).arg(TABLE_ANIME_EPISODE, episode.title);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TITLE_ORIGIN:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        episode.origin.replace("'", "''");
        cmd = QString("UPDATE `%2` SET origin='%3' WHERE eid=%1").arg(episode.eid).arg(TABLE_ANIME_EPISODE, episode.origin);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TAG1:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag1=%1 WHERE eid=%2").arg(episode.tag1?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeTag1(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TAG2:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag2=%1 WHERE eid=%2").arg(episode.tag2?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeTag2(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_ANIME_EPISODE_TAG3:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag3=%1 WHERE eid=%2").arg(episode.tag3?1:0).arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeTag3(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_INSERT_ANIME_IP:
    {
        AnimeIpData ip = var.value<AnimeIpData>();
        ip.name.replace("'", "''");
        ip.keywords.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (name,keywords,see,see_season,total_season,zhuifan,collect,point,display,tag1,tag2,tag3)"
                      " VALUES ('%2','%3',0,0,0,%4,0,0,%5,0,0,0)").arg(TABLE_ANIME_IP, ip.name, ip.keywords, ip.zhuifan?"1":"0", ip.display?"1":"0");
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_INSERT_ANIME_SEASON:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (pid,name,release_date,see,see_episode,total_episode,collect,point,display,tag1,tag2,tag3)"
                      " VALUES (%2,'%3','%4',0,0,0,%5,%6,%7,0,0,0)")
                .arg(TABLE_ANIME_SEASON, QString::number(season.pid), season.name,
                     season.release_date_valid?season.release_date.toString("yyyy-MM-dd"):"0000-00-00",
                     QString::number(season.collect), QString::number(season.point), season.display?"1":"0");
        mQuery.exec(cmd);

        calcAnimeSee(season.pid);
        calcAnimePoint(season.pid);
        calcAnimeCollect(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_INSERT_ANIME_EPISODE:
    {
        QList<AnimeEpisodeData> eps = var.value<QList<AnimeEpisodeData> >();
        int pid = eps.first().pid;
        int sid = eps.first().sid;
        cmd.clear();
        foreach(AnimeEpisodeData ep, eps)
        {
            cmd += QString("INSERT INTO `%6` (pid,sid,episode,title,origin,see,tag1,tag2,tag3)"
                           " VALUES (%1,%2,'%7','%8','%9',0,%3,%4,%5);")
                    .arg(pid).arg(sid).arg(ep.tag1).arg(ep.tag2).arg(ep.tag3).arg(TABLE_ANIME_EPISODE, ep.episode.replace("'", "''"), ep.title.replace("'", "''"),ep.origin.replace("'", "''"));
        }
        mDB.transaction();
        mQuery.exec(cmd);

        calcAnimeSee(pid, sid);
        calcAnimeTag1(pid, sid);
        calcAnimeTag2(pid, sid);
        calcAnimeTag3(pid, sid);
        mQuery.clear();
        mDB.commit();

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_IP:
    {
        int pid = var.toInt();
        //删除关联最近
        cmd = QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_ANIME_RECENT);
        //删除关联话
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_ANIME_EPISODE);
        //删除关联季
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_ANIME_SEASON);
        //删除动漫
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_ANIME_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_DELETE_ANIME_IP, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_SEASON:
    {
        AnimeSeasonData season = var.value<AnimeSeasonData>();
        //删除关联最近
        cmd = QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_ANIME_RECENT);
        //删除关联话
        cmd += QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_ANIME_EPISODE);
        //删除季
        cmd += QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_ANIME_SEASON);
        mQuery.exec(cmd);

        calcAnimeSee(season.pid);
        calcAnimePoint(season.pid);
        calcAnimeCollect(season.pid);
        calcAnimeTag1(season.pid, season.sid);
        calcAnimeTag2(season.pid, season.sid);
        calcAnimeTag3(season.pid, season.sid);

        emit SIGNALSendQueryData(SOT_DELETE_ANIME_SEASON, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_EPISODE:
    {
        AnimeEpisodeData episode = var.value<AnimeEpisodeData>();
        cmd = QString("DELETE FROM `%2` WHERE eid=%1").arg(episode.eid).arg(TABLE_ANIME_EPISODE);
        mQuery.exec(cmd);

        calcAnimeSee(episode.pid, episode.sid);
        calcAnimeTag1(episode.pid, episode.sid);
        calcAnimeTag2(episode.pid, episode.sid);
        calcAnimeTag3(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_ANIME_RESHOW, QVariant());
    }
        break;
    case SOT_DELETE_ANIME_RECENT:
    {
        cmd = QString("DELETE FROM `%2` WHERE id=%1").arg(var.toInt()).arg(TABLE_ANIME_RECENT);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_DELETE_ANIME_RECENT, QVariant());
    }
        break;
    case SOT_SELECT_MOVIE_IP:
    {
        QStringList strs = var.toStringList();
        int page_size = 20;
        int page = strs[0].toInt();
        int offset = (page-1)*page_size;
        cmd = QString("SELECT pid,name,keywords,see,see_season,"
                      "total_season,collect,point,display,"
                      "tag1,tag2,tag3 FROM `%1`").arg(TABLE_MOVIE_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        cmd += QString(" ORDER BY CONVERT(name using gbk) LIMIT %1,%2").arg(offset).arg(page_size);
        if(mQuery.exec(cmd))
        {
            QList<MovieIpData> ips;
            while(mQuery.next())
            {
                MovieIpData ip;
                ip.pid = mQuery.value(0).toInt();
                ip.name = mQuery.value(1).toString();
                ip.keywords = mQuery.value(2).toString();
                ip.see = (mQuery.value(3).toInt() == 1);
                ip.see_season = mQuery.value(4).toInt();
                ip.total_season = mQuery.value(5).toInt();
                ip.collect = mQuery.value(6).toInt();
                ip.point = mQuery.value(7).toInt();
                ip.display = (mQuery.value(8).toInt() == 1);
                ip.tag1 = (mQuery.value(9).toInt() == 1);
                ip.tag2 = (mQuery.value(10).toInt() == 1);
                ip.tag3 = (mQuery.value(11).toInt() == 1);
                ips.append(ip);
            }

            QVariant sendVar;
            sendVar.setValue(ips);
            emit SIGNALSendQueryData(SOT_SELECT_MOVIE_IP, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_MOVIE_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)page_size);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_MOVIE_IP_PAGE, strs_send);
    }
        break;
    case SOT_SELECT_MOVIE_RECENT:
    {
        bool limit = var.toBool();
        QList<MovieRecentData> recents;
        cmd = QString("SELECT id,pid,sid,name,display FROM `%1`%2 ORDER BY id DESC LIMIT 7").arg(TABLE_MOVIE_RECENT, limit?" WHERE display=1":"");
        mQuery.exec(cmd);
        while(mQuery.next())
        {
            MovieRecentData recent;
            recent.id = mQuery.value(0).toInt();
            recent.pid = mQuery.value(1).toInt();
            recent.sid = mQuery.value(2).toInt();
            recent.name = mQuery.value(3).toString();
            recent.display = (mQuery.value(4).toInt()==1);
            recents.append(recent);
        }

        QVariant var_send;
        var_send.setValue(recents);
        emit SIGNALSendQueryData(SOT_SELECT_MOVIE_RECENT, var_send);
    }
        break;
    case SOT_SELECT_MOVIE_SEASON:
    {
        QStringList strs = var.toStringList();
        int page_size = 20;
        int page = strs[1].toInt();
        int offset = (page-1)*page_size;
        cmd = QString("SELECT sid,pid,name,release_date,see,"
                      "collect,point,tag1,tag2,tag3"
                      " FROM `%3` WHERE pid=%4 LIMIT %1, %2")
                .arg(offset).arg(page_size).arg(TABLE_MOVIE_SEASON, strs[0]);
        if(mQuery.exec(cmd))
        {
            QList<MovieSeasonData> seasons;
            while(mQuery.next())
            {
                MovieSeasonData season;
                season.sid = mQuery.value(0).toInt();
                season.pid = mQuery.value(1).toInt();
                season.name = mQuery.value(2).toString();
                season.release_date_valid = !((mQuery.value(3).toString() == "0000-00-00") || (mQuery.value(3).toString() == ""));
                season.release_date = season.release_date_valid ? QDate::fromString(mQuery.value(3).toString(), "yyyy-MM-dd") : QDate::currentDate();
                season.see = (mQuery.value(4).toInt() == 1);
                season.collect = mQuery.value(5).toInt();
                season.point = mQuery.value(6).toInt();
                season.tag1 = (mQuery.value(7).toInt() == 1);
                season.tag2 = (mQuery.value(8).toInt() == 1);
                season.tag3 = (mQuery.value(9).toInt() == 1);
                seasons.append(season);
            }

            QVariant send_var;
            send_var.setValue(seasons);
            emit SIGNALSendQueryData(SOT_SELECT_MOVIE_SEASON, send_var);
        }
        cmd = QString("SELECT count(*) FROM `%1` WHERE pid=%2").arg(TABLE_MOVIE_SEASON, strs[0]);
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)page_size);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_MOVIE_SEASON_PAGE, strs_send);
    }
        break;
    case SOT_INSERT_MOVIE_IP:
    {
        MovieIpData ip = var.value<MovieIpData>();
        ip.name.replace("'", "''");
        ip.keywords.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (name,keywords,see,see_season,total_season,collect,point,display,tag1,tag2,tag3)"
                      " VALUES ('%2','%3',0,0,0,0,0,%4,0,0,0)").arg(TABLE_MOVIE_IP, ip.name, ip.keywords, ip.display?"1":"0");
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_DELETE_MOVIE_IP:
    {
        int pid = var.toInt();
        //删除关联部
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_MOVIE_SEASON);
        //删除电影
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_MOVIE_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_INSERT_MOVIE_SEASON:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("INSERT INTO `%7` (pid,name,release_date,see,collect,point,tag1,tag2,tag3)"
                      " VALUES (%1,'%8','%9',0,%2,%3,%4,%5,%6)")
                .arg(season.pid).arg(season.collect).arg(season.point).arg(season.tag1?1:0).arg(season.tag2?1:0).arg(season.tag3?1:0)
                .arg(TABLE_MOVIE_SEASON, season.name, season.release_date_valid?season.release_date.toString("yyyy-MM-dd"):"0000-00-00");
        mQuery.exec(cmd);

        calcMovieSee(season.pid);
        calcMoviePoint(season.pid);
        calcMovieCollect(season.pid);
        calcMovieTag1(season.pid);
        calcMovieTag2(season.pid);
        calcMovieTag3(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_DELETE_MOVIE_SEASON:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        //删除部
        cmd += QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMovieSee(season.pid);
        calcMoviePoint(season.pid);
        calcMovieCollect(season.pid);
        calcMovieTag1(season.pid);
        calcMovieTag2(season.pid);
        calcMovieTag3(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_IP_NAME:
    {
        MovieIpData ip = var.value<MovieIpData>();
        ip.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name='%3' WHERE pid=%1").arg(ip.pid).arg(TABLE_MOVIE_IP, ip.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_IP_DISPLAY:
    {
        MovieIpData ip = var.value<MovieIpData>();
        cmd = QString("UPDATE `%3` SET display=%1 WHERE pid=%2").arg(ip.display?1:0).arg(ip.pid).arg(TABLE_MOVIE_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_IP_KEYWORDS:
    {
        MovieIpData ip = var.value<MovieIpData>();
        ip.keywords.replace("'", "''");
        cmd = QString("UPDATE `%2` SET keywords='%3' WHERE pid=%1").arg(ip.pid).arg(TABLE_MOVIE_IP, ip.keywords);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_NAME:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name='%3' WHERE sid=%1").arg(season.sid).arg(TABLE_MOVIE_SEASON, season.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_RELEASE_DATE:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        QString date = "0000-00-00";
        if(season.release_date_valid)
        {
            date = season.release_date.toString("yyyy-MM-dd");
        }
        cmd = QString("UPDATE `%2` SET release_date='%3' WHERE sid=%1").arg(season.sid).arg(TABLE_MOVIE_SEASON, date);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_POINT:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        cmd = QString("UPDATE `%3` SET point=%1 WHERE sid=%2").arg(season.point).arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMoviePoint(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_COLLECT:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        cmd = QString("UPDATE `%3` SET collect=%1 WHERE sid=%2").arg(season.collect).arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMovieCollect(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_TAG1:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        cmd = QString("UPDATE `%3` SET tag1=%1 WHERE sid=%2").arg(season.tag1?1:0).arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMovieTag1(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_TAG2:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        cmd = QString("UPDATE `%3` SET tag2=%1 WHERE sid=%2").arg(season.tag2?1:0).arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMovieTag2(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_TAG3:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        cmd = QString("UPDATE `%3` SET tag3=%1 WHERE sid=%2").arg(season.tag3?1:0).arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMovieTag3(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_MOVIE_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_MOVIE_SEASON_SEE:
    {
        MovieSeasonData season = var.value<MovieSeasonData>();
        cmd = QString("UPDATE `%3` SET see=%1 WHERE sid=%2").arg(season.see?1:0).arg(season.sid).arg(TABLE_MOVIE_SEASON);
        mQuery.exec(cmd);

        calcMovieSee(season.pid);
        calcMovieRecent(season.pid, season.sid, season.name);

        emit SIGNALSendQueryData(SOT_UPDATE_MOVIE_SEASON_SEE, QVariant());
    }
        break;
    case SOT_DELETE_MOVIE_RECENT:
    {
        cmd = QString("DELETE FROM `%2` WHERE id=%1").arg(var.toInt()).arg(TABLE_MOVIE_RECENT);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_DELETE_MOVIE_RECENT, QVariant());
    }
        break;
    case SOT_SELECT_TV_IP:
    {
        QStringList strs = var.toStringList();
        int page_size = 20;
        int page = strs[0].toInt();
        int offset = (page-1)*page_size;
        cmd = QString("SELECT pid,name,keywords,see,see_season,"
                      "total_season,zhuiju,collect,point,display,"
                      "tag1,tag2,tag3 FROM `%1`").arg(TABLE_TV_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        cmd += QString(" ORDER BY CONVERT(name using gbk) LIMIT %1,%2").arg(offset).arg(page_size);
        if(mQuery.exec(cmd))
        {
            QList<TvIpData> ips;
            while(mQuery.next())
            {
                TvIpData ip;
                ip.pid = mQuery.value(0).toInt();
                ip.name = mQuery.value(1).toString();
                ip.keywords = mQuery.value(2).toString();
                ip.see = (mQuery.value(3).toInt() == 1);
                ip.see_season = mQuery.value(4).toInt();
                ip.total_season = mQuery.value(5).toInt();
                ip.zhuiju = (mQuery.value(6).toInt() == 1);
                ip.collect = mQuery.value(7).toInt();
                ip.point = mQuery.value(8).toInt();
                ip.display = (mQuery.value(9).toInt() == 1);
                ip.tag1 = (mQuery.value(10).toInt() == 1);
                ip.tag2 = (mQuery.value(11).toInt() == 1);
                ip.tag3 = (mQuery.value(12).toInt() == 1);
                ips.append(ip);
            }

            QVariant sendVar;
            sendVar.setValue(ips);
            emit SIGNALSendQueryData(SOT_SELECT_TV_IP, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_TV_IP);
        if(!strs[1].isEmpty())
        {
            cmd += strs[1];
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)page_size);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_TV_IP_PAGE, strs_send);
    }
        break;
    case SOT_SELECT_TV_SEASON:
    {
        QStringList strs = var.toStringList();
        int sid = strs[3].toInt();
        int page_size = 20;
        int page = strs[1].toInt();
        int offset = (page-1)*page_size;
        bool limit = (strs[2].toInt() == 1);
        cmd = QString("SELECT sid,pid,name,release_date,see,"
                      "see_episode,total_episode,collect,point,display,"
                      "tag1,tag2,tag3 FROM `%1` WHERE pid=%2").arg(TABLE_TV_SEASON, strs[0]);
        if(sid != -1)
        {   //最近模式
            cmd += QString(" AND sid=%1").arg(sid);
        }
        if(limit)
        {
            cmd += " AND display=1";
        }
        cmd += QString(" LIMIT %1,%2").arg(offset).arg(page_size);
        if(mQuery.exec(cmd))
        {
            QList<TvSeasonData> seasons;
            while(mQuery.next())
            {
                TvSeasonData season;
                season.sid = mQuery.value(0).toInt();
                season.pid = mQuery.value(1).toInt();
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

            QVariant send_var;
            send_var.setValue(seasons);
            emit SIGNALSendQueryData(SOT_SELECT_TV_SEASON, send_var);
        }
        cmd = QString("SELECT count(*) FROM `%1` WHERE pid=%2").arg(TABLE_TV_SEASON, strs[0]);
        if(limit)
        {
            cmd += " AND display = 1";
        }
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)page_size);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_TV_SEASON_PAGE, strs_send);
    }
        break;
    case SOT_SELECT_TV_EPISODE:
    {
        QStringList strs = var.toStringList();
        int pagesize = 24;
        int page = strs[2].toInt();
        int offset = (page-1)*pagesize;
        cmd = QString("SELECT eid,pid,sid,episode,title,"
                      "see,tag1,tag2,tag3 FROM `%3` WHERE pid=%4 AND sid=%5 LIMIT %1,%2").arg(offset).arg(pagesize)
                  .arg(TABLE_TV_EPISODE, strs[0], strs[1]);
        if(mQuery.exec(cmd))
        {
            QList<TvEpisodeData> eps;
            while(mQuery.next())
            {
                TvEpisodeData ep;
                ep.eid = mQuery.value(0).toInt();
                ep.pid = mQuery.value(1).toInt();
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
            emit SIGNALSendQueryData(SOT_SELECT_TV_EPISODE, sendVar);
        }
        cmd = QString("SELECT count(*) FROM `%1` WHERE pid=%2 AND sid=%3").arg(TABLE_TV_EPISODE, strs[0], strs[1]);
        mQuery.exec(cmd);
        mQuery.next();
        int total = mQuery.value(0).toInt();
        int total_page = qCeil(total/(double)pagesize);

        QStringList strs_send;
        strs_send << QString::number(page)
                  << QString::number(total_page)
                  << QString::number(total);
        emit SIGNALSendQueryData(SOT_INFO_TV_EPISODE_PAGE, strs_send);
    }
        break;
    case SOT_SELECT_TV_RECENT:
    {
        bool limit = var.toBool();
        QList<TvRecentData> recents;
        cmd = QString("SELECT id,pid,sid,name,display FROM `%1`%2 ORDER BY id DESC LIMIT 7").arg(TABLE_TV_RECENT, limit?" WHERE display=1":"");
        mQuery.exec(cmd);
        while(mQuery.next())
        {
            TvRecentData recent;
            recent.id = mQuery.value(0).toInt();
            recent.pid = mQuery.value(1).toInt();
            recent.sid = mQuery.value(2).toInt();
            recent.name = mQuery.value(3).toString();
            recent.display = (mQuery.value(4).toInt()==1);
            recents.append(recent);
        }

        QVariant var_send;
        var_send.setValue(recents);
        emit SIGNALSendQueryData(SOT_SELECT_TV_RECENT, var_send);
    }
        break;
    case SOT_INSERT_TV_IP:
    {
        TvIpData ip = var.value<TvIpData>();
        ip.name.replace("'", "''");
        ip.keywords.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (name,keywords,see,see_season,total_season,zhuiju,collect,point,display,tag1,tag2,tag3)"
                      " VALUES ('%2','%3',0,0,0,%4,0,0,%5,0,0,0)").arg(TABLE_TV_IP, ip.name, ip.keywords, ip.zhuiju?"1":"0", ip.display?"1":"0");
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_INSERT_TV_SEASON:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("INSERT INTO `%1` (pid,name,release_date,see,see_episode,total_episode,collect,point,display,tag1,tag2,tag3)"
                      " VALUES (%2,'%3','%4',0,0,0,%5,%6,%7,0,0,0)")
                  .arg(TABLE_TV_SEASON, QString::number(season.pid), season.name,
                       season.release_date_valid?season.release_date.toString("yyyy-MM-dd"):"0000-00-00",
                       QString::number(season.collect), QString::number(season.point), season.display?"1":"0");
        mQuery.exec(cmd);

        calcTvSee(season.pid);
        calcTvPoint(season.pid);
        calcTvCollect(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_INSERT_TV_EPISODE:
    {
        QList<TvEpisodeData> eps = var.value<QList<TvEpisodeData> >();
        int pid = eps.first().pid;
        int sid = eps.first().sid;
        cmd.clear();
        foreach(TvEpisodeData ep, eps)
        {
            cmd += QString("INSERT INTO `%6` (pid,sid,episode,title,see,tag1,tag2,tag3)"
                           " VALUES (%1,%2,'%7','%8',0,%3,%4,%5);")
                       .arg(pid).arg(sid).arg(ep.tag1).arg(ep.tag2).arg(ep.tag3).arg(TABLE_TV_EPISODE, ep.episode.replace("'", "''"), ep.title.replace("'", "''"));
        }
        mDB.transaction();
        mQuery.exec(cmd);

        calcTvSee(pid, sid);
        calcTvTag1(pid, sid);
        calcTvTag2(pid, sid);
        calcTvTag3(pid, sid);
        mQuery.clear();
        mDB.commit();

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_DELETE_TV_IP:
    {
        int pid = var.toInt();
        //删除关联最近
        cmd = QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_TV_RECENT);
        //删除关联集
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_TV_EPISODE);
        //删除关联部
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_TV_SEASON);
        //删除ip
        cmd += QString("DELETE FROM `%2` WHERE pid=%1;").arg(pid).arg(TABLE_TV_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_DELETE_TV_IP, QVariant());
    }
        break;
    case SOT_DELETE_TV_SEASON:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        //删除关联最近
        cmd = QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_TV_RECENT);
        //删除关联集
        cmd += QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_TV_EPISODE);
        //删除部
        cmd += QString("DELETE FROM `%2` WHERE sid=%1;").arg(season.sid).arg(TABLE_TV_SEASON);
        mQuery.exec(cmd);

        calcTvSee(season.pid);
        calcTvPoint(season.pid);
        calcTvCollect(season.pid);
        calcTvTag1(season.pid, season.sid);
        calcTvTag2(season.pid, season.sid);
        calcTvTag3(season.pid, season.sid);

        emit SIGNALSendQueryData(SOT_DELETE_TV_SEASON, QVariant());
    }
        break;
    case SOT_DELETE_TV_EPISODE:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        cmd = QString("DELETE FROM `%2` WHERE eid=%1").arg(episode.eid).arg(TABLE_TV_EPISODE);
        mQuery.exec(cmd);

        calcTvSee(episode.pid, episode.sid);
        calcTvTag1(episode.pid, episode.sid);
        calcTvTag2(episode.pid, episode.sid);
        calcTvTag3(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_DELETE_TV_RECENT:
    {
        cmd = QString("DELETE FROM `%2` WHERE id=%1").arg(var.toInt()).arg(TABLE_TV_RECENT);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_DELETE_TV_RECENT, QVariant());
    }
        break;
    case SOT_UPDATE_TV_IP_NAME:
    {
        TvIpData ip = var.value<TvIpData>();
        ip.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name='%3' WHERE pid=%1").arg(ip.pid).arg(TABLE_TV_IP, ip.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_IP_DISPLAY:
    {
        TvIpData ip = var.value<TvIpData>();
        cmd = QString("UPDATE `%3` SET display=%1 WHERE pid=%2").arg(ip.display?1:0).arg(ip.pid).arg(TABLE_TV_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_IP_ZHUIJU:
    {
        TvIpData ip = var.value<TvIpData>();
        cmd = QString("UPDATE `%3` SET zhuiju=%1 WHERE pid=%2").arg(ip.zhuiju?1:0).arg(ip.pid).arg(TABLE_TV_IP);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_IP_KEYWORDS:
    {
        TvIpData ip = var.value<TvIpData>();
        ip.keywords.replace("'", "''");
        cmd = QString("UPDATE `%2` SET keywords='%3' WHERE pid=%1").arg(ip.pid).arg(TABLE_TV_IP, ip.keywords);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_SEASON_NAME:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        season.name.replace("'", "''");
        cmd = QString("UPDATE `%2` SET name='%3' WHERE sid=%1").arg(season.sid).arg(TABLE_TV_SEASON, season.name);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_SEASON_DISPLAY:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        cmd = QString("UPDATE `%3` SET display=%1 WHERE sid=%2").arg(season.display).arg(season.sid).arg(TABLE_TV_SEASON);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_SEASON_RELEASE_DATE:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        QString date = "0000-00-00";
        if(season.release_date_valid)
        {
            date = season.release_date.toString("yyyy-MM-dd");
        }
        cmd = QString("UPDATE `%2` SET release_date='%3' WHERE sid=%1").arg(season.sid).arg(TABLE_TV_SEASON, date);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_SEASON_POINT:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        cmd = QString("UPDATE `%3` SET point=%1 WHERE sid=%2").arg(season.point).arg(season.sid).arg(TABLE_TV_SEASON);
        mQuery.exec(cmd);

        calcTvPoint(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_SEASON_COLLECT:
    {
        TvSeasonData season = var.value<TvSeasonData>();
        cmd = QString("UPDATE `%3` SET collect=%1 WHERE sid=%2").arg(season.collect).arg(season.sid).arg(TABLE_TV_SEASON);
        mQuery.exec(cmd);

        calcTvCollect(season.pid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_EPISODE_SEE:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        cmd = QString("UPDATE `%3` SET see=%1 WHERE eid=%2").arg(episode.see?1:0).arg(episode.eid).arg(TABLE_TV_EPISODE);
        mQuery.exec(cmd);

        calcTvSee(episode.pid, episode.sid);
        calcTvRecent(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_UPDATE_TV_EPISODE_SEE, QVariant());
    }
        break;
    case SOT_UPDATE_TV_EPISODE_EPISODE:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        episode.episode.replace("'", "''");
        cmd = QString("UPDATE `%2` SET episode='%3' WHERE eid=%1").arg(episode.eid).arg(TABLE_TV_EPISODE, episode.episode);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_EPISODE_TITLE:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        episode.title.replace("'", "''");
        cmd = QString("UPDATE `%2` SET title='%3' WHERE eid=%1").arg(episode.eid).arg(TABLE_TV_EPISODE, episode.title);
        mQuery.exec(cmd);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
        break;
    case SOT_UPDATE_TV_EPISODE_TAG1:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag1=%1 WHERE eid=%2").arg(episode.tag1?1:0).arg(episode.eid).arg(TABLE_TV_EPISODE);
        mQuery.exec(cmd);

        calcTvTag1(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
    break;
    case SOT_UPDATE_TV_EPISODE_TAG2:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag2=%1 WHERE eid=%2").arg(episode.tag2?1:0).arg(episode.eid).arg(TABLE_TV_EPISODE);
        mQuery.exec(cmd);

        calcTvTag2(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
    }
    break;
    case SOT_UPDATE_TV_EPISODE_TAG3:
    {
        TvEpisodeData episode = var.value<TvEpisodeData>();
        cmd = QString("UPDATE `%3` SET tag3=%1 WHERE eid=%2").arg(episode.tag3?1:0).arg(episode.eid).arg(TABLE_TV_EPISODE);
        mQuery.exec(cmd);

        calcTvTag3(episode.pid, episode.sid);

        emit SIGNALSendQueryData(SOT_TELL_TV_RESHOW, QVariant());
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
    mDB.setHostName(MYSQL_IP);
    mDB.setDatabaseName(MYSQL_DATABASE);
    mDB.setPort(MYSQL_PORT);
    mDB.setUserName(MYSQL_USER);
    mDB.setPassword(MYSQL_PWD);
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
void SqlThread::calcAnimeSee(int pid, int sid)
{
    //已看话
    QString cmd = QString("SELECT count(*) FROM `%3` WHERE pid=%1 AND sid=%2 AND see=1").arg(pid).arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int see_ep = mQuery.value(0).toInt();
    //总话
    cmd = QString("SELECT count(*) FROM `%3` WHERE pid=%1 AND sid=%2").arg(pid).arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int ep = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%6` SET see_episode=%1,total_episode=%2,see=%3 WHERE pid=%4 AND sid=%5")
            .arg(see_ep).arg(ep).arg((see_ep==ep && ep!=0)?1:0).arg(pid).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //已看季
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND see=1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();
    //总季
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int season = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%5` SET see_season=%1,total_season=%2,see=%3 WHERE pid=%4").arg(see_season).arg(season).arg((see_season==season && season!=0)?1:0).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算进度，季变化
void SqlThread::calcAnimeSee(int pid)
{
    //更新动漫表
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND see=1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();   //看了的季
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int total_season = mQuery.value(0).toInt(); //总的季
    cmd = QString("UPDATE `%5` SET see=%1,see_season=%2,total_season=%3 WHERE pid=%4")
            .arg((see_season==total_season && total_season!=0)?1:0).arg(see_season).arg(total_season).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算评分
void SqlThread::calcAnimePoint(int pid)
{
    //查找季最高评分
    QString cmd = QString("SELECT max(point) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int max_point = mQuery.value(0).toInt();
    //更新动漫最高评分
    cmd = QString("UPDATE `%3` SET point=%1 WHERE pid=%2").arg(max_point).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算收藏
void SqlThread::calcAnimeCollect(int pid)
{
    //查看季要收藏数
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND collect=1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_it = mQuery.value(0).toInt();
    //查看季已收藏数
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND collect=2").arg(pid).arg(TABLE_ANIME_SEASON);
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
    cmd = QString("UPDATE `%3` SET collect=%1 WHERE pid=%2").arg(collect).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算tag1
void SqlThread::calcAnimeTag1(int pid, int sid)
{
    //话总tag1
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag1=1 AND sid=%1").arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag1 = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%3` SET tag1=%1 WHERE sid=%2").arg(count_tag1>0?1:0).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //季总tag1
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag1=1 AND pid=%1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag1 = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%3` SET tag1=%1 WHERE pid=%2").arg(count_tag1>0?1:0).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算tag2
void SqlThread::calcAnimeTag2(int pid, int sid)
{
    //话总tag2
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag2=1 AND sid=%1").arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag2 = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%3` SET tag2=%1 WHERE sid=%2").arg(count_tag2>0?1:0).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //季总tag2
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag2=1 AND pid=%1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag2 = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%3` SET tag2=%1 WHERE pid=%2").arg(count_tag2>0?1:0).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///计算tag3
void SqlThread::calcAnimeTag3(int pid, int sid)
{
    //话总tag3
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag3=1 AND sid=%1").arg(sid).arg(TABLE_ANIME_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag3 = mQuery.value(0).toInt();
    //更新季
    cmd = QString("UPDATE `%3` SET tag3=%1 WHERE sid=%2").arg(count_tag3>0?1:0).arg(sid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    //季总tag3
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag3=1 AND pid=%1").arg(pid).arg(TABLE_ANIME_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag3 = mQuery.value(0).toInt();
    //更新动漫
    cmd = QString("UPDATE `%3` SET tag3=%1 WHERE pid=%2").arg(count_tag3>0?1:0).arg(pid).arg(TABLE_ANIME_IP);
    mQuery.exec(cmd);
}

///更新最近观看
void SqlThread::calcAnimeRecent(int pid, int sid)
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
        cmd = QString("INSERT INTO `%4` (pid,sid,name,display) VALUES (%1,%2,'%5',%3)").arg(pid).arg(sid).arg(display).arg(TABLE_ANIME_RECENT, name.replace("'", "''"));
        mQuery.exec(cmd);
    }
}

///计算进度
void SqlThread::calcMovieSee(int pid)
{
    //更新电影ip表
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND see=1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();   //看了的部
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int total_season = mQuery.value(0).toInt(); //总的部
    cmd = QString("UPDATE `%5` SET see=%1,see_season=%2,total_season=%3 WHERE pid=%4")
            .arg((see_season==total_season && total_season!=0)?1:0).arg(see_season).arg(total_season).arg(pid).arg(TABLE_MOVIE_IP);
    mQuery.exec(cmd);
}

///计算评分
void SqlThread::calcMoviePoint(int pid)
{
    //查找部最高评分
    QString cmd = QString("SELECT max(point) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int max_point = mQuery.value(0).toInt();
    //更新电影最高评分
    cmd = QString("UPDATE `%3` SET point=%1 WHERE pid=%2").arg(max_point).arg(pid).arg(TABLE_MOVIE_IP);
    mQuery.exec(cmd);
}

///计算收藏
void SqlThread::calcMovieCollect(int pid)
{
    //查看部要收藏数
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND collect=1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_it = mQuery.value(0).toInt();
    //查看部已收藏数
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND collect=2").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_ok = mQuery.value(0).toInt();
    //更新电影收藏
    int collect = 0;    //初始为无收藏
    if(count_collect_it > 0)
    {   //有要收藏，则标记为要收藏
        collect = 1;
    }
    else if(count_collect_ok > 0)
    {   //都是已收藏，则标记为已收藏
        collect = 2;
    }
    cmd = QString("UPDATE `%3` SET collect=%1 WHERE pid=%2").arg(collect).arg(pid).arg(TABLE_MOVIE_IP);
    mQuery.exec(cmd);
}

///计算tag1
void SqlThread::calcMovieTag1(int pid)
{
    //部总tag1
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag1=1 AND pid=%1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag1 = mQuery.value(0).toInt();
    //更新电影
    cmd = QString("UPDATE `%3` SET tag1=%1 WHERE pid=%2").arg(count_tag1>0?1:0).arg(pid).arg(TABLE_MOVIE_IP);
    mQuery.exec(cmd);
}

///计算tag2
void SqlThread::calcMovieTag2(int pid)
{
    //部总tag2
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag2=1 AND pid=%1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag2 = mQuery.value(0).toInt();
    //更新电影
    cmd = QString("UPDATE `%3` SET tag2=%1 WHERE pid=%2").arg(count_tag2>0?1:0).arg(pid).arg(TABLE_MOVIE_IP);
    mQuery.exec(cmd);
}

///计算tag3
void SqlThread::calcMovieTag3(int pid)
{
    //部总tag3
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag3=1 AND pid=%1").arg(pid).arg(TABLE_MOVIE_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag3 = mQuery.value(0).toInt();
    //更新电影
    cmd = QString("UPDATE `%3` SET tag3=%1 WHERE pid=%2").arg(count_tag3>0?1:0).arg(pid).arg(TABLE_MOVIE_IP);
    mQuery.exec(cmd);
}

///更新最近观看
void SqlThread::calcMovieRecent(int pid, int sid, QString name)
{
    QString cmd;

    bool need {false};  //是否插入记录
    cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_MOVIE_RECENT);
    mQuery.exec(cmd);
    mQuery.next();
    int count_recent = mQuery.value(0).toInt();
    if(count_recent == 0)
    {   //无记录，直接插入
        need = true;
    }
    else
    {   //有记录
        cmd = QString("SELECT sid FROM `%1` WHERE id=(SELECT MAX(id) FROM `%1`)").arg(TABLE_MOVIE_RECENT);
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
        //查看电影部显示
        cmd = QString("SELECT display FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_MOVIE_IP);
        mQuery.exec(cmd);
        mQuery.next();
        int display = mQuery.value(0).toInt();
        //插入
        cmd = QString("INSERT INTO `%4` (pid,sid,name,display) VALUES (%1,%2,'%5',%3)").arg(pid).arg(sid).arg(display).arg(TABLE_MOVIE_RECENT, name.replace("'", "''"));
        mQuery.exec(cmd);
    }
}

///计算进度
void SqlThread::calcTvSee(int pid, int sid)
{
    //已看集
    QString cmd = QString("SELECT count(*) FROM `%3` WHERE pid=%1 AND sid=%2 AND see=1").arg(pid).arg(sid).arg(TABLE_TV_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int see_ep = mQuery.value(0).toInt();
    //总集
    cmd = QString("SELECT count(*) FROM `%3` WHERE pid=%1 AND sid=%2").arg(pid).arg(sid).arg(TABLE_TV_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int ep = mQuery.value(0).toInt();
    //更新部
    cmd = QString("UPDATE `%6` SET see_episode=%1,total_episode=%2,see=%3 WHERE pid=%4 AND sid=%5")
              .arg(see_ep).arg(ep).arg((see_ep==ep && ep!=0)?1:0).arg(pid).arg(sid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    //已看部
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND see=1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();
    //总部
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int season = mQuery.value(0).toInt();
    //更新电视剧
    cmd = QString("UPDATE `%5` SET see_season=%1,total_season=%2,see=%3 WHERE pid=%4").arg(see_season).arg(season).arg((see_season==season && season!=0)?1:0).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///计算进度，部变化
void SqlThread::calcTvSee(int pid)
{
    //更新电视剧表
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND see=1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int see_season = mQuery.value(0).toInt();   //看了的部
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int total_season = mQuery.value(0).toInt(); //总的部
    cmd = QString("UPDATE `%5` SET see=%1,see_season=%2,total_season=%3 WHERE pid=%4")
              .arg((see_season==total_season && total_season!=0)?1:0).arg(see_season).arg(total_season).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///计算评分
void SqlThread::calcTvPoint(int pid)
{
    //查找季最高评分
    QString cmd = QString("SELECT max(point) FROM `%2` WHERE pid=%1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int max_point = mQuery.value(0).toInt();
    //更新电视剧最高评分
    cmd = QString("UPDATE `%3` SET point=%1 WHERE pid=%2").arg(max_point).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///计算收藏
void SqlThread::calcTvCollect(int pid)
{
    //查看部要收藏数
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND collect=1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_it = mQuery.value(0).toInt();
    //查看部已收藏数
    cmd = QString("SELECT count(*) FROM `%2` WHERE pid=%1 AND collect=2").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    int count_collect_ok = mQuery.value(0).toInt();
    //更新电视剧收藏
    int collect = 0;    //初始为无收藏
    if(count_collect_it > 0)
    {   //有要收藏，则标记为要收藏
        collect = 1;
    }
    else if(count_collect_ok > 0)
    {   //都是已收藏，则标记为已收藏
        collect = 2;
    }
    cmd = QString("UPDATE `%3` SET collect=%1 WHERE pid=%2").arg(collect).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///计算tag1
void SqlThread::calcTvTag1(int pid, int sid)
{
    //集总tag1
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag1=1 AND sid=%1").arg(sid).arg(TABLE_TV_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag1 = mQuery.value(0).toInt();
    //更新部
    cmd = QString("UPDATE `%3` SET tag1=%1 WHERE sid=%2").arg(count_tag1>0?1:0).arg(sid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    //部总tag1
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag1=1 AND pid=%1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag1 = mQuery.value(0).toInt();
    //更新电视剧
    cmd = QString("UPDATE `%3` SET tag1=%1 WHERE pid=%2").arg(count_tag1>0?1:0).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///计算tag2
void SqlThread::calcTvTag2(int pid, int sid)
{
    //集总tag2
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag2=1 AND sid=%1").arg(sid).arg(TABLE_TV_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag2 = mQuery.value(0).toInt();
    //更新部
    cmd = QString("UPDATE `%3` SET tag2=%1 WHERE sid=%2").arg(count_tag2>0?1:0).arg(sid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    //部总tag2
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag2=1 AND pid=%1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag2 = mQuery.value(0).toInt();
    //更新电视剧
    cmd = QString("UPDATE `%3` SET tag2=%1 WHERE pid=%2").arg(count_tag2>0?1:0).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///计算tag3
void SqlThread::calcTvTag3(int pid, int sid)
{
    //集总tag3
    QString cmd = QString("SELECT count(*) FROM `%2` WHERE tag3=1 AND sid=%1").arg(sid).arg(TABLE_TV_EPISODE);
    mQuery.exec(cmd);
    mQuery.next();
    int count_tag3 = mQuery.value(0).toInt();
    //更新部
    cmd = QString("UPDATE `%3` SET tag3=%1 WHERE sid=%2").arg(count_tag3>0?1:0).arg(sid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    //部总tag3
    cmd = QString("SELECT count(*) FROM `%2` WHERE tag3=1 AND pid=%1").arg(pid).arg(TABLE_TV_SEASON);
    mQuery.exec(cmd);
    mQuery.next();
    count_tag3 = mQuery.value(0).toInt();
    //更新电视剧
    cmd = QString("UPDATE `%3` SET tag3=%1 WHERE pid=%2").arg(count_tag3>0?1:0).arg(pid).arg(TABLE_TV_IP);
    mQuery.exec(cmd);
}

///更新最近观看
void SqlThread::calcTvRecent(int pid, int sid)
{
    QString cmd;

    bool need {false};  //是否插入记录
    cmd = QString("SELECT count(*) FROM `%1`").arg(TABLE_TV_RECENT);
    mQuery.exec(cmd);
    mQuery.next();
    int count_recent = mQuery.value(0).toInt();
    if(count_recent == 0)
    {   //无记录，直接插入
        need = true;
    }
    else
    {   //有记录
        cmd = QString("SELECT sid FROM `%1` WHERE id=(SELECT MAX(id) FROM `%1`)").arg(TABLE_TV_RECENT);
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
        //查看电视剧部显示、名称
        cmd = QString("SELECT display,name FROM `%2` WHERE sid=%1").arg(sid).arg(TABLE_TV_SEASON);
        mQuery.exec(cmd);
        mQuery.next();
        int display = mQuery.value(0).toInt();
        QString name = mQuery.value(1).toString();
        //插入
        cmd = QString("INSERT INTO `%4` (pid,sid,name,display) VALUES (%1,%2,'%5',%3)").arg(pid).arg(sid).arg(display).arg(TABLE_TV_RECENT, name.replace("'", "''"));
        mQuery.exec(cmd);
    }
}
