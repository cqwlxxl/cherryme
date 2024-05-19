#include "Widget.h"
#include "ui_Widget.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>

#include "UiThread/AnimeAnimeWidgetItem.h"

Q_DECLARE_METATYPE(AnimeData)
Q_DECLARE_METATYPE(AnimeSeasonData)
Q_DECLARE_METATYPE(AnimeEpisodeData)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    qian();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slotSendQuery(SqlOperateType operate, QVariant var)
{
    Q_UNUSED(var)
    switch(operate)
    {
    default:
        ui->tab_Anime->setEnabled(false);
        break;
    }
}

///接收sql结果槽函数
void Widget::slotReceiveQueryData(SqlOperateType operate, QVariant var)
{
    ui->tab_Anime->setEnabled(true);
    switch(operate)
    {
    case SOT_LOGIN_ANIME:
        mConnectedAnime = var.toBool();
        ui->pushButton_AnimeLogin->setText(mConnectedAnime?tr("断开服务器"):tr("连接服务器"));
        ui->pushButton_AnimeLogin->setChecked(mConnectedAnime);
        if(mConnectedAnime)
        {   //连接上服务器了
            on_pushButton_FindAnime_clicked();
        }
        else
        {   //断开服务器
            showBarAnimeId(-1);
            ui->listWidget_Anime_Anime->clear();
            ui->listWidget_Anime_Season->clear();
            ui->listWidget_Anime_Ep->clear();
        }
        break;
    case SOT_GET_ANIME_ANIME:
    {
        gIPD.anime.animes = var.value<QList<AnimeData> >();
        gIPD.anime.items.clear();
        gIPD.anime.widgets.clear();
        ui->listWidget_Anime_Anime->clear();
        ui->listWidget_Anime_Season->clear();
        ui->listWidget_Anime_Ep->clear();
        if(gIPD.anime.animes.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime.animes.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeAnimeWidgetItem *widget = new AnimeAnimeWidgetItem(gIPD.anime.animes.at(i), ui->listWidget_Anime_Anime);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_Anime_Anime->insertItem(i, item);
            ui->listWidget_Anime_Anime->setItemWidget(item, widget);
            gIPD.anime.items.append(item);
            gIPD.anime.widgets.append(widget);
        }
        if(gIPD.index_anime.a_click)
        {
            gIPD.index_anime.a_click = false;
            //查看是否还有显示
            bool has {false};
            for(int j = 0; j < gIPD.anime.animes.size(); j++)
            {
                if(gIPD.anime.animes.at(j).aid == gIPD.index_anime.aid)
                {
                    has = true;
                    break;
                }
            }
            if(has)
            {
                ui->listWidget_Anime_Anime->verticalScrollBar()->setValue(gIPD.index_anime.a_pos);
                ui->listWidget_Anime_Anime->setCurrentRow(gIPD.index_anime.a_row);
                on_listWidget_Anime_Anime_itemClicked(gIPD.anime.items.at(gIPD.index_anime.a_row));
            }
            else
            {
                gIPD.index_anime.s_click = false;
                gIPD.index_anime.e_click = false;
            }
        }
        break;
    }
    case SOT_ANIME_ANIME_PAGE:
    {
        QStringList strs = var.toStringList();
        mAAPageTotal = strs[1].toInt();
        ui->lineEdit_AA_Page->setText(strs[0]);
        ui->label_AA_PageTotal->setText("/"+strs[1]);
        ui->label_AA_Total->setText(QString(tr("共%1个系列")).arg(strs[2]));
    }
        break;
    case SOT_GET_ANIME_SEASON:
        gIPD.anime_season.seasons = var.value<QList<AnimeSeasonData> >();
        gIPD.anime_season.items.clear();
        gIPD.anime_season.widgets.clear();
        ui->listWidget_Anime_Season->clear();
        ui->listWidget_Anime_Ep->clear();
        if(gIPD.anime_season.seasons.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime_season.seasons.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeSeasonWidgetItem *widget = new AnimeSeasonWidgetItem(gIPD.anime_season.seasons.at(i), ui->listWidget_Anime_Season);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_Anime_Season->insertItem(i, item);
            ui->listWidget_Anime_Season->setItemWidget(item, widget);
            gIPD.anime_season.items.append(item);
            gIPD.anime_season.widgets.append(widget);
        }
        if(gIPD.index_anime.s_click)
        {
            gIPD.index_anime.s_click = false;
            ui->listWidget_Anime_Season->verticalScrollBar()->setValue(gIPD.index_anime.s_pos);
            ui->listWidget_Anime_Season->setCurrentRow(gIPD.index_anime.s_row);
            on_listWidget_Anime_Season_itemClicked(gIPD.anime_season.items.at(gIPD.index_anime.s_row));
        }
        break;
    case SOT_ANIME_SEASON_PAGE:
    {
        QStringList strs = var.toStringList();
        mASPageTotal = strs[1].toInt();
        ui->lineEdit_AS_Page->setText(strs[0]);
        ui->label_AS_PageTotal->setText("/"+strs[1]);
        ui->label_AS_Total->setText(QString(tr("共%1季")).arg(strs[2]));
    }
        break;
    case SOT_GET_ANIME_EPISODE:
        gIPD.anime_ep.eps = var.value<QList<AnimeEpisodeData> >();
        gIPD.anime_ep.items.clear();
        gIPD.anime_ep.widgets.clear();
        ui->listWidget_Anime_Ep->clear();
        if(gIPD.anime_ep.eps.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime_ep.eps.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeEpisodeWidgetItem *widget = new AnimeEpisodeWidgetItem(gIPD.anime_ep.eps.at(i), i, ui->listWidget_Anime_Ep);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_Anime_Ep->insertItem(i, item);
            ui->listWidget_Anime_Ep->setItemWidget(item, widget);
            gIPD.anime_ep.items.append(item);
            gIPD.anime_ep.widgets.append(widget);
        }
        if(gIPD.index_anime.e_click)
        {
            gIPD.index_anime.e_click = false;
            ui->listWidget_Anime_Ep->verticalScrollBar()->setValue(gIPD.index_anime.e_pos);
            ui->listWidget_Anime_Ep->setCurrentRow(gIPD.index_anime.e_row);
            on_listWidget_Anime_Ep_itemClicked(gIPD.anime_ep.items.at(gIPD.index_anime.e_row));
        }
        break;
    case SOT_ANIME_EPISODE_PAGE:
    {
        QStringList strs = var.toStringList();
        mAEPageTotal = strs[1].toInt();
        ui->lineEdit_AE_Page->setText(strs[0]);
        ui->label_AE_PageTotal->setText("/"+strs[1]);
        ui->label_AE_Total->setText(QString(tr("共%1话")).arg(strs[2]));
    }
        break;
    case SOT_UPDATE_ANIME:
        getAnime(ui->lineEdit_AA_Page->text().toInt());
        break;
    default:
        break;
    }
}

