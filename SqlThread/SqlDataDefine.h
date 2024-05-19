#ifndef SQLDATADEFINE_H
#define SQLDATADEFINE_H

#include <QDate>
#include <QString>

#define TABLE_ANIME_IP      "anime_ip"
#define TABLE_ANIME_SEASON  "anime_season"
#define TABLE_ANIME_EPISODE "anime_episode"
#define TABLE_ANIME_RECENT  "anime_recent"

#define MYSQL_IP        "localhost"
#define MYSQL_PORT      3306
#define MYSQL_DATABASE  "cherryme"
#define MYSQL_USER      "cherryme"
#define MYSQL_PWD       "cherryme"

///数据操作
enum SqlOperateType
{
    SOT_LOGIN_ANIME,        //连接anime
    SOT_GET_ANIME_ANIME,    //获取动漫
    SOT_GET_ANIME_RECENT,   //获取动漫，最近观看
    SOT_ANIME_ANIME_PAGE,   //动漫页面
    SOT_GET_ANIME_SEASON,   //获取动漫季
    SOT_ANIME_SEASON_PAGE,  //动漫季页面
    SOT_GET_ANIME_EPISODE,  //获取动漫话
    SOT_ANIME_EPISODE_PAGE, //动漫话页面
    SOT_UPDATE_ANIME,                       //更新动漫
    SOT_UPDATE_ANIME_ANIME_NAME,            //更新动漫，名称
    SOT_UPDATE_ANIME_ANIME_KEYWORDS,        //更新动漫，关键词
    SOT_UPDATE_ANIME_ANIME_ZHUIFAN,         //更新动漫，追番
    SOT_UPDATE_ANIME_ANIME_DISPLAY,         //更新动漫，公开
    SOT_UPDATE_ANIME_SEASON_COLLECT,        //更新动漫，收藏
    SOT_UPDATE_ANIME_SEASON_NAME,           //更新动漫季，名称
    SOT_UPDATE_ANIME_SEASON_RELEASE_DATE,   //更新动漫季，发布日期
    SOT_UPDATE_ANIME_SEASON_POINT,          //更新动漫季，评分
    SOT_UPDATE_ANIME_SEASON_DISPLAY,        //更新动漫季，公开
    SOT_UPDATE_ANIME_EPISODE_SEE,           //更新动漫话，看完
    SOT_UPDATE_ANIME_EPISODE_EPISODE,       //更新动漫话，序号
    SOT_UPDATE_ANIME_EPISODE_TITLE,         //更新动漫话，标题
    SOT_UPDATE_ANIME_EPISODE_TAG1,          //更新动漫话，tag1
    SOT_UPDATE_ANIME_EPISODE_TAG2,          //更新动漫话，tag2
    SOT_UPDATE_ANIME_EPISODE_TAG3,          //更新动漫话，tag3
    SOT_INSERT_ANIME_ANIME,     //新增动漫
    SOT_INSERT_ANIME_SEASON,    //新增动漫季
    SOT_INSERT_ANIME_EPISODE,   //新增动漫话
    SOT_DELETE_ANIME_ANIME,     //删除动漫
    SOT_DELETE_ANIME_SEASON,    //删除动漫季
    SOT_DELETE_ANIME_EPISODE,   //删除动漫话
    SOT_DELETE_ANIME_RECENT,    //删除动漫最近观看
};

struct AnimeData
{
    //MySQL中，ALL int
    int     pid {0};            //pid
    QString name;               //名称
    QString keywords;           //关键词
    bool    see {false};        //看完
    int     see_season {0};     //看了的季
    int     total_season {0};   //总的季
    bool    zhuifan {false};    //追番
    int     collect {0};        //收藏
    int     point {0};          //评分
    bool    display {false};    //可见
    bool    tag1 {false};       //tag1
    bool    tag2 {false};       //tag2
    bool    tag3 {true};        //tag3
};

struct AnimeSeasonData
{
    int     sid {0};                    //sid
    int     pid {0};                    //pid
    QString name;                       //名称
    QDate   release_date;               //发布日期
    bool    release_date_valid {false}; //发布日期是否有效
    bool    see {false};                //看完
    int     see_episode {0};            //看了的话
    int     total_episode {0};          //总的话
    int     collect {0};                //收藏
    int     point {0};                  //评分
    bool    display {false};            //可见
    bool    tag1 {false};               //tag1
    bool    tag2 {false};               //tag2
    bool    tag3 {true};                //tag3
};

struct AnimeEpisodeData
{
    int     eid {0};        //eid
    int     pid {0};        //pid
    int     sid {0};        //sid
    QString episode {"XX"}; //话序号
    QString title;          //标题
    bool    see {false};    //看完
    bool    tag1 {false};   //tag1
    bool    tag2 {false};   //tag2
    bool    tag3 {true};    //tag3
};

struct AnimeRecentData
{
    int     id {0};             //id
    int     pid {0};            //pid
    int     sid {0};            //sid
    QString name;               //季度名称
    bool    display {false};    //可见
};

#endif // SQLDATADEFINE_H
