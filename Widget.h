#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QWidget>

#include "UiThread/InterfacePublicData.h"
#include "UiThread/Anime/AnimeIpNewDialog.h"
#include "UiThread/Anime/AnimeSeasonNewDialog.h"
#include "UiThread/Anime/AnimeEpisodeNewDialog.h"
#include "UiThread/Movie/MovieIpNewDialog.h"
#include "UiThread/Movie/MovieSeasonNewDialog.h"

struct AnimeRecentModeData
{
    bool    enable {false}; //启用
    int     pid {0};        //pid
    int     sid {0};        //sid
    QString name;           //季名称
};

struct MovieRecentModeData
{
    bool    enable {false}; //启用
    int     pid {0};        //pid
    int     sid {0};        //sid
    QString name;           //部名称
};

struct TvRecentModeData
{
    bool    enable {false}; //启用
    int     pid {0};        //pid
    int     sid {0};        //sid
    QString name;           //部名称
};

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void slotSendQuery(SqlOperateType operate, QVariant var);
    void slotReceiveQueryData(SqlOperateType operate, QVariant var);    //接收sql结果槽函数
    void slotAnimeEpisodeSee(AnimeEpisodeData episode, int row);        //动漫话看完
    void slotMovieSeasonSee(MovieSeasonData season, int row);           //电影部看完
    void slotTvEpisodeSee(TvEpisodeData episode, int row);              //电视剧集看完

private slots:
    void on_pushButton_ConnectMysql_clicked(bool checked);    //连接|断开服务器
    void on_checkBox_Limit_clicked(bool checked);  //Limit
    ///anime相关
    //anime检索
    void on_pushButton_FindAnimeReset_clicked();    //重置条件
    void on_pushButton_FindAnime_clicked();         //检索动漫
    //anime条目
    void on_listWidget_AP_itemClicked(QListWidgetItem *item);   //动漫ip点击
    void on_listWidget_AS_itemClicked(QListWidgetItem *item);   //动漫季点击
    void on_listWidget_AE_itemClicked(QListWidgetItem *item);   //动漫话点击
    //anime翻页增删
    void on_pushButton_AP_PrePage_clicked();    //动漫ip上一页
    void on_pushButton_AP_NextPage_clicked();   //动漫ip下一页
    void on_pushButton_AS_PrePage_clicked();    //动漫季上一页
    void on_pushButton_AS_NextPage_clicked();   //动漫季下一页
    void on_pushButton_AE_PrePage_clicked();    //动漫话上一页
    void on_pushButton_AE_NextPage_clicked();   //动漫话下一页
    void on_pushButton_AP_New_clicked();    //新增动漫ip
    void on_pushButton_AP_Delete_clicked(); //删除动漫ip
    void on_pushButton_AS_New_clicked();    //新增动漫季
    void on_pushButton_AS_Delete_clicked(); //删除动漫季
    void on_pushButton_AE_New_clicked();    //新增动漫话
    void on_pushButton_AE_Delete_clicked(); //删除动漫话
    //anime操作ip
    void on_lineEdit_AP_Name_textChanged(const QString &arg1);  //动漫名称改变
    void on_pushButton_AP_NameOk_clicked();                     //动漫名称提交
    void on_checkBox_AP_Display_clicked(bool checked);  //动漫公开提交
    void on_checkBox_AP_Zhuifan_clicked(bool checked);  //动漫追番提交
    void on_lineEdit_AP_Keyword_textChanged(const QString &arg1);   //动漫关键词改变
    void on_pushButton_AP_KeywordsOk_clicked();                     //动漫关键词提交
    //anime操作季
    void on_lineEdit_AS_Name_textChanged(const QString &arg1);  //动漫季名称改变
    void on_pushButton_AS_NaneOk_clicked();                     //动漫季名称提交
    void on_checkBox_AS_Display_clicked(bool checked);  //动漫季公开提交
    void on_checkBox_AS_ReleaseDateEnable_clicked(bool checked);    //动漫季发布日期有效
    void on_dateEdit_AS_ReleaseDate_dateChanged(const QDate &date); //动漫季发布日期改变
    void on_pushButton_AS_ReleaseDateOk_clicked();                  //动漫季发布日期提交
    void on_comboBox_AS_Point_activated(int index);     //动漫季评分改变
    void on_checkBox_AS_CollectIt_clicked(bool checked);    //动漫季要收藏提交
    void on_checkBox_AS_CollectOk_clicked(bool checked);    //动漫季已收藏提交
    //anime操作话
    void on_lineEdit_AE_Episode_textChanged(const QString &arg1);   //动漫话序号改变
    void on_pushButton_AE_EpisodeOk_clicked();                      //动漫话序号提交
    void on_lineEdit_AE_Title_textChanged(const QString &arg1);     //动漫话标题改变
    void on_pushButton_AE_TitleOk_clicked();                        //动漫话标题提交
    void on_checkBox_AE_Tag1_clicked(bool checked); //动漫话tag1提交
    void on_checkBox_AE_Tag2_clicked(bool checked); //动漫话tag2提交
    void on_checkBox_AE_Tag3_clicked(bool checked); //动漫话tag3提交
    ///movie相关
    //movie检索
    void on_pushButton_FindMovieReset_clicked();    //重置条件
    void on_pushButton_FindMovie_clicked();         //检索电影
    //movie条目
    void on_listWidget_MP_itemClicked(QListWidgetItem *item);   //电影ip点击
    void on_listWidget_MS_itemClicked(QListWidgetItem *item);   //电影部点击
    //movie翻页增删
    void on_pushButton_MP_PrePage_clicked();    //电影ip上一页
    void on_pushButton_MP_NextPage_clicked();   //电影ip下一页
    void on_pushButton_MS_PrePage_clicked();    //电影季上一页
    void on_pushButton_MS_NextPage_clicked();   //电影季下一页
    void on_pushButton_MP_New_clicked();        //新增电影ip
    void on_pushButton_MP_Delete_clicked();     //删除电影ip
    void on_pushButton_MS_New_clicked();        //新增电影部
    void on_pushButton_MS_Delete_clicked();     //删除电影部
    //movie操作ip
    void on_lineEdit_MP_Name_textChanged(const QString &arg1);  //电影名称改变
    void on_pushButton_MP_NameOk_clicked();                     //电影名称提交
    void on_checkBox_MP_Display_clicked(bool checked);  //电影公开提交
    void on_lineEdit_MP_Keyword_textChanged(const QString &arg1);   //电影关键词改变
    void on_pushButton_MP_KeywordsOk_clicked();                     //电影关键词提交
    //movie操作部
    void on_lineEdit_MS_Name_textChanged(const QString &arg1);  //电影部名称改变
    void on_pushButton_MS_NaneOk_clicked();                     //电影部名称提交
    void on_checkBox_MS_ReleaseDateEnable_clicked(bool checked);    //电影部发布日期有效
    void on_dateEdit_MS_ReleaseDate_dateChanged(const QDate &date); //电影部发布日期改变
    void on_pushButton_MS_ReleaseDateOk_clicked();                  //电影部发布日期提交
    void on_comboBox_MS_Point_activated(int index);     //电影部评分改变
    void on_checkBox_MS_CollectIt_clicked(bool checked);    //电影部要收藏提交
    void on_checkBox_MS_CollectOk_clicked(bool checked);    //电影部已收藏提交
    void on_checkBox_MS_Tag1_clicked(bool checked);     //电影部tag1提交
    void on_checkBox_MS_Tag2_clicked(bool checked);     //电影部tag2提交
    void on_checkBox_MS_Tag3_clicked(bool checked);     //电影部tag3提交
    ///tv相关
    //tv检索
    void on_pushButton_FindTvReset_clicked();       //重置条件
    void on_pushButton_FindTv_clicked();            //检索电视剧
    //tv条目
    void on_listWidget_TP_itemClicked(QListWidgetItem *item);   //电视剧ip点击
    void on_listWidget_TS_itemClicked(QListWidgetItem *item);   //电视剧部点击
    void on_listWidget_TE_itemClicked(QListWidgetItem *item);   //电视剧集点击
    //tv翻页增删
    void on_pushButton_TP_PrePage_clicked();    //电视剧ip上一页
    void on_pushButton_TP_NextPage_clicked();   //电视剧ip下一页
    void on_pushButton_TS_PrePage_clicked();    //电视剧部上一页
    void on_pushButton_TS_NextPage_clicked();   //电视剧部下一页
    void on_pushButton_TE_PrePage_clicked();    //电视剧集上一页
    void on_pushButton_TE_NextPage_clicked();   //电视剧集下一页