///动漫话看完
void Widget::slotAnimeEpisodeSee(AnimeEpisodeData episode, int row)
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    gIPD.index_anime.e_row = row;
    gIPD.index_anime.e_pos = ui->listWidget_Anime_Ep->verticalScrollBar()->value();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_SEE, var_send);
}

/////连接|断开服务器
void Widget::on_pushButton_AnimeLogin_clicked(bool checked)
{
    emit gIPD.SIGNALSendQuery(SOT_LOGIN_ANIME, checked);
    if(checked)
    {
        ui->pushButton_AnimeLogin->setText(tr("连接中.."));
    }
}

///init
void Widget::qian()
{
    mAnimeAnimeNewDialog = new AnimeAnimeNewDialog(this);
    mAnimeSeasonNewDialog = new AnimeSeasonNewDialog(this);
    mAnimeEpisodeNewDialog = new AnimeEpisodeNewDialog(this);
    ui->label_BarAAidText->setVisible(false);
    ui->label_BarASidText->setVisible(false);
    ui->label_BarAEidText->setVisible(false);
    ui->label_BarAAid->setVisible(false);
    ui->label_BarASid->setVisible(false);
    ui->label_BarAEid->setVisible(false);
    ui->dateEdit_AS_ReleaseDate->setVisible(false);
    ui->checkBox_AS_CollectOk->setEnabled(false);
    ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);
    connect(&gIPD, &InterfacePublicData::SIGNALSendQuery, this, &Widget::slotReceiveQueryData, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALReceiveQueryData, this, &Widget::slotReceiveQueryData, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALAnimeEpisodeSee, this, &Widget::slotAnimeEpisodeSee, Qt::UniqueConnection);

    //初始化后操作
    QTimer::singleShot(0, this, [this]{
        showBarAnimeId(-1);
    });
}

