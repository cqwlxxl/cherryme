#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "UiThread/InterfacePublicData.h"
#include "UiThread/AnimeAnimeNewDialog.h"
#include "UiThread/AnimeSeasonNewDialog.h"
#include "UiThread/AnimeEpisodeNewDialog.h"

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
    void slotAnimeEpisodeSee(AnimeEpisodeData episode, int row);     //动漫话看完
    void on_pushButton_AnimeLogin_clicked(bool checked);    //连接|断开服务器
    void on_listWidget_Anime_Anime_itemClicked(QListWidgetItem *item);  //动漫点击
    void on_listWidget_Anime_Season_itemClicked(QListWidgetItem *item); //动漫季点击
    void on_listWidget_Anime_Ep_itemClicked(QListWidgetItem *item);     //动漫话点击
    void on_pushButton_AA_PrePage_clicked();    //动漫上一页
    void on_pushButton_AA_NextPage_clicked();   //动漫下一页
    void on_pushButton_AS_PrePage_clicked();    //动漫季上一页
    void on_pushButton_AS_NextPage_clicked();   //动漫季下一页
    void on_pushButton_AE_PrePage_clicked();    //动漫话上一页
    void on_pushButton_AE_NextPage_clicked();   //动漫话下一页
    void on_checkBox_LimitAnime_clicked(bool checked);  //Limit Anime

    void on_lineEdit_AE_Episode_textChanged(const QString &arg1);   //动漫话序号改变
    void on_pushButton_AE_EpisodeOk_clicked();  //动漫话序号提交
    void on_lineEdit_AE_Title_textChanged(const QString &arg1);     //动漫话标题改变
    void on_pushButton_AE_TitleOk_clicked();    //动漫话标题提交
    void on_checkBox_AE_Tag1_clicked(bool checked); //动漫话tag1提交
    void on_checkBox_AE_Tag2_clicked(bool checked); //动漫话tag2提交
    void on_checkBox_AE_Tag3_clicked(bool checked); //动漫话tag3提交
    void on_lineEdit_AS_Name_textChanged(const QString &arg1);      //动漫季名称改变
    void on_pushButton_AS_NaneOk_clicked();     //动漫季名称提交
    void on_checkBox_AS_ReleaseDateEnable_clicked(bool checked);    //发布日期有效
    void on_pushButton_AS_ReleaseDateOk_clicked();  //动漫季发布日期提交
    void on_dateEdit_AS_ReleaseDate_dateChanged(const QDate &date); //动漫季发布日期改变
    void on_comboBox_AS_Point_activated(int index);     //动漫季评分改变
    void on_checkBox_AA_Zhuifan_clicked(bool checked);  //动漫追番提交
    void on_checkBox_AS_CollectIt_clicked(bool checked);    //动漫季要收藏提交
    void on_checkBox_AS_CollectOk_clicked(bool checked);    //动漫季已收藏提交
    void on_pushButton_FindAnime_clicked();     //检索动漫
    void on_pushButton_FindAnimeReset_clicked();    //重置条件
    void on_lineEdit_AA_Name_textChanged(const QString &arg1);  //动漫名称改变
    void on_pushButton_AA_NaneOk_clicked(); //动漫名称提交
    void on_lineEdit_AA_Keyword_textChanged(const QString &arg1);   //动漫关键词改变
    void on_pushButton_AA_KeywordsOk_clicked(); //动漫关键词提交
    void on_pushButton_AA_New_clicked();    //新增动漫
    void on_pushButton_AA_Delete_clicked(); //删除动漫
    void on_pushButton_AS_New_clicked();    //新增动漫季
    void on_pushButton_AS_Delete_clicked(); //删除动漫季
    void on_pushButton_AE_New_clicked();    //新增动漫话
    void on_pushButton_AE_Delete_clicked(); //删除动漫话

private:
    void qian();    //init
    void getAnime(int page);    //获取动漫
    void getAnimeSeason(int page);  //获取动漫季
    void getAnimeEpisode(int page); //获取动漫话
    void showBarAnimeId(int what);  //显示id条
    void genFindAnimeSql();         //更新查找动漫条件

private:
    Ui::Widget *ui;

private:
    int     mConnectedAnime {false};
    int     mLimitAnime {true};
    int     mAAPageTotal {0};
    int     mASPageTotal {0};
    int     mAEPageTotal {0};
    QString mFindAnimeSql;
    AnimeAnimeNewDialog     *mAnimeAnimeNewDialog {nullptr};
    AnimeSeasonNewDialog    *mAnimeSeasonNewDialog {nullptr};
    AnimeEpisodeNewDialog   *mAnimeEpisodeNewDialog {nullptr};
};
#endif // WIDGET_H