private:
    void qian();    //init
    //anime
    void getAnimeIp(int page);      //获取动漫ip
    void getAnimeSeason(int page);  //获取动漫季
    void getAnimeEpisode(int page); //获取动漫话
    void showBarAnimeId(int what);  //显示动漫id条
    void genFindAnimeSql();         //更新查找动漫条件
    void setAnimeRecentLabel();     //设置最近观看
    void showAnimeRecent(int index);    //显示最近观看
    void closeAnimeRecent(int index);   //关闭最近观看
    //movie
    void getMovieIp(int page);      //获取电影ip
    void getMovieSeason(int page);  //获取电影部
    void showBarMovieId(int what);  //显示电影id条
    void genFindMovieSql();         //更新查找电影条件
    void setMovieRecentLabel();     //设置最近观看
    void showMovieRecent(int index);    //显示最近观看
    void closeMovieRecent(int index);   //关闭最近观看
    //tv
    void getTvIp(int page);         //获取电视剧ip
    void getTvSeason(int page);     //获取电视剧部
    void getTvEpisode(int page);    //获取电视剧集
    void showBarTvId(int what); //显示电视剧id条
    void genFindTvSql();        //更新查找电视剧条件

protected:
    bool eventFilter(QObject *obj, QEvent *event);  //事件过滤

private:
    Ui::Widget *ui;

private:
    bool    mConnectedMysql {false};
    bool    mLimit {true};
    AnimeRecentModeData mAnimeRecentMode;
    MovieRecentModeData mMovieRecentMode;
    TvRecentModeData    mTvRecentMode;
    int     mAPPageTotal {0};
    int     mASPageTotal {0};
    int     mAEPageTotal {0};
    int     mMPPageTotal {0};
    int     mMSPageTotal {0};
    int     mTPPageTotal {0};
    int     mTSPageTotal {0};
    int     mTEPageTotal {0};
    QString mFindAnimeSql;
    QString mFindMovieSql;
    QString mFindTvSql;
    AnimeIpNewDialog        *mAnimeIpNewDialog {nullptr};
    AnimeSeasonNewDialog    *mAnimeSeasonNewDialog {nullptr};
    AnimeEpisodeNewDialog   *mAnimeEpisodeNewDialog {nullptr};
    MovieIpNewDialog        *mMovieIpNewDialog {nullptr};
    MovieSeasonNewDialog    *mMovieSeasonNewDialog {nullptr};
    QList<QPair<QLabel *, QLabel *> >   mAnimeRecentLabels;
    QList<QPair<QLabel *, QLabel *> >   mMovieRecentLabels;
    QList<AnimeRecentData>  mAnimeRecents;
    QList<MovieRecentData>  mMovieRecents;
    QList<TvRecentData>     mTvRecents;
};
#endif // WIDGET_H