///获取动漫
void Widget::getAnime(int page)
{
    showBarAnimeId(0);
    QStringList strs;
    strs << QString::number(page)
         << mFindAnimeSql;
    emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_ANIME, strs);   //获取动漫
}

///获取动漫季
void Widget::getAnimeSeason(int page)
{
    showBarAnimeId(1);
    QStringList strs;
    strs << QString::number(gIPD.index_anime.aid)
         << QString::number(page)
         << QString::number(mLimitAnime?1:0);
    emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_SEASON, strs);   //获取动漫季
}

///获取动漫话
void Widget::getAnimeEpisode(int page)
{
    showBarAnimeId(2);
    QStringList strs;
    strs << QString::number(gIPD.index_anime.aid)
         << QString::number(gIPD.index_anime.sid)
         << QString::number(page);
    emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_EPISODE, strs);   //获取动漫话
}

///显示id条
void Widget::showBarAnimeId(int what)
{
    bool show_aa {false};
    bool show_as {false};
    bool show_ae {false};
    switch(what)
    {
    case -1:    //断开服务器
        ui->widget_AA_Op->setEnabled(false);
        ui->lineEdit_AA_Page->setText("0");
        ui->label_AA_PageTotal->setText("/0");
        ui->label_AA_Total->setText("少女祈祷中..");
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);
        break;
    case 0:     //连上了服务器
        ui->widget_AA_Op->setEnabled(true);
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);
        break;
    case 1:     //点击了aa
        show_aa = true;
        ui->label_BarAAid->setText(QString::number(gIPD.index_anime.aid));
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_AA);
        break;
    case 2:     //点击了as
        show_aa = true;
        show_as = true;
        ui->label_BarASid->setText(QString::number(gIPD.index_anime.sid));
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_AS);
        break;
    case 3:     //点击了ae
        show_aa = true;
        show_as = true;
        show_ae = true;
        ui->label_BarAEid->setText(QString::number(gIPD.index_anime.eid));
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_AE);
        break;
    default:
        break;
    }
    ui->label_BarAAidText->setVisible(show_aa);
    ui->label_BarASidText->setVisible(show_as);
    ui->label_BarAEidText->setVisible(show_ae);
    ui->label_BarAAid->setVisible(show_aa);
    ui->label_BarASid->setVisible(show_as);
    ui->label_BarAEid->setVisible(show_ae);
    ui->widget_AS_Op->setEnabled(show_aa);
    if(!show_aa)
    {
        ui->lineEdit_AS_Page->setText("0");
        ui->label_AS_PageTotal->setText("/0");
        ui->label_AS_Total->setText("少女祈祷中..");
    }
    ui->widget_AE_Op->setEnabled(show_as);
    if(!show_as)
    {
        ui->lineEdit_AE_Page->setText("0");
        ui->label_AE_PageTotal->setText("/0");
        ui->label_AE_Total->setText("少女祈祷中..");
    }
}

///更新查找动漫条件
void Widget::genFindAnimeSql()
{
    mFindAnimeSql.clear();
    if(!ui->checkBox_LimitAnime->isChecked())
    {
        mFindAnimeSql += " AND display = 1";
    }
    if(ui->checkBox_FindAnimeZhuifan->isChecked())
    {
        mFindAnimeSql += " AND zhuifan = 1";
    }
    if(ui->checkBox_FindAnimeNotsee->isChecked())
    {
        mFindAnimeSql += " AND see = 0";
    }
    int point_a = ui->comboBox_FindAnimePointA->currentIndex();
    int point_b = ui->comboBox_FindAnimePointB->currentIndex();
    int point_min = qMin(point_a, point_b);
    int point_max = qMax(point_a, point_b);
    if(point_min != 0 || point_max != 12)
    {
        if(point_min == point_max)
        {
            mFindAnimeSql += QString(" AND point = %1").arg(point_max);
        }
        else
        {
            mFindAnimeSql += QString(" AND (point BETWEEN %1 AND %2)").arg(point_min).arg(point_max);
        }
    }
    QString anime_name = ui->lineEdit_FindAnimeName->text().replace("'", "''");
    if(!anime_name.isEmpty())
    {
        mFindAnimeSql += QString(" AND (name LIKE '%%1%' OR keywords LIKE '%%1%')").arg(anime_name);
    }
    if(!mFindAnimeSql.isEmpty())
    {
        mFindAnimeSql.remove(0, 5);
        mFindAnimeSql = " WHERE " + mFindAnimeSql;
    }
}

///动漫点击
void Widget::on_listWidget_Anime_Anime_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_Anime_Anime->row(item);
    gIPD.index_anime.a_row = row;
    gIPD.index_anime.aid = gIPD.anime.animes.at(row).aid;
    gIPD.index_anime.a_pos = ui->listWidget_Anime_Anime->verticalScrollBar()->value();
    showBarAnimeId(1);
    getAnimeSeason(1);
    ui->checkBox_AA_Zhuifan->setChecked(gIPD.anime.animes.at(row).zhuifan);
    ui->lineEdit_AA_Name->setText(gIPD.anime.animes.at(row).name);
    ui->lineEdit_AA_Keyword->setText(gIPD.anime.animes.at(row).keywords);
}

///动漫季点击
void Widget::on_listWidget_Anime_Season_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_Anime_Season->row(item);
    AnimeSeasonData season = gIPD.anime_season.seasons.at(row);
    gIPD.index_anime.s_row = row;
    gIPD.index_anime.sid = season.sid;
    gIPD.index_anime.s_pos = ui->listWidget_Anime_Season->verticalScrollBar()->value();
    showBarAnimeId(2);
    getAnimeEpisode(1);
    ui->lineEdit_AS_Name->setText(season.name);
    if(season.release_date_valid)
    {
        ui->checkBox_AS_ReleaseDateEnable->setChecked(true);
        ui->dateEdit_AS_ReleaseDate->setDate(season.release_date);
        ui->dateEdit_AS_ReleaseDate->setVisible(true);
    }
    else
    {
        ui->checkBox_AS_ReleaseDateEnable->setChecked(false);
        ui->dateEdit_AS_ReleaseDate->setDate(QDate::currentDate());
        ui->dateEdit_AS_ReleaseDate->setVisible(false);
    }
    ui->comboBox_AS_Point->setCurrentIndex(season.point);
    if(season.collect == 0)
    {   //不收藏
        ui->checkBox_AS_CollectIt->setChecked(false);
        ui->checkBox_AS_CollectOk->setChecked(false);
        ui->checkBox_AS_CollectOk->setEnabled(false);
    }
    else if(season.collect == 1)
    {
        ui->checkBox_AS_CollectIt->setChecked(true);
        ui->checkBox_AS_CollectOk->setChecked(false);
        ui->checkBox_AS_CollectOk->setEnabled(true);
    }
    else if(season.collect == 2)
    {
        ui->checkBox_AS_CollectIt->setChecked(true);
        ui->checkBox_AS_CollectOk->setChecked(true);
        ui->checkBox_AS_CollectOk->setEnabled(true);
    }
}

///动漫话点击
void Widget::on_listWidget_Anime_Ep_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_Anime_Ep->row(item);
    gIPD.index_anime.e_row = row;
    gIPD.index_anime.eid = gIPD.anime_ep.eps.at(row).eid;
    gIPD.index_anime.e_pos = ui->listWidget_Anime_Ep->verticalScrollBar()->value();
    showBarAnimeId(3);
    ui->checkBox_AE_Tag1->setChecked(gIPD.anime_ep.eps.at(row).tag1);
    ui->checkBox_AE_Tag2->setChecked(gIPD.anime_ep.eps.at(row).tag2);
    ui->checkBox_AE_Tag3->setChecked(gIPD.anime_ep.eps.at(row).tag3);
    ui->lineEdit_AE_Episode->setText(gIPD.anime_ep.eps.at(row).episode);
    ui->lineEdit_AE_Title->setText(gIPD.anime_ep.eps.at(row).title);
}

///动漫上一页
void Widget::on_pushButton_AA_PrePage_clicked()
{
    if(mConnectedAnime)
    {
        int page = ui->lineEdit_AA_Page->text().toInt() - 1;
        if(page > 0)
        {
            getAnime(page);
        }
    }
}

///动漫下一页
void Widget::on_pushButton_AA_NextPage_clicked()
{
    if(mConnectedAnime)
    {
        int page = ui->lineEdit_AA_Page->text().toInt() + 1;
        if(page <= mAAPageTotal)
        {
            getAnime(page);
        }
    }
}

///动漫季上一页
void Widget::on_pushButton_AS_PrePage_clicked()
{
    if(mConnectedAnime)
    {
        int page = ui->lineEdit_AS_Page->text().toInt() - 1;
        if(page > 0)
        {
            getAnimeSeason(page);
        }
    }
}

///动漫季下一页
void Widget::on_pushButton_AS_NextPage_clicked()
{
    if(mConnectedAnime)
    {
        int page = ui->lineEdit_AS_Page->text().toInt() + 1;
        if(page <= mASPageTotal)
        {
            getAnimeSeason(page);
        }
    }
}

///动漫话上一页
void Widget::on_pushButton_AE_PrePage_clicked()
{
    if(mConnectedAnime)
    {
        int page = ui->lineEdit_AE_Page->text().toInt() - 1;
        if(page > 0)
        {
            getAnimeEpisode(page);
        }
    }
}

///动漫话下一页
void Widget::on_pushButton_AE_NextPage_clicked()
{
    if(mConnectedAnime)
    {
        int page = ui->lineEdit_AE_Page->text().toInt() + 1;
        if(page <= mAEPageTotal)
        {
            getAnimeEpisode(page);
        }
    }
}

///Limit Anime
void Widget::on_checkBox_LimitAnime_clicked(bool checked)
{
    if(checked)
    {
        if(ui->lineEdit_LimitAnime->text() == "showmeall123")
        {
            mLimitAnime = false;
            getAnime(1);
        }
        else
        {
            ui->checkBox_LimitAnime->setChecked(false);
        }
    }
    else
    {
        mLimitAnime = true;
        getAnime(1);
    }
    ui->lineEdit_LimitAnime->clear();
}

///动漫话序号改变
void Widget::on_lineEdit_AE_Episode_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime_ep.eps.at(gIPD.index_anime.e_row).episode)
    {
        ui->lineEdit_AE_Episode->setStyleSheet("#lineEdit_AE_Episode{background-color:#fcae74;}");
        ui->pushButton_AE_EpisodeOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AE_Episode->setStyleSheet("");
        ui->pushButton_AE_EpisodeOk->setEnabled(false);
    }
}

///动漫话序号提交
void Widget::on_pushButton_AE_EpisodeOk_clicked()
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    ui->lineEdit_AE_Episode->setStyleSheet("");
    ui->pushButton_AE_EpisodeOk->setEnabled(false);
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.episode = ui->lineEdit_AE_Episode->text().replace("'", "''");
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_EPISODE, var_send);
}

///动漫话标题改变
void Widget::on_lineEdit_AE_Title_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime_ep.eps.at(gIPD.index_anime.e_row).title)
    {
        ui->lineEdit_AE_Title->setStyleSheet("#lineEdit_AE_Title{background-color:#fcae74;}");
        ui->pushButton_AE_TitleOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AE_Title->setStyleSheet("");
        ui->pushButton_AE_TitleOk->setEnabled(false);
    }
}

///动漫话标题提交
void Widget::on_pushButton_AE_TitleOk_clicked()
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    ui->lineEdit_AE_Title->setStyleSheet("");
    ui->pushButton_AE_TitleOk->setEnabled(false);
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.title = ui->lineEdit_AE_Title->text().replace("'", "''");
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_TITLE, var_send);
}

///动漫话tag1提交
void Widget::on_checkBox_AE_Tag1_clicked(bool checked)
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.tag1 = checked;
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_TAG1, var_send);
}

///动漫话tag2提交
void Widget::on_checkBox_AE_Tag2_clicked(bool checked)
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.tag2 = checked;
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_TAG2, var_send);
}

///动漫话tag3提交
void Widget::on_checkBox_AE_Tag3_clicked(bool checked)
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.tag3 = checked;
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_TAG3, var_send);
}

///动漫季名称改变
void Widget::on_lineEdit_AS_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).name)
    {
        ui->lineEdit_AS_Name->setStyleSheet("#lineEdit_AS_Name{background-color:#fcae74;}");
        ui->pushButton_AS_NaneOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AS_Name->setStyleSheet("");
        ui->pushButton_AS_NaneOk->setEnabled(false);
    }
}

///动漫季名称提交
void Widget::on_pushButton_AS_NaneOk_clicked()
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AS_Name->setStyleSheet("");
    ui->pushButton_AS_NaneOk->setEnabled(false);
    AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
    season.name = ui->lineEdit_AS_Name->text().replace("'", "''");
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_NAME, var_send);
}

///发布日期有效
void Widget::on_checkBox_AS_ReleaseDateEnable_clicked(bool checked)
{
    ui->dateEdit_AS_ReleaseDate->setVisible(checked);
    bool can_update = false;
    if(checked && (gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).release_date != ui->dateEdit_AS_ReleaseDate->date()))
    {
        can_update = true;
    }
    if(checked != gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).release_date_valid)
    {
        can_update = true;
    }
    ui->pushButton_AS_ReleaseDateOk->setEnabled(can_update);
}

///动漫季发布日期提交
void Widget::on_pushButton_AS_ReleaseDateOk_clicked()
{
    ui->pushButton_AS_ReleaseDateOk->setEnabled(false);
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
    season.release_date_valid = ui->checkBox_AS_ReleaseDateEnable->isChecked();
    season.release_date = ui->dateEdit_AS_ReleaseDate->date();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_RELEASE_DATE, var_send);
}

///动漫季发布日期改变
void Widget::on_dateEdit_AS_ReleaseDate_dateChanged(const QDate &date)
{
    bool can_update = false;
    if(ui->checkBox_AS_ReleaseDateEnable->isChecked() && (gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).release_date != date))
    {
        can_update = true;
    }
    if(ui->checkBox_AS_ReleaseDateEnable->isChecked() != gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).release_date_valid)
    {
        can_update = true;
    }
    ui->pushButton_AS_ReleaseDateOk->setEnabled(can_update);
}

///动漫季评分改变
void Widget::on_comboBox_AS_Point_activated(int index)
{
    if(gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).point != index)
    {
        gIPD.index_anime.a_click = true;
        gIPD.index_anime.s_click = true;
        gIPD.index_anime.e_click = false;
        AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
        season.point = index;
        QVariant var_send;
        var_send.setValue(season);
        emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_POINT, var_send);
    }
}

///动漫追番提交
void Widget::on_checkBox_AA_Zhuifan_clicked(bool checked)
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.a_row);
    anime.zhuifan = checked;
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_ANIME_ZHUIFAN, var_send);
}

///动漫季要收藏提交
void Widget::on_checkBox_AS_CollectIt_clicked(bool checked)
{
    ui->checkBox_AS_CollectOk->setEnabled(checked);
    ui->checkBox_AS_CollectOk->setChecked(false);
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
    season.collect = checked ? 1 : 0;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_COLLECT, var_send);
}

///动漫季已收藏提交
void Widget::on_checkBox_AS_CollectOk_clicked(bool checked)
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
    season.collect = checked ? 2 : 1;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_COLLECT, var_send);
}

///检索动漫
void Widget::on_pushButton_FindAnime_clicked()
{
    if(mConnectedAnime)
    {
        genFindAnimeSql();
        getAnime(1);
    }
}

///重置条件
void Widget::on_pushButton_FindAnimeReset_clicked()
{
    ui->lineEdit_FindAnimeName->clear();
    ui->checkBox_FindAnimeZhuifan->setChecked(false);
    ui->checkBox_FindAnimeNotsee->setChecked(false);
    ui->comboBox_FindAnimePointA->setCurrentIndex(0);
    ui->comboBox_FindAnimePointB->setCurrentIndex(12);
}

///动漫名称改变
void Widget::on_lineEdit_AA_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime.animes.at(gIPD.index_anime.a_row).name)
    {
        ui->lineEdit_AA_Name->setStyleSheet("#lineEdit_AA_Name{background-color:#fcae74;}");
        ui->pushButton_AA_NaneOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AA_Name->setStyleSheet("");
        ui->pushButton_AA_NaneOk->setEnabled(false);
    }
}

///动漫名称提交
void Widget::on_pushButton_AA_NaneOk_clicked()
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AA_Name->setStyleSheet("");
    ui->pushButton_AA_NaneOk->setEnabled(false);
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.a_row);
    anime.name = ui->lineEdit_AA_Name->text().replace("'", "''");
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_ANIME_NAME, var_send);
}

///动漫关键词改变
void Widget::on_lineEdit_AA_Keyword_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime.animes.at(gIPD.index_anime.a_row).keywords)
    {
        ui->lineEdit_AA_Keyword->setStyleSheet("#lineEdit_AA_Keyword{background-color:#fcae74;}");
        ui->pushButton_AA_KeywordsOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AA_Keyword->setStyleSheet("");
        ui->pushButton_AA_KeywordsOk->setEnabled(false);
    }
}

///动漫关键词提交
void Widget::on_pushButton_AA_KeywordsOk_clicked()
{
    gIPD.index_anime.a_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AA_Keyword->setStyleSheet("");
    ui->pushButton_AA_KeywordsOk->setEnabled(false);
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.a_row);
    anime.keywords = ui->lineEdit_AA_Keyword->text().replace("'", "''");
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_ANIME_KEYWORDS, var_send);
}

///新增动漫
void Widget::on_pushButton_AA_New_clicked()
{
    mAnimeAnimeNewDialog->Hi();
}

///删除动漫
void Widget::on_pushButton_AA_Delete_clicked()
{
    int row = ui->listWidget_Anime_Anime->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择动漫"), tr("请选择要删除的动漫"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除动漫《%1》?\n注意: 其包含的所有季度和话数据均会被关联删除!")).arg(gIPD.anime.animes.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_ANIME, gIPD.anime.animes.at(row).aid);
        }
    }
}

///新增动漫季
void Widget::on_pushButton_AS_New_clicked()
{
    mAnimeSeasonNewDialog->Hi(gIPD.index_anime.aid);
}

///删除动漫季
void Widget::on_pushButton_AS_Delete_clicked()
{
    int row = ui->listWidget_Anime_Season->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择季度"), tr("请选择要删除的季度"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除季度《%1》?\n注意: 其包含的所有话数据均会被关联删除!")).arg(gIPD.anime_season.seasons.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            gIPD.index_anime.a_click = true;
            gIPD.index_anime.s_click = false;
            gIPD.index_anime.e_click = false;
            QVariant var_send;
            var_send.setValue(gIPD.anime_season.seasons.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_SEASON, var_send);
        }
    }
}

///新增动漫话
void Widget::on_pushButton_AE_New_clicked()
{
    mAnimeEpisodeNewDialog->Hi(gIPD.index_anime.aid, gIPD.index_anime.sid);
}

///删除动漫话
void Widget::on_pushButton_AE_Delete_clicked()
{
    int row = ui->listWidget_Anime_Ep->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择话"), tr("请选择要删除的话"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除话《[%1] %2》?")).arg(gIPD.anime_ep.eps.at(row).episode, gIPD.anime_ep.eps.at(row).title), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            gIPD.index_anime.a_click = true;
            gIPD.index_anime.s_click = true;
            gIPD.index_anime.e_click = false;
            QVariant var_send;
            var_send.setValue(gIPD.anime_ep.eps.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_EPISODE, var_send);
        }
    }
}

