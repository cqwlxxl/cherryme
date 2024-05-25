#include "Widget.h"
#include "ui_Widget.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>

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

PublicUseData   gPUD;

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
    case SOT_CONNECT_MYSQL:
        mConnectedMysql = var.toBool();
        ui->pushButton_ConnectMysql->setText(mConnectedMysql?QString("%2:%1(%3)").arg(MYSQL_PORT).arg(MYSQL_IP, tr("点击断开")):tr("连接服务器"));
        ui->pushButton_ConnectMysql->setChecked(mConnectedMysql);
        if(mConnectedMysql)
        {   //连接上服务器了
            emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_RECENT, mLimit);
            emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_RECENT, mLimit);
            emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_RECENT, mLimit);
            on_pushButton_FindAnime_clicked();
            on_pushButton_FindMovie_clicked();
            on_pushButton_FindTv_clicked();
        }
        else
        {   //断开服务器
            mAnimeRecents.clear();
            mMovieRecents.clear();
            mTvRecents.clear();
            mAnimeRecentMode.enable = false;
            mMovieRecentMode.enable = false;
            mTvRecentMode.enable = false;
            ui->label_A_LockRecent->setVisible(false);
            ui->label_A_UnlockRecent->setVisible(false);
            ui->label_M_LockRecent->setVisible(false);
            ui->label_M_UnlockRecent->setVisible(false);
            ui->label_T_LockRecent->setVisible(false);
            ui->label_T_UnlockRecent->setVisible(false);
            setAnimeRecentLabel();
            setMovieRecentLabel();
            setTvRecentLabel();
            showBarAnimeId(-1);
            showBarMovieId(-1);
            showBarTvId(-1);
            ui->listWidget_AP->clear();
            ui->listWidget_AS->clear();
            ui->listWidget_AE->clear();
            ui->listWidget_MP->clear();
            ui->listWidget_MS->clear();
            ui->listWidget_TP->clear();
            ui->listWidget_TS->clear();
            ui->listWidget_TE->clear();
        }
        break;
    case SOT_SELECT_ANIME_IP:
    {
        gIPD.anime_ip.ips = var.value<QList<AnimeIpData> >();
        gIPD.anime_ip.items.clear();
        gIPD.anime_ip.widgets.clear();
        ui->listWidget_AP->clear();
        ui->listWidget_AS->clear();
        ui->listWidget_AE->clear();
        if(gIPD.anime_ip.ips.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime_ip.ips.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeIpWidgetItem *widget = new AnimeIpWidgetItem(gIPD.anime_ip.ips.at(i), ui->listWidget_AP);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_AP->insertItem(i, item);
            ui->listWidget_AP->setItemWidget(item, widget);
            gIPD.anime_ip.items.append(item);
            gIPD.anime_ip.widgets.append(widget);
        }
        if(mAnimeRecentMode.enable)
        {   //最近模式
            on_listWidget_AP_itemClicked(gIPD.anime_ip.items.at(0));
        }
        else if(gIPD.index_anime.p_click)
        {
            gIPD.index_anime.p_click = false;
            //查看是否还有显示
            bool has {false};
            if(gIPD.index_anime.p_row < gIPD.anime_ip.ips.size())
            {
                if(gIPD.anime_ip.ips.at(gIPD.index_anime.p_row).pid == gIPD.index_anime.pid)
                {
                    has = true;
                }
            }
            if(has)
            {
                ui->listWidget_AP->verticalScrollBar()->setValue(gIPD.index_anime.p_pos);
                ui->listWidget_AP->setCurrentRow(gIPD.index_anime.p_row);
                on_listWidget_AP_itemClicked(gIPD.anime_ip.items.at(gIPD.index_anime.p_row));
            }
            else
            {
                gIPD.index_anime.s_click = false;
                gIPD.index_anime.e_click = false;
            }
        }
    }
        break;
    case SOT_SELECT_ANIME_RECENT:
    {
        mAnimeRecents = var.value<QList<AnimeRecentData> >();
        setAnimeRecentLabel();
    }
        break;
    case SOT_INFO_ANIME_IP_PAGE:
    {
        QStringList strs = var.toStringList();
        mAPPageTotal = strs[1].toInt();
        ui->lineEdit_AP_Page->setText(strs[0]);
        ui->label_AP_PageTotal->setText("/"+strs[1]);
        ui->label_AP_Total->setText(QString(tr("共%1个系列")).arg(strs[2]));
    }
        break;
    case SOT_SELECT_ANIME_SEASON:
        gIPD.anime_season.seasons = var.value<QList<AnimeSeasonData> >();
        gIPD.anime_season.items.clear();
        gIPD.anime_season.widgets.clear();
        ui->listWidget_AS->clear();
        ui->listWidget_AE->clear();
        if(gIPD.anime_season.seasons.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime_season.seasons.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeSeasonWidgetItem *widget = new AnimeSeasonWidgetItem(gIPD.anime_season.seasons.at(i), ui->listWidget_AS);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_AS->insertItem(i, item);
            ui->listWidget_AS->setItemWidget(item, widget);
            gIPD.anime_season.items.append(item);
            gIPD.anime_season.widgets.append(widget);
        }
        if(mAnimeRecentMode.enable)
        {   //最近模式
            on_listWidget_AS_itemClicked(gIPD.anime_season.items.at(0));
        }
        else if(gIPD.index_anime.s_click)
        {
            gIPD.index_anime.s_click = false;
            //查看是否还有显示
            bool has {false};
            if(gIPD.index_anime.s_row < gIPD.anime_season.seasons.size())
            {
                if(gIPD.anime_season.seasons.at(gIPD.index_anime.s_row).sid == gIPD.index_anime.sid)
                {
                    has = true;
                }
            }
            if(has)
            {
                ui->listWidget_AS->verticalScrollBar()->setValue(gIPD.index_anime.s_pos);
                ui->listWidget_AS->setCurrentRow(gIPD.index_anime.s_row);
                on_listWidget_AS_itemClicked(gIPD.anime_season.items.at(gIPD.index_anime.s_row));
            }
            else
            {
                gIPD.index_anime.e_click = false;
            }
        }
        break;
    case SOT_INFO_ANIME_SEASON_PAGE:
    {
        QStringList strs = var.toStringList();
        mASPageTotal = strs[1].toInt();
        ui->lineEdit_AS_Page->setText(strs[0]);
        gIPD.index_anime.s_page = ui->lineEdit_AS_Page->text().toInt();
        ui->label_AS_PageTotal->setText("/"+strs[1]);
        ui->label_AS_Total->setText(QString(tr("共%1季")).arg(strs[2]));
    }
        break;
    case SOT_SELECT_ANIME_EPISODE:
        gIPD.anime_ep.eps = var.value<QList<AnimeEpisodeData> >();
        gIPD.anime_ep.items.clear();
        gIPD.anime_ep.widgets.clear();
        ui->listWidget_AE->clear();
        if(gIPD.anime_ep.eps.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime_ep.eps.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeEpisodeWidgetItem *widget = new AnimeEpisodeWidgetItem(gIPD.anime_ep.eps.at(i), i, ui->listWidget_AE);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_AE->insertItem(i, item);
            ui->listWidget_AE->setItemWidget(item, widget);
            gIPD.anime_ep.items.append(item);
            gIPD.anime_ep.widgets.append(widget);
        }
        if(gIPD.index_anime.e_click)
        {
            gIPD.index_anime.e_click = false;
            ui->listWidget_AE->verticalScrollBar()->setValue(gIPD.index_anime.e_pos);
            ui->listWidget_AE->setCurrentRow(gIPD.index_anime.e_row);
            on_listWidget_AE_itemClicked(gIPD.anime_ep.items.at(gIPD.index_anime.e_row));
        }
        break;
    case SOT_INFO_ANIME_EPISODE_PAGE:
    {
        QStringList strs = var.toStringList();
        mAEPageTotal = strs[1].toInt();
        ui->lineEdit_AE_Page->setText(strs[0]);
        gIPD.index_anime.e_page = ui->lineEdit_AE_Page->text().toInt();
        ui->label_AE_PageTotal->setText("/"+strs[1]);
        ui->label_AE_Total->setText(QString(tr("共%1话")).arg(strs[2]));
    }
        break;
    case SOT_TELL_ANIME_RESHOW:
        getAnimeIp(ui->lineEdit_AP_Page->text().toInt());
        break;
    case SOT_UPDATE_ANIME_EPISODE_SEE:
    case SOT_DELETE_ANIME_IP:
    case SOT_DELETE_ANIME_SEASON:
        emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_RECENT, mLimit);
        getAnimeIp(ui->lineEdit_AP_Page->text().toInt());
        break;
    case SOT_DELETE_ANIME_RECENT:
        emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_RECENT, mLimit);
        break;
    case SOT_SELECT_MOVIE_IP:
    {
        gIPD.movie_ip.ips = var.value<QList<MovieIpData> >();
        gIPD.movie_ip.items.clear();
        gIPD.movie_ip.widgets.clear();
        ui->listWidget_MP->clear();
        ui->listWidget_MS->clear();
        if(gIPD.movie_ip.ips.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.movie_ip.ips.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            MovieIpWidgetItem *widget = new MovieIpWidgetItem(gIPD.movie_ip.ips.at(i), ui->listWidget_MP);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_MP->insertItem(i, item);
            ui->listWidget_MP->setItemWidget(item, widget);
            gIPD.movie_ip.items.append(item);
            gIPD.movie_ip.widgets.append(widget);
        }
        if(mMovieRecentMode.enable)
        {   //最近模式
            on_listWidget_MP_itemClicked(gIPD.movie_ip.items.at(0));
        }
        else if(gIPD.index_movie.p_click)
        {
            gIPD.index_movie.p_click = false;
            //查看是否还有显示
            bool has {false};
            if(gIPD.index_movie.p_row < gIPD.movie_ip.ips.size())
            {
                if(gIPD.movie_ip.ips.at(gIPD.index_movie.p_row).pid == gIPD.index_movie.pid)
                {
                    has = true;
                }
            }
            if(has)
            {
                ui->listWidget_MP->verticalScrollBar()->setValue(gIPD.index_movie.p_pos);
                ui->listWidget_MP->setCurrentRow(gIPD.index_movie.p_row);
                on_listWidget_MP_itemClicked(gIPD.movie_ip.items.at(gIPD.index_movie.p_row));
            }
            else
            {
                gIPD.index_movie.s_click = false;
            }
        }
    }
        break;
    case SOT_SELECT_MOVIE_RECENT:
    {
        mMovieRecents = var.value<QList<MovieRecentData> >();
        setMovieRecentLabel();
    }
        break;
    case SOT_INFO_MOVIE_IP_PAGE:
    {
        QStringList strs = var.toStringList();
        mMPPageTotal = strs[1].toInt();
        ui->lineEdit_MP_Page->setText(strs[0]);
        ui->label_MP_PageTotal->setText("/"+strs[1]);
        ui->label_MP_Total->setText(QString(tr("共%1个系列")).arg(strs[2]));
    }
        break;
    case SOT_SELECT_MOVIE_SEASON:
        gIPD.movie_season.seasons = var.value<QList<MovieSeasonData> >();
        gIPD.movie_season.items.clear();
        gIPD.movie_season.widgets.clear();
        ui->listWidget_MS->clear();
        if(gIPD.movie_season.seasons.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.movie_season.seasons.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            MovieSeasonWidgetItem *widget = new MovieSeasonWidgetItem(gIPD.movie_season.seasons.at(i), i, ui->listWidget_MS);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_MS->insertItem(i, item);
            ui->listWidget_MS->setItemWidget(item, widget);
            gIPD.movie_season.items.append(item);
            gIPD.movie_season.widgets.append(widget);
        }
        if(mMovieRecentMode.enable)
        {   //最近模式
            on_listWidget_MS_itemClicked(gIPD.movie_season.items.at(0));
        }
        else if(gIPD.index_movie.s_click)
        {
            gIPD.index_movie.s_click = false;
            ui->listWidget_MS->verticalScrollBar()->setValue(gIPD.index_movie.s_pos);
            ui->listWidget_MS->setCurrentRow(gIPD.index_movie.s_row);
            on_listWidget_MS_itemClicked(gIPD.movie_season.items.at(gIPD.index_movie.s_row));
        }
        break;
    case SOT_INFO_MOVIE_SEASON_PAGE:
    {
        QStringList strs = var.toStringList();
        mMSPageTotal = strs[1].toInt();
        ui->lineEdit_MS_Page->setText(strs[0]);
        gIPD.index_movie.s_page = ui->lineEdit_MS_Page->text().toInt();
        ui->label_MS_PageTotal->setText("/"+strs[1]);
        ui->label_MS_Total->setText(QString(tr("共%1部")).arg(strs[2]));
    }
        break;
    case SOT_TELL_MOVIE_RESHOW:
        getMovieIp(ui->lineEdit_MP_Page->text().toInt());
        break;
    case SOT_UPDATE_MOVIE_SEASON_SEE:
        emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_RECENT, mLimit);
        getMovieIp(ui->lineEdit_MP_Page->text().toInt());
        break;
    case SOT_DELETE_MOVIE_RECENT:
        emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_RECENT, mLimit);
        break;
    case SOT_SELECT_TV_IP:
    {
        gIPD.tv_ip.ips = var.value<QList<TvIpData> >();
        gIPD.tv_ip.items.clear();
        gIPD.tv_ip.widgets.clear();
        ui->listWidget_TP->clear();
        ui->listWidget_TS->clear();
        ui->listWidget_TE->clear();
        if(gIPD.tv_ip.ips.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.tv_ip.ips.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            TvIpWidgetItem *widget = new TvIpWidgetItem(gIPD.tv_ip.ips.at(i), ui->listWidget_TP);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_TP->insertItem(i, item);
            ui->listWidget_TP->setItemWidget(item, widget);
            gIPD.tv_ip.items.append(item);
            gIPD.tv_ip.widgets.append(widget);
        }
        if(mTvRecentMode.enable)
        {   //最近模式
            on_listWidget_TP_itemClicked(gIPD.tv_ip.items.at(0));
        }
        else if(gIPD.index_tv.p_click)
        {
            gIPD.index_tv.p_click = false;
            //查看是否还有显示
            bool has {false};
            if(gIPD.index_tv.p_row < gIPD.tv_ip.ips.size())
            {
                if(gIPD.tv_ip.ips.at(gIPD.index_tv.p_row).pid == gIPD.index_tv.pid)
                {
                    has = true;
                }
            }
            if(has)
            {
                ui->listWidget_TP->verticalScrollBar()->setValue(gIPD.index_tv.p_pos);
                ui->listWidget_TP->setCurrentRow(gIPD.index_tv.p_row);
                on_listWidget_TP_itemClicked(gIPD.tv_ip.items.at(gIPD.index_tv.p_row));
            }
            else
            {
                gIPD.index_tv.s_click = false;
                gIPD.index_tv.e_click = false;
            }
        }
    }
        break;
    case SOT_SELECT_TV_SEASON:
        gIPD.tv_season.seasons = var.value<QList<TvSeasonData> >();
        gIPD.tv_season.items.clear();
        gIPD.tv_season.widgets.clear();
        ui->listWidget_TS->clear();
        ui->listWidget_TE->clear();
        if(gIPD.tv_season.seasons.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.tv_season.seasons.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            TvSeasonWidgetItem *widget = new TvSeasonWidgetItem(gIPD.tv_season.seasons.at(i), ui->listWidget_TS);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_TS->insertItem(i, item);
            ui->listWidget_TS->setItemWidget(item, widget);
            gIPD.tv_season.items.append(item);
            gIPD.tv_season.widgets.append(widget);
        }
        if(mTvRecentMode.enable)
        {   //最近模式
            on_listWidget_TS_itemClicked(gIPD.tv_season.items.at(0));
        }
        else if(gIPD.index_tv.s_click)
        {
            gIPD.index_tv.s_click = false;
            //查看是否还有显示
            bool has {false};
            if(gIPD.index_tv.s_row < gIPD.tv_season.seasons.size())
            {
                if(gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).sid == gIPD.index_tv.sid)
                {
                    has = true;
                }
            }
            if(has)
            {
                ui->listWidget_TS->verticalScrollBar()->setValue(gIPD.index_tv.s_pos);
                ui->listWidget_TS->setCurrentRow(gIPD.index_tv.s_row);
                on_listWidget_TS_itemClicked(gIPD.tv_season.items.at(gIPD.index_tv.s_row));
            }
            else
            {
                gIPD.index_tv.e_click = false;
            }
        }
        break;
    case SOT_SELECT_TV_EPISODE:
        gIPD.tv_ep.eps = var.value<QList<TvEpisodeData> >();
        gIPD.tv_ep.items.clear();
        gIPD.tv_ep.widgets.clear();
        ui->listWidget_TE->clear();
        if(gIPD.tv_ep.eps.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.tv_ep.eps.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            TvEpisodeWidgetItem *widget = new TvEpisodeWidgetItem(gIPD.tv_ep.eps.at(i), i, ui->listWidget_TE);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_TE->insertItem(i, item);
            ui->listWidget_TE->setItemWidget(item, widget);
            gIPD.tv_ep.items.append(item);
            gIPD.tv_ep.widgets.append(widget);
        }
        if(gIPD.index_tv.e_click)
        {
            gIPD.index_tv.e_click = false;
            ui->listWidget_TE->verticalScrollBar()->setValue(gIPD.index_tv.e_pos);
            ui->listWidget_TE->setCurrentRow(gIPD.index_tv.e_row);
            on_listWidget_TE_itemClicked(gIPD.tv_ep.items.at(gIPD.index_tv.e_row));
        }
        break;
    case SOT_SELECT_TV_RECENT:
    {
        mTvRecents = var.value<QList<TvRecentData> >();
        setTvRecentLabel();
    }
        break;
    case SOT_INFO_TV_IP_PAGE:
    {
        QStringList strs = var.toStringList();
        mTPPageTotal = strs[1].toInt();
        ui->lineEdit_TP_Page->setText(strs[0]);
        ui->label_TP_PageTotal->setText("/"+strs[1]);
        ui->label_TP_Total->setText(QString(tr("共%1个系列")).arg(strs[2]));
    }
        break;
    case SOT_INFO_TV_SEASON_PAGE:
    {
        QStringList strs = var.toStringList();
        mTSPageTotal = strs[1].toInt();
        ui->lineEdit_TS_Page->setText(strs[0]);
        gIPD.index_tv.s_page = ui->lineEdit_TS_Page->text().toInt();
        ui->label_TS_PageTotal->setText("/"+strs[1]);
        ui->label_TS_Total->setText(QString(tr("共%1部")).arg(strs[2]));
    }
        break;
    case SOT_INFO_TV_EPISODE_PAGE:
    {
        QStringList strs = var.toStringList();
        mTEPageTotal = strs[1].toInt();
        ui->lineEdit_TE_Page->setText(strs[0]);
        gIPD.index_tv.e_page = ui->lineEdit_TE_Page->text().toInt();
        ui->label_TE_PageTotal->setText("/"+strs[1]);
        ui->label_TE_Total->setText(QString(tr("共%1集")).arg(strs[2]));
    }
        break;
    case SOT_TELL_TV_RESHOW:
        getTvIp(ui->lineEdit_TP_Page->text().toInt());
        break;
    case SOT_UPDATE_TV_EPISODE_SEE:
    case SOT_DELETE_TV_IP:
    case SOT_DELETE_TV_SEASON:
        emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_RECENT, mLimit);
        getTvIp(ui->lineEdit_TP_Page->text().toInt());
        break;
    case SOT_DELETE_TV_RECENT:
        emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_RECENT, mLimit);
        break;
    default:
        break;
    }
}

///动漫话看完
void Widget::slotAnimeEpisodeSee(AnimeEpisodeData episode, int row)
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    gIPD.index_anime.e_row = row;
    gIPD.index_anime.e_pos = ui->listWidget_AE->verticalScrollBar()->value();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_SEE, var_send);
}

///电影部看完
void Widget::slotMovieSeasonSee(MovieSeasonData season, int row)
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    gIPD.index_movie.s_row = row;
    gIPD.index_movie.s_pos = ui->listWidget_MS->verticalScrollBar()->value();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_SEE, var_send);
}

///电视剧集看完
void Widget::slotTvEpisodeSee(TvEpisodeData episode, int row)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = true;
    gIPD.index_tv.e_row = row;
    gIPD.index_tv.e_pos = ui->listWidget_TE->verticalScrollBar()->value();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_EPISODE_SEE, var_send);
}

/////连接|断开服务器
void Widget::on_pushButton_ConnectMysql_clicked(bool checked)
{
    emit gIPD.SIGNALSendQuery(SOT_CONNECT_MYSQL, checked);
    if(checked)
    {
        ui->pushButton_ConnectMysql->setText(tr("连接中.."));
    }
}

///init
void Widget::qian()
{
    mAnimeIpNewDialog = new AnimeIpNewDialog(this);
    mAnimeSeasonNewDialog = new AnimeSeasonNewDialog(this);
    mAnimeEpisodeNewDialog = new AnimeEpisodeNewDialog(this);
    mMovieIpNewDialog = new MovieIpNewDialog(this);
    mMovieSeasonNewDialog = new MovieSeasonNewDialog(this);
    mTvIpNewDialog = new TvIpNewDialog(this);
    mTvSeasonNewDialog = new TvSeasonNewDialog(this);
    mTvEpisodeNewDialog = new TvEpisodeNewDialog(this);

    ui->label_BarA_PidText->setVisible(false);
    ui->label_BarA_SidText->setVisible(false);
    ui->label_BarA_EidText->setVisible(false);
    ui->label_BarA_Pid->setVisible(false);
    ui->label_BarA_Sid->setVisible(false);
    ui->label_BarA_Eid->setVisible(false);
    ui->label_BarM_PidText->setVisible(false);
    ui->label_BarM_SidText->setVisible(false);
    ui->label_BarM_Pid->setVisible(false);
    ui->label_BarM_Sid->setVisible(false);
    ui->label_BarT_PidText->setVisible(false);
    ui->label_BarT_SidText->setVisible(false);
    ui->label_BarT_EidText->setVisible(false);
    ui->label_BarT_Pid->setVisible(false);
    ui->label_BarT_Sid->setVisible(false);
    ui->label_BarT_Eid->setVisible(false);

    ui->dateEdit_AS_ReleaseDate->setVisible(false);
    ui->checkBox_AS_CollectOk->setEnabled(false);
    ui->dateEdit_MS_ReleaseDate->setVisible(false);
    ui->checkBox_MS_CollectOk->setEnabled(false);
    ui->dateEdit_TS_ReleaseDate->setVisible(false);
    ui->checkBox_TS_CollectOk->setEnabled(false);

    ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);
    ui->stackedWidget_Movie->setCurrentWidget(ui->page_MDefault);
    ui->stackedWidget_Tv->setCurrentWidget(ui->page_TDefault);

    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent1_Name, ui->label_A_Recent1_Close));
    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent2_Name, ui->label_A_Recent2_Close));
    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent3_Name, ui->label_A_Recent3_Close));
    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent4_Name, ui->label_A_Recent4_Close));
    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent5_Name, ui->label_A_Recent5_Close));
    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent6_Name, ui->label_A_Recent6_Close));
    mAnimeRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_A_Recent7_Name, ui->label_A_Recent7_Close));
    setAnimeRecentLabel();
    ui->label_A_LockRecent->setVisible(false);
    ui->label_A_UnlockRecent->setVisible(false);
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent1_Name, ui->label_M_Recent1_Close));
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent2_Name, ui->label_M_Recent2_Close));
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent3_Name, ui->label_M_Recent3_Close));
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent4_Name, ui->label_M_Recent4_Close));
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent5_Name, ui->label_M_Recent5_Close));
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent6_Name, ui->label_M_Recent6_Close));
    mMovieRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_M_Recent7_Name, ui->label_M_Recent7_Close));
    setMovieRecentLabel();
    ui->label_M_LockRecent->setVisible(false);
    ui->label_M_UnlockRecent->setVisible(false);
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent1_Name, ui->label_T_Recent1_Close));
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent2_Name, ui->label_T_Recent2_Close));
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent3_Name, ui->label_T_Recent3_Close));
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent4_Name, ui->label_T_Recent4_Close));
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent5_Name, ui->label_T_Recent5_Close));
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent6_Name, ui->label_T_Recent6_Close));
    mTvRecentLabels.append(QPair<QLabel *, QLabel *>(ui->label_T_Recent7_Name, ui->label_T_Recent7_Close));
    setTvRecentLabel();
    ui->label_T_LockRecent->setVisible(false);
    ui->label_T_UnlockRecent->setVisible(false);

    ui->label_A_UnlockRecent->installEventFilter(this);
    ui->label_A_Recent1_Name->installEventFilter(this);
    ui->label_A_Recent2_Name->installEventFilter(this);
    ui->label_A_Recent3_Name->installEventFilter(this);
    ui->label_A_Recent4_Name->installEventFilter(this);
    ui->label_A_Recent5_Name->installEventFilter(this);
    ui->label_A_Recent6_Name->installEventFilter(this);
    ui->label_A_Recent7_Name->installEventFilter(this);
    ui->label_A_Recent1_Close->installEventFilter(this);
    ui->label_A_Recent2_Close->installEventFilter(this);
    ui->label_A_Recent3_Close->installEventFilter(this);
    ui->label_A_Recent4_Close->installEventFilter(this);
    ui->label_A_Recent5_Close->installEventFilter(this);
    ui->label_A_Recent6_Close->installEventFilter(this);
    ui->label_A_Recent7_Close->installEventFilter(this);
    ui->label_M_UnlockRecent->installEventFilter(this);
    ui->label_M_Recent1_Name->installEventFilter(this);
    ui->label_M_Recent2_Name->installEventFilter(this);
    ui->label_M_Recent3_Name->installEventFilter(this);
    ui->label_M_Recent4_Name->installEventFilter(this);
    ui->label_M_Recent5_Name->installEventFilter(this);
    ui->label_M_Recent6_Name->installEventFilter(this);
    ui->label_M_Recent7_Name->installEventFilter(this);
    ui->label_M_Recent1_Close->installEventFilter(this);
    ui->label_M_Recent2_Close->installEventFilter(this);
    ui->label_M_Recent3_Close->installEventFilter(this);
    ui->label_M_Recent4_Close->installEventFilter(this);
    ui->label_M_Recent5_Close->installEventFilter(this);
    ui->label_M_Recent6_Close->installEventFilter(this);
    ui->label_M_Recent7_Close->installEventFilter(this);
    ui->label_T_UnlockRecent->installEventFilter(this);
    ui->label_T_Recent1_Name->installEventFilter(this);
    ui->label_T_Recent2_Name->installEventFilter(this);
    ui->label_T_Recent3_Name->installEventFilter(this);
    ui->label_T_Recent4_Name->installEventFilter(this);
    ui->label_T_Recent5_Name->installEventFilter(this);
    ui->label_T_Recent6_Name->installEventFilter(this);
    ui->label_T_Recent7_Name->installEventFilter(this);
    ui->label_T_Recent1_Close->installEventFilter(this);
    ui->label_T_Recent2_Close->installEventFilter(this);
    ui->label_T_Recent3_Close->installEventFilter(this);
    ui->label_T_Recent4_Close->installEventFilter(this);
    ui->label_T_Recent5_Close->installEventFilter(this);
    ui->label_T_Recent6_Close->installEventFilter(this);
    ui->label_T_Recent7_Close->installEventFilter(this);

    connect(&gIPD, &InterfacePublicData::SIGNALSendQuery, this, &Widget::slotSendQuery, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALReceiveQueryData, this, &Widget::slotReceiveQueryData, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALAnimeEpisodeSee, this, &Widget::slotAnimeEpisodeSee, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALMovieSeasonSee, this, &Widget::slotMovieSeasonSee, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALTvEpisodeSee, this, &Widget::slotTvEpisodeSee, Qt::UniqueConnection);

    //初始化后操作
    QTimer::singleShot(0, this, [this]{
        showBarAnimeId(-1);
        showBarMovieId(-1);
        showBarTvId(-1);
    });
}

///获取动漫ip
void Widget::getAnimeIp(int page)
{
    showBarAnimeId(0);
    QStringList strs;
    strs << QString::number(page)
         << mFindAnimeSql;
    emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_IP, strs);   //获取动漫
}

///获取动漫季
void Widget::getAnimeSeason(int page)
{
    showBarAnimeId(1);
    QStringList strs;
    strs << QString::number(gIPD.index_anime.pid)
         << QString::number(page)
         << QString::number(mLimit?1:0)
         << QString::number(mAnimeRecentMode.enable?mAnimeRecentMode.sid:-1);
    emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_SEASON, strs);   //获取动漫季
}

///获取动漫话
void Widget::getAnimeEpisode(int page)
{
    showBarAnimeId(2);
    QStringList strs;
    strs << QString::number(gIPD.index_anime.pid)
         << QString::number(gIPD.index_anime.sid)
         << QString::number(page);
    emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_EPISODE, strs);   //获取动漫话
}

///显示动漫id条
void Widget::showBarAnimeId(int what)
{
    bool show_ap {false};
    bool show_as {false};
    bool show_ae {false};
    switch(what)
    {
    case -1:    //断开服务器
        ui->widget_AP_Op->setEnabled(false);
        ui->lineEdit_AP_Page->setText("0");
        ui->label_AP_PageTotal->setText("/0");
        ui->label_AP_Total->setText("少女祈祷中..");
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);
        break;
    case 0:     //连上了服务器
        ui->widget_AP_Op->setEnabled(true);
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);
        break;
    case 1:     //点击了ap
        show_ap = true;
        ui->label_BarA_Pid->setText(QString::number(gIPD.index_anime.pid));
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_AP);
        break;
    case 2:     //点击了as
        show_ap = true;
        show_as = true;
        ui->label_BarA_Sid->setText(QString::number(gIPD.index_anime.sid));
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_AS);
        break;
    case 3:     //点击了ae
        show_ap = true;
        show_as = true;
        show_ae = true;
        ui->label_BarA_Eid->setText(QString::number(gIPD.index_anime.eid));
        ui->stackedWidget_Anime->setCurrentWidget(ui->page_AE);
        break;
    default:
        break;
    }
    ui->label_BarA_PidText->setVisible(show_ap);
    ui->label_BarA_SidText->setVisible(show_as);
    ui->label_BarA_EidText->setVisible(show_ae);
    ui->label_BarA_Pid->setVisible(show_ap);
    ui->label_BarA_Sid->setVisible(show_as);
    ui->label_BarA_Eid->setVisible(show_ae);
    ui->widget_AS_Op->setEnabled(show_ap);
    if(!show_ap)
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
    if(mAnimeRecentMode.enable)
    {   //锁定最近模式
        mFindAnimeSql += QString(" AND pid=%1").arg(mAnimeRecentMode.pid);
    }
    else
    {   //正常检索模式
        if(!ui->checkBox_Limit->isChecked())
        {
            mFindAnimeSql += " AND display=1";
        }
        if(ui->checkBox_FindAnimeZhuifan->isChecked())
        {
            mFindAnimeSql += " AND zhuifan=1";
        }
        if(ui->checkBox_FindAnimeNotsee->isChecked())
        {
            mFindAnimeSql += " AND see=0";
        }
        int point_a = ui->comboBox_FindAnimePointA->currentIndex();
        int point_b = ui->comboBox_FindAnimePointB->currentIndex();
        int point_min = qMin(point_a, point_b);
        int point_max = qMax(point_a, point_b);
        if(point_min != 0 || point_max != 12)
        {
            if(point_min == point_max)
            {
                mFindAnimeSql += QString(" AND point=%1").arg(point_max);
            }
            else
            {
                mFindAnimeSql += QString(" AND (point BETWEEN %1 AND %2)").arg(point_min).arg(point_max);
            }
        }
        QString anime_name = ui->lineEdit_FindAnimeName->text().trimmed().replace("'", "''");
        if(!anime_name.isEmpty())
        {
            mFindAnimeSql += QString(" AND (name LIKE '%%1%' OR keywords LIKE '%%1%')").arg(anime_name);
        }
    }
    if(!mFindAnimeSql.isEmpty())
    {
        mFindAnimeSql.remove(0, 5);
        mFindAnimeSql = " WHERE " + mFindAnimeSql;
    }
}

///设置最近观看
void Widget::setAnimeRecentLabel()
{
    for(int i = 0; i < mAnimeRecentLabels.size(); i++)
    {
        mAnimeRecentLabels.at(i).first->setVisible(false);
        mAnimeRecentLabels.at(i).second->setVisible(false);
    }
    QString qss_display = QString("QLabel{padding-left:2px;padding-right:2px;border:1px solid grey;border-right:none;background-color:#99cc99;color:white;}"
                          "QLabel:hover{background-color:white;color:#99cc99;}");
    QString qss_hide = QString("QLabel{padding-left:2px;padding-right:2px;border:1px solid grey;border-right:none;background-color:red;color:white;}"
                          "QLabel:hover{background-color:white;color:red;}");
    for(int i = 0; i < mAnimeRecents.size(); i++)
    {
        mAnimeRecentLabels.at(i).first->setText(mAnimeRecents.at(i).name);
        mAnimeRecentLabels.at(i).first->setToolTip(mAnimeRecents.at(i).name);
        mAnimeRecentLabels.at(i).first->setStyleSheet(mAnimeRecents.at(i).display?qss_display:qss_hide);
        mAnimeRecentLabels.at(i).first->setVisible(true);
        mAnimeRecentLabels.at(i).second->setVisible(true);
    }
}

///显示最近观看
void Widget::showAnimeRecent(int index)
{
    mAnimeRecentMode.enable = true;
    mAnimeRecentMode.pid = mAnimeRecents.at(index).pid;
    mAnimeRecentMode.sid = mAnimeRecents.at(index).sid;
    mAnimeRecentMode.name = mAnimeRecents.at(index).name;
    ui->label_A_LockRecent->setText(QString("已锁定显示 %3 PID/SID [%1/%2]").arg(mAnimeRecentMode.pid).arg(mAnimeRecentMode.sid).arg(mAnimeRecentMode.name));
    ui->label_A_LockRecent->setVisible(true);
    ui->label_A_UnlockRecent->setVisible(true);
    on_pushButton_FindAnime_clicked();
}

///关闭最近观看
void Widget::closeAnimeRecent(int index)
{
    int ret = QMessageBox::information(this, tr("警告"), tr("确认删除这条记录?"), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret == QMessageBox::Ok)
    {
        if(mAnimeRecentMode.enable)
        {
            if(mAnimeRecentMode.sid == mAnimeRecents.at(index).sid)
            {
                mAnimeRecentMode.enable = false;
                ui->label_A_LockRecent->setVisible(false);
                ui->label_A_UnlockRecent->setVisible(false);
                on_pushButton_FindAnime_clicked();
            }
        }
        emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_RECENT, mAnimeRecents.at(index).id);
    }
}

///获取电影ip
void Widget::getMovieIp(int page)
{
    showBarMovieId(0);
    QStringList strs;
    strs << QString::number(page)
         << mFindMovieSql;
    emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_IP, strs);   //获取电影ip
}

///获取电影部
void Widget::getMovieSeason(int page)
{
    showBarMovieId(1);
    QStringList strs;
    strs << QString::number(gIPD.index_movie.pid)
         << QString::number(page);
    emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_SEASON, strs);   //获取电影部
}

///显示电影id条
void Widget::showBarMovieId(int what)
{
    bool show_mp {false};
    bool show_ms {false};
    switch(what)
    {
    case -1:    //断开服务器
        ui->widget_MP_Op->setEnabled(false);
        ui->lineEdit_MP_Page->setText("0");
        ui->label_MP_PageTotal->setText("/0");
        ui->label_MP_Total->setText("少女祈祷中..");
        ui->stackedWidget_Movie->setCurrentWidget(ui->page_MDefault);
        break;
    case 0:     //连上了服务器
        ui->widget_MP_Op->setEnabled(true);
        ui->stackedWidget_Movie->setCurrentWidget(ui->page_MDefault);
        break;
    case 1:     //点击了mp
        show_mp = true;
        ui->label_BarM_Pid->setText(QString::number(gIPD.index_movie.pid));
        ui->stackedWidget_Movie->setCurrentWidget(ui->page_MP);
        break;
    case 2:     //点击了ms
        show_mp = true;
        show_ms = true;
        ui->label_BarM_Sid->setText(QString::number(gIPD.index_movie.sid));
        ui->stackedWidget_Movie->setCurrentWidget(ui->page_MS);
        break;
    default:
        break;
    }
    ui->label_BarM_PidText->setVisible(show_mp);
    ui->label_BarM_SidText->setVisible(show_ms);
    ui->label_BarM_Pid->setVisible(show_mp);
    ui->label_BarM_Sid->setVisible(show_ms);
    ui->widget_MS_Op->setEnabled(show_mp);
    if(!show_mp)
    {
        ui->lineEdit_MS_Page->setText("0");
        ui->label_MS_PageTotal->setText("/0");
        ui->label_MS_Total->setText("少女祈祷中..");
    }
}

///更新查找电影条件
void Widget::genFindMovieSql()
{
    mFindMovieSql.clear();
    if(mMovieRecentMode.enable)
    {   //锁定最近模式
        mFindMovieSql += QString(" AND pid=%1").arg(mMovieRecentMode.pid);
    }
    else
    {   //正常检索模式
        if(!ui->checkBox_Limit->isChecked())
        {
            mFindMovieSql += " AND display=1";
        }
        if(ui->checkBox_FindMovieNotsee->isChecked())
        {
            mFindMovieSql += " AND see=0";
        }
        int point_a = ui->comboBox_FindMoviePointA->currentIndex();
        int point_b = ui->comboBox_FindMoviePointB->currentIndex();
        int point_min = qMin(point_a, point_b);
        int point_max = qMax(point_a, point_b);
        if(point_min != 0 || point_max != 12)
        {
            if(point_min == point_max)
            {
                mFindMovieSql += QString(" AND point=%1").arg(point_max);
            }
            else
            {
                mFindMovieSql += QString(" AND (point BETWEEN %1 AND %2)").arg(point_min).arg(point_max);
            }
        }
        QString movie_name = ui->lineEdit_FindMovieName->text().trimmed().replace("'", "''");
        if(!movie_name.isEmpty())
        {
            mFindMovieSql += QString(" AND (name LIKE '%%1%' OR keywords LIKE '%%1%')").arg(movie_name);
        }
    }
    if(!mFindMovieSql.isEmpty())
    {
        mFindMovieSql.remove(0, 5);
        mFindMovieSql = " WHERE " + mFindMovieSql;
    }
}

///设置最近观看
void Widget::setMovieRecentLabel()
{
    for(int i = 0; i < mMovieRecentLabels.size(); i++)
    {
        mMovieRecentLabels.at(i).first->setVisible(false);
        mMovieRecentLabels.at(i).second->setVisible(false);
    }
    QString qss_display = QString("QLabel{padding-left:2px;padding-right:2px;border:1px solid grey;border-right:none;background-color:#99cc99;color:white;}"
                          "QLabel:hover{background-color:white;color:#99cc99;}");
    QString qss_hide = QString("QLabel{padding-left:2px;padding-right:2px;border:1px solid grey;border-right:none;background-color:red;color:white;}"
                          "QLabel:hover{background-color:white;color:red;}");
    for(int i = 0; i < mMovieRecents.size(); i++)
    {
        mMovieRecentLabels.at(i).first->setText(mMovieRecents.at(i).name);
        mMovieRecentLabels.at(i).first->setToolTip(mMovieRecents.at(i).name);
        mMovieRecentLabels.at(i).first->setStyleSheet(mMovieRecents.at(i).display?qss_display:qss_hide);
        mMovieRecentLabels.at(i).first->setVisible(true);
        mMovieRecentLabels.at(i).second->setVisible(true);
    }
}

///显示最近观看
void Widget::showMovieRecent(int index)
{
    mMovieRecentMode.enable = true;
    mMovieRecentMode.pid = mMovieRecents.at(index).pid;
    mMovieRecentMode.sid = mMovieRecents.at(index).sid;
    mMovieRecentMode.name = mMovieRecents.at(index).name;
    ui->label_M_LockRecent->setText(QString("已锁定显示 %3 PID [%1]").arg(mMovieRecentMode.pid).arg(mMovieRecentMode.name));
    ui->label_M_LockRecent->setVisible(true);
    ui->label_M_UnlockRecent->setVisible(true);
    on_pushButton_FindMovie_clicked();
}

///关闭最近观看
void Widget::closeMovieRecent(int index)
{
    int ret = QMessageBox::information(this, tr("警告"), tr("确认删除这条记录?"), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret == QMessageBox::Ok)
    {
        if(mMovieRecentMode.enable)
        {
            if(mMovieRecentMode.sid == mMovieRecents.at(index).sid)
            {
                mMovieRecentMode.enable = false;
                ui->label_M_LockRecent->setVisible(false);
                ui->label_M_UnlockRecent->setVisible(false);
                on_pushButton_FindMovie_clicked();
            }
        }
        emit gIPD.SIGNALSendQuery(SOT_DELETE_MOVIE_RECENT, mMovieRecents.at(index).id);
    }
}

///获取电视剧ip
void Widget::getTvIp(int page)
{
    showBarTvId(0);
    QStringList strs;
    strs << QString::number(page)
         << mFindTvSql;
    emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_IP, strs);   //获取电视剧
}

///获取电视剧部
void Widget::getTvSeason(int page)
{
    showBarTvId(1);
    QStringList strs;
    strs << QString::number(gIPD.index_tv.pid)
         << QString::number(page)
         << QString::number(mLimit?1:0)
         << QString::number(mTvRecentMode.enable?mTvRecentMode.sid:-1);
    emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_SEASON, strs);   //获取电视剧部
}

///获取电视剧集
void Widget::getTvEpisode(int page)
{
    showBarTvId(2);
    QStringList strs;
    strs << QString::number(gIPD.index_tv.pid)
         << QString::number(gIPD.index_tv.sid)
         << QString::number(page);
    emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_EPISODE, strs);   //获取电视剧集
}

///显示电视剧id条
void Widget::showBarTvId(int what)
{
    bool show_tp {false};
    bool show_ts {false};
    bool show_te {false};
    switch(what)
    {
    case -1:    //断开服务器
        ui->widget_TP_Op->setEnabled(false);
        ui->lineEdit_TP_Page->setText("0");
        ui->label_TP_PageTotal->setText("/0");
        ui->label_TP_Total->setText("少女祈祷中..");
        ui->stackedWidget_Tv->setCurrentWidget(ui->page_TDefault);
        break;
    case 0:     //连上了服务器
        ui->widget_TP_Op->setEnabled(true);
        ui->stackedWidget_Tv->setCurrentWidget(ui->page_TDefault);
        break;
    case 1:     //点击了tp
        show_tp = true;
        ui->label_BarT_Pid->setText(QString::number(gIPD.index_tv.pid));
        ui->stackedWidget_Tv->setCurrentWidget(ui->page_TP);
        break;
    case 2:     //点击了ts
        show_tp = true;
        show_ts = true;
        ui->label_BarT_Sid->setText(QString::number(gIPD.index_tv.sid));
        ui->stackedWidget_Tv->setCurrentWidget(ui->page_TS);
        break;
    case 3:     //点击了te
        show_tp = true;
        show_ts = true;
        show_te = true;
        ui->label_BarT_Eid->setText(QString::number(gIPD.index_tv.eid));
        ui->stackedWidget_Tv->setCurrentWidget(ui->page_TE);
        break;
    default:
        break;
    }
    ui->label_BarT_PidText->setVisible(show_tp);
    ui->label_BarT_SidText->setVisible(show_ts);
    ui->label_BarT_EidText->setVisible(show_te);
    ui->label_BarT_Pid->setVisible(show_tp);
    ui->label_BarT_Sid->setVisible(show_ts);
    ui->label_BarT_Eid->setVisible(show_te);
    ui->widget_TS_Op->setEnabled(show_tp);
    if(!show_tp)
    {
        ui->lineEdit_TS_Page->setText("0");
        ui->label_TS_PageTotal->setText("/0");
        ui->label_TS_Total->setText("少女祈祷中..");
    }
    ui->widget_TE_Op->setEnabled(show_ts);
    if(!show_ts)
    {
        ui->lineEdit_TE_Page->setText("0");
        ui->label_TE_PageTotal->setText("/0");
        ui->label_TE_Total->setText("少女祈祷中..");
    }
}

///更新查找电视剧条件
void Widget::genFindTvSql()
{
    mFindTvSql.clear();
    if(mTvRecentMode.enable)
    {   //锁定最近模式
        mFindTvSql += QString(" AND pid=%1").arg(mTvRecentMode.pid);
    }
    else
    {   //正常检索模式
        if(!ui->checkBox_Limit->isChecked())
        {
            mFindTvSql += " AND display=1";
        }
        if(ui->checkBox_FindTvZhuiju->isChecked())
        {
            mFindTvSql += " AND zhuiju=1";
        }
        if(ui->checkBox_FindTvNotsee->isChecked())
        {
            mFindTvSql += " AND see=0";
        }
        int point_a = ui->comboBox_FindTvPointA->currentIndex();
        int point_b = ui->comboBox_FindTvPointB->currentIndex();
        int point_min = qMin(point_a, point_b);
        int point_max = qMax(point_a, point_b);
        if(point_min != 0 || point_max != 12)
        {
            if(point_min == point_max)
            {
                mFindTvSql += QString(" AND point=%1").arg(point_max);
            }
            else
            {
                mFindTvSql += QString(" AND (point BETWEEN %1 AND %2)").arg(point_min).arg(point_max);
            }
        }
        QString tv_name = ui->lineEdit_FindTvName->text().trimmed().replace("'", "''");
        if(!tv_name.isEmpty())
        {
            mFindTvSql += QString(" AND (name LIKE '%%1%' OR keywords LIKE '%%1%')").arg(tv_name);
        }
    }
    if(!mFindTvSql.isEmpty())
    {
        mFindTvSql.remove(0, 5);
        mFindTvSql = " WHERE " + mFindTvSql;
    }
}

///设置最近观看
void Widget::setTvRecentLabel()
{
    for(int i = 0; i < mTvRecentLabels.size(); i++)
    {
        mTvRecentLabels.at(i).first->setVisible(false);
        mTvRecentLabels.at(i).second->setVisible(false);
    }
    QString qss_display = QString("QLabel{padding-left:2px;padding-right:2px;border:1px solid grey;border-right:none;background-color:#99cc99;color:white;}"
                                  "QLabel:hover{background-color:white;color:#99cc99;}");
    QString qss_hide = QString("QLabel{padding-left:2px;padding-right:2px;border:1px solid grey;border-right:none;background-color:red;color:white;}"
                               "QLabel:hover{background-color:white;color:red;}");
    for(int i = 0; i < mTvRecents.size(); i++)
    {
        mTvRecentLabels.at(i).first->setText(mTvRecents.at(i).name);
        mTvRecentLabels.at(i).first->setToolTip(mTvRecents.at(i).name);
        mTvRecentLabels.at(i).first->setStyleSheet(mTvRecents.at(i).display?qss_display:qss_hide);
        mTvRecentLabels.at(i).first->setVisible(true);
        mTvRecentLabels.at(i).second->setVisible(true);
    }
}

///显示最近观看
void Widget::showTvRecent(int index)
{
    mTvRecentMode.enable = true;
    mTvRecentMode.pid = mTvRecents.at(index).pid;
    mTvRecentMode.sid = mTvRecents.at(index).sid;
    mTvRecentMode.name = mTvRecents.at(index).name;
    ui->label_T_LockRecent->setText(QString("已锁定显示 %3 PID/SID [%1/%2]").arg(mTvRecentMode.pid).arg(mTvRecentMode.sid).arg(mTvRecentMode.name));
    ui->label_T_LockRecent->setVisible(true);
    ui->label_T_UnlockRecent->setVisible(true);
    on_pushButton_FindTv_clicked();
}

///关闭最近观看
void Widget::closeTvRecent(int index)
{
    int ret = QMessageBox::information(this, tr("警告"), tr("确认删除这条记录?"), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret == QMessageBox::Ok)
    {
        if(mTvRecentMode.enable)
        {
            if(mTvRecentMode.sid == mTvRecents.at(index).sid)
            {
                mTvRecentMode.enable = false;
                ui->label_T_LockRecent->setVisible(false);
                ui->label_T_UnlockRecent->setVisible(false);
                on_pushButton_FindTv_clicked();
            }
        }
        emit gIPD.SIGNALSendQuery(SOT_DELETE_TV_RECENT, mTvRecents.at(index).id);
    }
}

///事件过滤
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_M_UnlockRecent)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            mMovieRecentMode.enable = false;
            ui->label_M_LockRecent->setVisible(false);
            ui->label_M_UnlockRecent->setVisible(false);
            on_pushButton_FindMovie_clicked();
            return true;
        }
    }
    else if(obj == ui->label_M_Recent1_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(0);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent2_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(1);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent3_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(2);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent4_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(3);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent5_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(4);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent6_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(5);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent7_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showMovieRecent(6);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent1_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(0);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent2_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(1);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent3_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(2);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent4_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(3);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent5_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(4);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent6_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(5);
            return true;
        }
    }
    else if(obj == ui->label_M_Recent7_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeMovieRecent(6);
            return true;
        }
    }
    else if(obj == ui->label_T_UnlockRecent)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            mTvRecentMode.enable = false;
            ui->label_T_LockRecent->setVisible(false);
            ui->label_T_UnlockRecent->setVisible(false);
            on_pushButton_FindTv_clicked();
            return true;
        }
    }
    else if(obj == ui->label_T_Recent1_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(0);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent2_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(1);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent3_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(2);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent4_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(3);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent5_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(4);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent6_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(5);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent7_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showTvRecent(6);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent1_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(0);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent2_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(1);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent3_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(2);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent4_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(3);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent5_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(4);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent6_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(5);
            return true;
        }
    }
    else if(obj == ui->label_T_Recent7_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeTvRecent(6);
            return true;
        }
    }
    else if(obj == ui->label_A_UnlockRecent)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            mAnimeRecentMode.enable = false;
            ui->label_A_LockRecent->setVisible(false);
            ui->label_A_UnlockRecent->setVisible(false);
            on_pushButton_FindAnime_clicked();
            return true;
        }
    }
    else if(obj == ui->label_A_Recent1_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(0);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent2_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(1);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent3_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(2);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent4_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(3);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent5_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(4);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent6_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(5);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent7_Name)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            showAnimeRecent(6);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent1_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(0);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent2_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(1);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent3_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(2);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent4_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(3);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent5_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(4);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent6_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(5);
            return true;
        }
    }
    else if(obj == ui->label_A_Recent7_Close)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            closeAnimeRecent(6);
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

///动漫ip点击
void Widget::on_listWidget_AP_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_AP->row(item);
    AnimeIpData ip = gIPD.anime_ip.ips.at(row);
    gIPD.index_anime.p_row = row;
    gIPD.index_anime.pid = gIPD.anime_ip.ips.at(row).pid;
    gIPD.index_anime.p_pos = ui->listWidget_AP->verticalScrollBar()->value();
    showBarAnimeId(1);
    getAnimeSeason(gIPD.index_anime.s_click?gIPD.index_anime.s_page:1);
    ui->checkBox_AP_Display->setChecked(ip.display);
    ui->checkBox_AP_Zhuifan->setChecked(ip.zhuifan);
    ui->lineEdit_AP_Name->setText(ip.name);
    ui->lineEdit_AP_Keyword->setText(ip.keywords);
}

///动漫季点击
void Widget::on_listWidget_AS_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_AS->row(item);
    AnimeSeasonData season = gIPD.anime_season.seasons.at(row);
    gIPD.index_anime.s_row = row;
    gIPD.index_anime.sid = season.sid;
    gIPD.index_anime.s_pos = ui->listWidget_AS->verticalScrollBar()->value();
    showBarAnimeId(2);
    getAnimeEpisode(gIPD.index_anime.e_click?gIPD.index_anime.e_page:1);
    ui->lineEdit_AS_Name->setText(season.name);
    ui->checkBox_AS_Display->setChecked(season.display);
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
void Widget::on_listWidget_AE_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_AE->row(item);
    gIPD.index_anime.e_row = row;
    gIPD.index_anime.eid = gIPD.anime_ep.eps.at(row).eid;
    gIPD.index_anime.e_pos = ui->listWidget_AE->verticalScrollBar()->value();
    showBarAnimeId(3);
    ui->checkBox_AE_Tag1->setChecked(gIPD.anime_ep.eps.at(row).tag1);
    ui->checkBox_AE_Tag2->setChecked(gIPD.anime_ep.eps.at(row).tag2);
    ui->checkBox_AE_Tag3->setChecked(gIPD.anime_ep.eps.at(row).tag3);
    ui->lineEdit_AE_Episode->setText(gIPD.anime_ep.eps.at(row).episode);
    ui->lineEdit_AE_Title->setText(gIPD.anime_ep.eps.at(row).title);
    ui->lineEdit_AE_TitleOrigin->setText(gIPD.anime_ep.eps.at(row).origin);
}

///动漫ip上一页
void Widget::on_pushButton_AP_PrePage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_AP_Page->text().toInt() - 1;
        if(page > 0)
        {
            getAnimeIp(page);
        }
    }
}

///动漫ip下一页
void Widget::on_pushButton_AP_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_AP_Page->text().toInt() + 1;
        if(page <= mAPPageTotal)
        {
            getAnimeIp(page);
        }
    }
}

///动漫季上一页
void Widget::on_pushButton_AS_PrePage_clicked()
{
    if(mConnectedMysql)
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
    if(mConnectedMysql)
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
    if(mConnectedMysql)
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
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_AE_Page->text().toInt() + 1;
        if(page <= mAEPageTotal)
        {
            getAnimeEpisode(page);
        }
    }
}

///Limit
void Widget::on_checkBox_Limit_clicked(bool checked)
{
    if(checked)
    {
        if(ui->lineEdit_Limit->text() == "showmeall123")
        {
            mLimit = false;
            emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_RECENT, mLimit);
            emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_RECENT, mLimit);
            emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_RECENT, mLimit);
            on_pushButton_FindAnime_clicked();
            on_pushButton_FindMovie_clicked();
            on_pushButton_FindTv_clicked();
        }
        else
        {
            ui->checkBox_Limit->setChecked(false);
        }
    }
    else
    {
        mLimit = true;
        emit gIPD.SIGNALSendQuery(SOT_SELECT_ANIME_RECENT, mLimit);
        emit gIPD.SIGNALSendQuery(SOT_SELECT_MOVIE_RECENT, mLimit);
        emit gIPD.SIGNALSendQuery(SOT_SELECT_TV_RECENT, mLimit);
        on_pushButton_FindAnime_clicked();
        on_pushButton_FindMovie_clicked();
        on_pushButton_FindTv_clicked();
    }
    ui->lineEdit_Limit->clear();
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
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    ui->lineEdit_AE_Episode->setStyleSheet("");
    ui->pushButton_AE_EpisodeOk->setEnabled(false);
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.episode = ui->lineEdit_AE_Episode->text().trimmed();
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

///动漫话原始标题改变
void Widget::on_lineEdit_AE_TitleOrigin_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime_ep.eps.at(gIPD.index_anime.e_row).origin)
    {
        ui->lineEdit_AE_TitleOrigin->setStyleSheet("#lineEdit_AE_TitleOrigin{background-color:#fcae74;}");
        ui->pushButton_AE_TitleOriginOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AE_TitleOrigin->setStyleSheet("");
        ui->pushButton_AE_TitleOriginOk->setEnabled(false);
    }
}

///动漫话标题提交
void Widget::on_pushButton_AE_TitleOk_clicked()
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    ui->lineEdit_AE_Title->setStyleSheet("");
    ui->pushButton_AE_TitleOk->setEnabled(false);
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.title = ui->lineEdit_AE_Title->text().trimmed();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_TITLE, var_send);
}

///动漫话原始标题提交
void Widget::on_pushButton_AE_TitleOriginOk_clicked()
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = true;
    ui->lineEdit_AE_TitleOrigin->setStyleSheet("");
    ui->pushButton_AE_TitleOriginOk->setEnabled(false);
    AnimeEpisodeData episode = gIPD.anime_ep.eps.at(gIPD.index_anime.e_row);
    episode.origin = ui->lineEdit_AE_TitleOrigin->text().trimmed();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_EPISODE_TITLE_ORIGIN, var_send);
}

///动漫话tag1提交
void Widget::on_checkBox_AE_Tag1_clicked(bool checked)
{
    gIPD.index_anime.p_click = true;
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
    gIPD.index_anime.p_click = true;
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
    gIPD.index_anime.p_click = true;
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
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AS_Name->setStyleSheet("");
    ui->pushButton_AS_NaneOk->setEnabled(false);
    AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
    season.name = ui->lineEdit_AS_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_NAME, var_send);
}

///动漫季发布日期有效
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
    gIPD.index_anime.p_click = true;
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
        gIPD.index_anime.p_click = true;
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
void Widget::on_checkBox_AP_Zhuifan_clicked(bool checked)
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    AnimeIpData ip = gIPD.anime_ip.ips.at(gIPD.index_anime.p_row);
    ip.zhuifan = checked;
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_ZHUIFAN, var_send);
}

///动漫公开提交
void Widget::on_checkBox_AP_Display_clicked(bool checked)
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    AnimeIpData ip = gIPD.anime_ip.ips.at(gIPD.index_anime.p_row);
    ip.display = checked;
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_DISPLAY, var_send);
}

///动漫季公开提交
void Widget::on_checkBox_AS_Display_clicked(bool checked)
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    AnimeSeasonData season = gIPD.anime_season.seasons.at(gIPD.index_anime.s_row);
    season.display = checked ? 1 : 0;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_SEASON_DISPLAY, var_send);
}

///动漫季要收藏提交
void Widget::on_checkBox_AS_CollectIt_clicked(bool checked)
{
    ui->checkBox_AS_CollectOk->setEnabled(checked);
    ui->checkBox_AS_CollectOk->setChecked(false);
    gIPD.index_anime.p_click = true;
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
    gIPD.index_anime.p_click = true;
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
    if(mConnectedMysql)
    {
        genFindAnimeSql();
        getAnimeIp(1);
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
void Widget::on_lineEdit_AP_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime_ip.ips.at(gIPD.index_anime.p_row).name)
    {
        ui->lineEdit_AP_Name->setStyleSheet("#lineEdit_AP_Name{background-color:#fcae74;}");
        ui->pushButton_AP_NameOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AP_Name->setStyleSheet("");
        ui->pushButton_AP_NameOk->setEnabled(false);
    }
}

///动漫名称提交
void Widget::on_pushButton_AP_NameOk_clicked()
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AP_Name->setStyleSheet("");
    ui->pushButton_AP_NameOk->setEnabled(false);
    AnimeIpData ip = gIPD.anime_ip.ips.at(gIPD.index_anime.p_row);
    ip.name = ui->lineEdit_AP_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_NAME, var_send);
}

///动漫关键词改变
void Widget::on_lineEdit_AP_Keyword_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime_ip.ips.at(gIPD.index_anime.p_row).keywords)
    {
        ui->lineEdit_AP_Keyword->setStyleSheet("#lineEdit_AP_Keyword{background-color:#fcae74;}");
        ui->pushButton_AP_KeywordsOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AP_Keyword->setStyleSheet("");
        ui->pushButton_AP_KeywordsOk->setEnabled(false);
    }
}

///动漫关键词提交
void Widget::on_pushButton_AP_KeywordsOk_clicked()
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AP_Keyword->setStyleSheet("");
    ui->pushButton_AP_KeywordsOk->setEnabled(false);
    AnimeIpData ip = gIPD.anime_ip.ips.at(gIPD.index_anime.p_row);
    ip.keywords = ui->lineEdit_AP_Keyword->text().trimmed();
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_KEYWORDS, var_send);
}

///新增动漫ip
void Widget::on_pushButton_AP_New_clicked()
{
    mAnimeIpNewDialog->Hi();
}

///删除动漫ip
void Widget::on_pushButton_AP_Delete_clicked()
{
    int row = ui->listWidget_AP->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择动漫"), tr("请选择要删除的动漫"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除动漫《%1》?\n注意: 其包含的所有季度和话数据均会被关联删除!")).arg(gIPD.anime_ip.ips.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            if(mAnimeRecentMode.enable)
            {
                mAnimeRecentMode.enable = false;
                ui->label_A_LockRecent->setVisible(false);
                ui->label_A_UnlockRecent->setVisible(false);
                genFindAnimeSql();
            }
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_IP, gIPD.anime_ip.ips.at(row).pid);
        }
    }
}

///新增动漫季
void Widget::on_pushButton_AS_New_clicked()
{
    mAnimeSeasonNewDialog->Hi(gIPD.index_anime.pid);
}

///删除动漫季
void Widget::on_pushButton_AS_Delete_clicked()
{
    int row = ui->listWidget_AS->currentRow();
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
            gIPD.index_anime.p_click = true;
            gIPD.index_anime.s_click = false;
            gIPD.index_anime.e_click = false;
            if(mAnimeRecentMode.enable)
            {
                mAnimeRecentMode.enable = false;
                ui->label_A_LockRecent->setVisible(false);
                ui->label_A_UnlockRecent->setVisible(false);
                genFindAnimeSql();
            }
            QVariant var_send;
            var_send.setValue(gIPD.anime_season.seasons.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_SEASON, var_send);
        }
    }
}

///新增动漫话
void Widget::on_pushButton_AE_New_clicked()
{
    mAnimeEpisodeNewDialog->Hi(gIPD.index_anime.pid, gIPD.index_anime.sid);
}

///删除动漫话
void Widget::on_pushButton_AE_Delete_clicked()
{
    int row = ui->listWidget_AE->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择话"), tr("请选择要删除的话"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除话\n[%1]\n%2\n%3")).arg(gIPD.anime_ep.eps.at(row).episode, gIPD.anime_ep.eps.at(row).title, gIPD.anime_ep.eps.at(row).origin), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            gIPD.index_anime.p_click = true;
            gIPD.index_anime.s_click = true;
            gIPD.index_anime.e_click = false;
            QVariant var_send;
            var_send.setValue(gIPD.anime_ep.eps.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_EPISODE, var_send);
        }
    }
}

///重置条件
void Widget::on_pushButton_FindMovieReset_clicked()
{
    ui->lineEdit_FindMovieName->clear();
    ui->checkBox_FindMovieNotsee->setChecked(false);
    ui->comboBox_FindMoviePointA->setCurrentIndex(0);
    ui->comboBox_FindMoviePointB->setCurrentIndex(12);
}

///检索电影
void Widget::on_pushButton_FindMovie_clicked()
{
    if(mConnectedMysql)
    {
        genFindMovieSql();
        getMovieIp(1);
    }
}

///电影ip点击
void Widget::on_listWidget_MP_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_MP->row(item);
    MovieIpData ip = gIPD.movie_ip.ips.at(row);
    gIPD.index_movie.p_row = row;
    gIPD.index_movie.pid = gIPD.movie_ip.ips.at(row).pid;
    gIPD.index_movie.p_pos = ui->listWidget_MP->verticalScrollBar()->value();
    showBarMovieId(1);
    getMovieSeason(gIPD.index_movie.s_click?gIPD.index_movie.s_page:1);
    ui->checkBox_MP_Display->setChecked(ip.display);
    ui->lineEdit_MP_Name->setText(ip.name);
    ui->lineEdit_MP_Keyword->setText(ip.keywords);
}

///电影部点击
void Widget::on_listWidget_MS_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_MS->row(item);
    MovieSeasonData season = gIPD.movie_season.seasons.at(row);
    gIPD.index_movie.s_row = row;
    gIPD.index_movie.sid = season.sid;
    gIPD.index_movie.s_pos = ui->listWidget_MS->verticalScrollBar()->value();
    showBarMovieId(2);
    ui->lineEdit_MS_Name->setText(season.name);
    if(season.release_date_valid)
    {
        ui->checkBox_MS_ReleaseDateEnable->setChecked(true);
        ui->dateEdit_MS_ReleaseDate->setDate(season.release_date);
        ui->dateEdit_MS_ReleaseDate->setVisible(true);
    }
    else
    {
        ui->checkBox_MS_ReleaseDateEnable->setChecked(false);
        ui->dateEdit_MS_ReleaseDate->setDate(QDate::currentDate());
        ui->dateEdit_MS_ReleaseDate->setVisible(false);
    }
    ui->comboBox_MS_Point->setCurrentIndex(season.point);
    if(season.collect == 0)
    {   //不收藏
        ui->checkBox_MS_CollectIt->setChecked(false);
        ui->checkBox_MS_CollectOk->setChecked(false);
        ui->checkBox_MS_CollectOk->setEnabled(false);
    }
    else if(season.collect == 1)
    {
        ui->checkBox_MS_CollectIt->setChecked(true);
        ui->checkBox_MS_CollectOk->setChecked(false);
        ui->checkBox_MS_CollectOk->setEnabled(true);
    }
    else if(season.collect == 2)
    {
        ui->checkBox_MS_CollectIt->setChecked(true);
        ui->checkBox_MS_CollectOk->setChecked(true);
        ui->checkBox_MS_CollectOk->setEnabled(true);
    }
    ui->checkBox_MS_Tag1->setChecked(season.tag1);
    ui->checkBox_MS_Tag2->setChecked(season.tag2);
    ui->checkBox_MS_Tag3->setChecked(season.tag3);
}

///电影ip上一页
void Widget::on_pushButton_MP_PrePage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_MP_Page->text().toInt() - 1;
        if(page > 0)
        {
            getMovieIp(page);
        }
    }
}

///电影ip下一页
void Widget::on_pushButton_MP_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_MP_Page->text().toInt() + 1;
        if(page <= mMPPageTotal)
        {
            getMovieIp(page);
        }
    }
}

///电影季上一页
void Widget::on_pushButton_MS_PrePage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_MS_Page->text().toInt() - 1;
        if(page > 0)
        {
            getMovieSeason(page);
        }
    }
}

///电影季下一页
void Widget::on_pushButton_MS_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_MS_Page->text().toInt() + 1;
        if(page <= mMSPageTotal)
        {
            getMovieSeason(page);
        }
    }
}

///新增电影ip
void Widget::on_pushButton_MP_New_clicked()
{
    mMovieIpNewDialog->Hi();
}

///删除电影ip
void Widget::on_pushButton_MP_Delete_clicked()
{
    int row = ui->listWidget_MP->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择电影"), tr("请选择要删除的电影"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除电影《%1》?\n注意: 其包含的所有部数据均会被关联删除!")).arg(gIPD.movie_ip.ips.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            emit gIPD.SIGNALSendQuery(SOT_DELETE_MOVIE_IP, gIPD.movie_ip.ips.at(row).pid);
        }
    }
}

///新增电影部
void Widget::on_pushButton_MS_New_clicked()
{
    mMovieSeasonNewDialog->Hi(gIPD.index_movie.pid);
}

///删除电影部
void Widget::on_pushButton_MS_Delete_clicked()
{
    int row = ui->listWidget_MS->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择部"), tr("请选择要删除的部"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除部《%1》?")).arg(gIPD.movie_season.seasons.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            gIPD.index_movie.p_click = true;
            gIPD.index_movie.s_click = false;
            QVariant var_send;
            var_send.setValue(gIPD.movie_season.seasons.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_MOVIE_SEASON, var_send);
        }
    }
}

///电影名称改变
void Widget::on_lineEdit_MP_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.movie_ip.ips.at(gIPD.index_movie.p_row).name)
    {
        ui->lineEdit_MP_Name->setStyleSheet("#lineEdit_MP_Name{background-color:#fcae74;}");
        ui->pushButton_MP_NameOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_MP_Name->setStyleSheet("");
        ui->pushButton_MP_NameOk->setEnabled(false);
    }
}

///电影名称提交
void Widget::on_pushButton_MP_NameOk_clicked()
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = false;
    ui->lineEdit_MP_Name->setStyleSheet("");
    ui->pushButton_MP_NameOk->setEnabled(false);
    MovieIpData ip = gIPD.movie_ip.ips.at(gIPD.index_movie.p_row);
    ip.name = ui->lineEdit_MP_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_IP_NAME, var_send);
}

///电影公开提交
void Widget::on_checkBox_MP_Display_clicked(bool checked)
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = false;
    MovieIpData ip = gIPD.movie_ip.ips.at(gIPD.index_movie.p_row);
    ip.display = checked;
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_IP_DISPLAY, var_send);
}

///电影关键词改变
void Widget::on_lineEdit_MP_Keyword_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.movie_ip.ips.at(gIPD.index_movie.p_row).keywords)
    {
        ui->lineEdit_MP_Keyword->setStyleSheet("#lineEdit_MP_Keyword{background-color:#fcae74;}");
        ui->pushButton_MP_KeywordsOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_MP_Keyword->setStyleSheet("");
        ui->pushButton_MP_KeywordsOk->setEnabled(false);
    }
}

///电影关键词提交
void Widget::on_pushButton_MP_KeywordsOk_clicked()
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = false;
    ui->lineEdit_MP_Keyword->setStyleSheet("");
    ui->pushButton_MP_KeywordsOk->setEnabled(false);
    MovieIpData ip = gIPD.movie_ip.ips.at(gIPD.index_movie.p_row);
    ip.keywords = ui->lineEdit_MP_Keyword->text().trimmed();
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_IP_KEYWORDS, var_send);
}

///电影部名称改变
void Widget::on_lineEdit_MS_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.movie_season.seasons.at(gIPD.index_movie.s_row).name)
    {
        ui->lineEdit_MS_Name->setStyleSheet("#lineEdit_MS_Name{background-color:#fcae74;}");
        ui->pushButton_MS_NaneOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_MS_Name->setStyleSheet("");
        ui->pushButton_MS_NaneOk->setEnabled(false);
    }
}

///电影部名称提交
void Widget::on_pushButton_MS_NaneOk_clicked()
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    ui->lineEdit_MS_Name->setStyleSheet("");
    ui->pushButton_MS_NaneOk->setEnabled(false);
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.name = ui->lineEdit_MS_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_NAME, var_send);
}

///电影部发布日期有效
void Widget::on_checkBox_MS_ReleaseDateEnable_clicked(bool checked)
{
    ui->dateEdit_MS_ReleaseDate->setVisible(checked);
    bool can_update = false;
    if(checked && (gIPD.movie_season.seasons.at(gIPD.index_movie.s_row).release_date != ui->dateEdit_MS_ReleaseDate->date()))
    {
        can_update = true;
    }
    if(checked != gIPD.movie_season.seasons.at(gIPD.index_movie.s_row).release_date_valid)
    {
        can_update = true;
    }
    ui->pushButton_MS_ReleaseDateOk->setEnabled(can_update);
}

///电影部发布日期改变
void Widget::on_dateEdit_MS_ReleaseDate_dateChanged(const QDate &date)
{
    bool can_update = false;
    if(ui->checkBox_MS_ReleaseDateEnable->isChecked() && (gIPD.movie_season.seasons.at(gIPD.index_movie.s_row).release_date != date))
    {
        can_update = true;
    }
    if(ui->checkBox_MS_ReleaseDateEnable->isChecked() != gIPD.movie_season.seasons.at(gIPD.index_movie.s_row).release_date_valid)
    {
        can_update = true;
    }
    ui->pushButton_MS_ReleaseDateOk->setEnabled(can_update);
}

///电影部发布日期提交
void Widget::on_pushButton_MS_ReleaseDateOk_clicked()
{
    ui->pushButton_MS_ReleaseDateOk->setEnabled(false);
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.release_date_valid = ui->checkBox_MS_ReleaseDateEnable->isChecked();
    season.release_date = ui->dateEdit_MS_ReleaseDate->date();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_RELEASE_DATE, var_send);
}

///电影部评分改变
void Widget::on_comboBox_MS_Point_activated(int index)
{
    if(gIPD.movie_season.seasons.at(gIPD.index_movie.s_row).point != index)
    {
        gIPD.index_movie.p_click = true;
        gIPD.index_movie.s_click = true;
        MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
        season.point = index;
        QVariant var_send;
        var_send.setValue(season);
        emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_POINT, var_send);
    }
}

///电影部要收藏提交
void Widget::on_checkBox_MS_CollectIt_clicked(bool checked)
{
    ui->checkBox_MS_CollectOk->setEnabled(checked);
    ui->checkBox_MS_CollectOk->setChecked(false);
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.collect = checked ? 1 : 0;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_COLLECT, var_send);
}

///电影部已收藏提交
void Widget::on_checkBox_MS_CollectOk_clicked(bool checked)
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.collect = checked ? 2 : 1;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_COLLECT, var_send);
}

///电影部tag1提交
void Widget::on_checkBox_MS_Tag1_clicked(bool checked)
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.tag1 = checked;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_TAG1, var_send);
}

///电影部tag2提交
void Widget::on_checkBox_MS_Tag2_clicked(bool checked)
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.tag2 = checked;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_TAG2, var_send);
}

///电影部tag3提交
void Widget::on_checkBox_MS_Tag3_clicked(bool checked)
{
    gIPD.index_movie.p_click = true;
    gIPD.index_movie.s_click = true;
    MovieSeasonData season = gIPD.movie_season.seasons.at(gIPD.index_movie.s_row);
    season.tag3 = checked;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_MOVIE_SEASON_TAG3, var_send);
}

///重置条件
void Widget::on_pushButton_FindTvReset_clicked()
{
    ui->lineEdit_FindTvName->clear();
    ui->checkBox_FindTvZhuiju->setChecked(false);
    ui->checkBox_FindTvNotsee->setChecked(false);
    ui->comboBox_FindTvPointA->setCurrentIndex(0);
    ui->comboBox_FindTvPointB->setCurrentIndex(12);
}

///检索电视剧
void Widget::on_pushButton_FindTv_clicked()
{
    if(mConnectedMysql)
    {
        genFindTvSql();
        getTvIp(1);
    }
}

///电视剧ip点击
void Widget::on_listWidget_TP_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_TP->row(item);
    TvIpData ip = gIPD.tv_ip.ips.at(row);
    gIPD.index_tv.p_row = row;
    gIPD.index_tv.pid = gIPD.tv_ip.ips.at(row).pid;
    gIPD.index_tv.p_pos = ui->listWidget_TP->verticalScrollBar()->value();
    showBarTvId(1);
    getTvSeason(gIPD.index_tv.s_click?gIPD.index_tv.s_page:1);
    ui->checkBox_TP_Display->setChecked(ip.display);
    ui->checkBox_TP_Zhuiju->setChecked(ip.zhuiju);
    ui->lineEdit_TP_Name->setText(ip.name);
    ui->lineEdit_TP_Keyword->setText(ip.keywords);
}

///电视剧部点击
void Widget::on_listWidget_TS_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_TS->row(item);
    TvSeasonData season = gIPD.tv_season.seasons.at(row);
    gIPD.index_tv.s_row = row;
    gIPD.index_tv.sid = season.sid;
    gIPD.index_tv.s_pos = ui->listWidget_TS->verticalScrollBar()->value();
    showBarTvId(2);
    getTvEpisode(gIPD.index_tv.e_click?gIPD.index_tv.e_page:1);
    ui->lineEdit_TS_Name->setText(season.name);
    ui->checkBox_TS_Display->setChecked(season.display);
    if(season.release_date_valid)
    {
        ui->checkBox_TS_ReleaseDateEnable->setChecked(true);
        ui->dateEdit_TS_ReleaseDate->setDate(season.release_date);
        ui->dateEdit_TS_ReleaseDate->setVisible(true);
    }
    else
    {
        ui->checkBox_TS_ReleaseDateEnable->setChecked(false);
        ui->dateEdit_TS_ReleaseDate->setDate(QDate::currentDate());
        ui->dateEdit_TS_ReleaseDate->setVisible(false);
    }
    ui->comboBox_TS_Point->setCurrentIndex(season.point);
    if(season.collect == 0)
    {   //不收藏
        ui->checkBox_TS_CollectIt->setChecked(false);
        ui->checkBox_TS_CollectOk->setChecked(false);
        ui->checkBox_TS_CollectOk->setEnabled(false);
    }
    else if(season.collect == 1)
    {
        ui->checkBox_TS_CollectIt->setChecked(true);
        ui->checkBox_TS_CollectOk->setChecked(false);
        ui->checkBox_TS_CollectOk->setEnabled(true);
    }
    else if(season.collect == 2)
    {
        ui->checkBox_TS_CollectIt->setChecked(true);
        ui->checkBox_TS_CollectOk->setChecked(true);
        ui->checkBox_TS_CollectOk->setEnabled(true);
    }
}

///电视剧集点击
void Widget::on_listWidget_TE_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_TE->row(item);
    gIPD.index_tv.e_row = row;
    gIPD.index_tv.eid = gIPD.tv_ep.eps.at(row).eid;
    gIPD.index_tv.e_pos = ui->listWidget_TE->verticalScrollBar()->value();
    showBarTvId(3);
    ui->checkBox_TE_Tag1->setChecked(gIPD.tv_ep.eps.at(row).tag1);
    ui->checkBox_TE_Tag2->setChecked(gIPD.tv_ep.eps.at(row).tag2);
    ui->checkBox_TE_Tag3->setChecked(gIPD.tv_ep.eps.at(row).tag3);
    ui->lineEdit_TE_Episode->setText(gIPD.tv_ep.eps.at(row).episode);
    ui->lineEdit_TE_Title->setText(gIPD.tv_ep.eps.at(row).title);
}

///电视剧ip上一页
void Widget::on_pushButton_TP_PrePage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_TP_Page->text().toInt() - 1;
        if(page > 0)
        {
            getTvIp(page);
        }
    }
}

///电视剧ip下一页
void Widget::on_pushButton_TP_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_TP_Page->text().toInt() + 1;
        if(page <= mTPPageTotal)
        {
            getTvIp(page);
        }
    }
}

///电视剧部上一页
void Widget::on_pushButton_TS_PrePage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_TS_Page->text().toInt() - 1;
        if(page > 0)
        {
            getTvSeason(page);
        }
    }
}

///电视剧部下一页
void Widget::on_pushButton_TS_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_TS_Page->text().toInt() + 1;
        if(page <= mTSPageTotal)
        {
            getTvSeason(page);
        }
    }
}

///电视剧集上一页
void Widget::on_pushButton_TE_PrePage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_TE_Page->text().toInt() - 1;
        if(page > 0)
        {
            getTvEpisode(page);
        }
    }
}

///电视剧集下一页
void Widget::on_pushButton_TE_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_TE_Page->text().toInt() + 1;
        if(page <= mTEPageTotal)
        {
            getTvEpisode(page);
        }
    }
}

///新增电视剧ip
void Widget::on_pushButton_TP_New_clicked()
{
    mTvIpNewDialog->Hi();
}

///新增电视剧部
void Widget::on_pushButton_TS_New_clicked()
{
    mTvSeasonNewDialog->Hi(gIPD.index_tv.pid);
}

///新增电视剧集
void Widget::on_pushButton_TE_New_clicked()
{
    mTvEpisodeNewDialog->Hi(gIPD.index_tv.pid, gIPD.index_tv.sid);
}

///删除电视剧ip
void Widget::on_pushButton_TP_Delete_clicked()
{
    int row = ui->listWidget_TP->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择电视剧"), tr("请选择要删除的电视剧"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除电视剧《%1》?\n注意: 其包含的所有部和集数据均会被关联删除!")).arg(gIPD.tv_ip.ips.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            if(mTvRecentMode.enable)
            {
                mTvRecentMode.enable = false;
                ui->label_T_LockRecent->setVisible(false);
                ui->label_T_UnlockRecent->setVisible(false);
                genFindTvSql();
            }
            emit gIPD.SIGNALSendQuery(SOT_DELETE_TV_IP, gIPD.tv_ip.ips.at(row).pid);
        }
    }
}

///删除电视剧部
void Widget::on_pushButton_TS_Delete_clicked()
{
    int row = ui->listWidget_TS->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择部"), tr("请选择要删除的部"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除部《%1》?\n注意: 其包含的所有集数据均会被关联删除!")).arg(gIPD.tv_season.seasons.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            gIPD.index_tv.p_click = true;
            gIPD.index_tv.s_click = false;
            gIPD.index_tv.e_click = false;
            if(mTvRecentMode.enable)
            {
                mTvRecentMode.enable = false;
                ui->label_T_LockRecent->setVisible(false);
                ui->label_T_UnlockRecent->setVisible(false);
                genFindTvSql();
            }
            QVariant var_send;
            var_send.setValue(gIPD.tv_season.seasons.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_TV_SEASON, var_send);
        }
    }
}

///删除电视剧集
void Widget::on_pushButton_TE_Delete_clicked()
{
    int row = ui->listWidget_TE->currentRow();
    if(row == -1)
    {
        QMessageBox::information(this, tr("未选择集"), tr("请选择要删除的集"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除集《[%1] %2》?")).arg(gIPD.tv_ep.eps.at(row).episode, gIPD.tv_ep.eps.at(row).title), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            gIPD.index_tv.p_click = true;
            gIPD.index_tv.s_click = true;
            gIPD.index_tv.e_click = false;
            QVariant var_send;
            var_send.setValue(gIPD.tv_ep.eps.at(row));
            emit gIPD.SIGNALSendQuery(SOT_DELETE_TV_EPISODE, var_send);
        }
    }
}

///电视剧名称改变
void Widget::on_lineEdit_TP_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.tv_ip.ips.at(gIPD.index_tv.p_row).name)
    {
        ui->lineEdit_TP_Name->setStyleSheet("#lineEdit_TP_Name{background-color:#fcae74;}");
        ui->pushButton_TP_NameOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_TP_Name->setStyleSheet("");
        ui->pushButton_TP_NameOk->setEnabled(false);
    }
}

///电视剧名称提交
void Widget::on_pushButton_TP_NameOk_clicked()
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = false;
    gIPD.index_tv.e_click = false;
    ui->lineEdit_TP_Name->setStyleSheet("");
    ui->pushButton_TP_NameOk->setEnabled(false);
    TvIpData ip = gIPD.tv_ip.ips.at(gIPD.index_tv.p_row);
    ip.name = ui->lineEdit_TP_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_IP_NAME, var_send);
}

///电视剧公开提交
void Widget::on_checkBox_TP_Display_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = false;
    gIPD.index_tv.e_click = false;
    TvIpData ip = gIPD.tv_ip.ips.at(gIPD.index_tv.p_row);
    ip.display = checked;
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_IP_DISPLAY, var_send);
}

///电视剧追剧提交
void Widget::on_checkBox_TP_Zhuiju_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = false;
    gIPD.index_tv.e_click = false;
    TvIpData ip = gIPD.tv_ip.ips.at(gIPD.index_tv.p_row);
    ip.zhuiju = checked;
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_IP_ZHUIJU, var_send);
}

///电视剧关键词改变
void Widget::on_lineEdit_TP_Keyword_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.tv_ip.ips.at(gIPD.index_tv.p_row).keywords)
    {
        ui->lineEdit_TP_Keyword->setStyleSheet("#lineEdit_TP_Keyword{background-color:#fcae74;}");
        ui->pushButton_TP_KeywordsOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_TP_Keyword->setStyleSheet("");
        ui->pushButton_TP_KeywordsOk->setEnabled(false);
    }
}

///电视剧关键词提交
void Widget::on_pushButton_TP_KeywordsOk_clicked()
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = false;
    gIPD.index_tv.e_click = false;
    ui->lineEdit_TP_Keyword->setStyleSheet("");
    ui->pushButton_TP_KeywordsOk->setEnabled(false);
    TvIpData ip = gIPD.tv_ip.ips.at(gIPD.index_tv.p_row);
    ip.keywords = ui->lineEdit_TP_Keyword->text().trimmed();
    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_IP_KEYWORDS, var_send);
}

///电视剧部名称改变
void Widget::on_lineEdit_TS_Name_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).name)
    {
        ui->lineEdit_TS_Name->setStyleSheet("#lineEdit_TS_Name{background-color:#fcae74;}");
        ui->pushButton_TS_NaneOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_TS_Name->setStyleSheet("");
        ui->pushButton_TS_NaneOk->setEnabled(false);
    }
}

///电视剧部名称提交
void Widget::on_pushButton_TS_NaneOk_clicked()
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = false;
    ui->lineEdit_TS_Name->setStyleSheet("");
    ui->pushButton_TS_NaneOk->setEnabled(false);
    TvSeasonData season = gIPD.tv_season.seasons.at(gIPD.index_tv.s_row);
    season.name = ui->lineEdit_TS_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_SEASON_NAME, var_send);
}

///电视剧部公开提交
void Widget::on_checkBox_TS_Display_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = false;
    TvSeasonData season = gIPD.tv_season.seasons.at(gIPD.index_tv.s_row);
    season.display = checked ? 1 : 0;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_SEASON_DISPLAY, var_send);
}

///电视剧部发布日期有效
void Widget::on_checkBox_TS_ReleaseDateEnable_clicked(bool checked)
{
    ui->dateEdit_TS_ReleaseDate->setVisible(checked);
    bool can_update = false;
    if(checked && (gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).release_date != ui->dateEdit_TS_ReleaseDate->date()))
    {
        can_update = true;
    }
    if(checked != gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).release_date_valid)
    {
        can_update = true;
    }
    ui->pushButton_TS_ReleaseDateOk->setEnabled(can_update);
}

///电视剧部发布日期改变
void Widget::on_dateEdit_TS_ReleaseDate_dateChanged(const QDate &date)
{
    bool can_update = false;
    if(ui->checkBox_TS_ReleaseDateEnable->isChecked() && (gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).release_date != date))
    {
        can_update = true;
    }
    if(ui->checkBox_TS_ReleaseDateEnable->isChecked() != gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).release_date_valid)
    {
        can_update = true;
    }
    ui->pushButton_TS_ReleaseDateOk->setEnabled(can_update);
}

///电视剧部发布日期提交
void Widget::on_pushButton_TS_ReleaseDateOk_clicked()
{
    ui->pushButton_TS_ReleaseDateOk->setEnabled(false);
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = false;
    TvSeasonData season = gIPD.tv_season.seasons.at(gIPD.index_tv.s_row);
    season.release_date_valid = ui->checkBox_TS_ReleaseDateEnable->isChecked();
    season.release_date = ui->dateEdit_TS_ReleaseDate->date();
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_SEASON_RELEASE_DATE, var_send);
}

///电视剧部评分改变
void Widget::on_comboBox_TS_Point_activated(int index)
{
    if(gIPD.tv_season.seasons.at(gIPD.index_tv.s_row).point != index)
    {
        gIPD.index_tv.p_click = true;
        gIPD.index_tv.s_click = true;
        gIPD.index_tv.e_click = false;
        TvSeasonData season = gIPD.tv_season.seasons.at(gIPD.index_tv.s_row);
        season.point = index;
        QVariant var_send;
        var_send.setValue(season);
        emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_SEASON_POINT, var_send);
    }
}

///电视剧部要收藏提交
void Widget::on_checkBox_TS_CollectIt_clicked(bool checked)
{
    ui->checkBox_TS_CollectOk->setEnabled(checked);
    ui->checkBox_TS_CollectOk->setChecked(false);
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = false;
    TvSeasonData season = gIPD.tv_season.seasons.at(gIPD.index_tv.s_row);
    season.collect = checked ? 1 : 0;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_SEASON_COLLECT, var_send);
}

///电视剧部已收藏提交
void Widget::on_checkBox_TS_CollectOk_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = false;
    TvSeasonData season = gIPD.tv_season.seasons.at(gIPD.index_tv.s_row);
    season.collect = checked ? 2 : 1;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_SEASON_COLLECT, var_send);
}

///电视剧集序号改变
void Widget::on_lineEdit_TE_Episode_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.tv_ep.eps.at(gIPD.index_tv.e_row).episode)
    {
        ui->lineEdit_TE_Episode->setStyleSheet("#lineEdit_TE_Episode{background-color:#fcae74;}");
        ui->pushButton_TE_EpisodeOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_TE_Episode->setStyleSheet("");
        ui->pushButton_TE_EpisodeOk->setEnabled(false);
    }
}

///电视剧集序号提交
void Widget::on_pushButton_TE_EpisodeOk_clicked()
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = true;
    ui->lineEdit_TE_Episode->setStyleSheet("");
    ui->pushButton_TE_EpisodeOk->setEnabled(false);
    TvEpisodeData episode = gIPD.tv_ep.eps.at(gIPD.index_tv.e_row);
    episode.episode = ui->lineEdit_TE_Episode->text().trimmed();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_EPISODE_EPISODE, var_send);
}

///电视剧集标题改变
void Widget::on_lineEdit_TE_Title_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.tv_ep.eps.at(gIPD.index_tv.e_row).title)
    {
        ui->lineEdit_TE_Title->setStyleSheet("#lineEdit_TE_Title{background-color:#fcae74;}");
        ui->pushButton_TE_TitleOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_TE_Title->setStyleSheet("");
        ui->pushButton_TE_TitleOk->setEnabled(false);
    }
}

///电视剧集标题提交
void Widget::on_pushButton_TE_TitleOk_clicked()
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = true;
    ui->lineEdit_TE_Title->setStyleSheet("");
    ui->pushButton_TE_TitleOk->setEnabled(false);
    TvEpisodeData episode = gIPD.tv_ep.eps.at(gIPD.index_tv.e_row);
    episode.title = ui->lineEdit_TE_Title->text().trimmed();
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_EPISODE_TITLE, var_send);
}

///电视剧集tag1提交
void Widget::on_checkBox_TE_Tag1_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = true;
    TvEpisodeData episode = gIPD.tv_ep.eps.at(gIPD.index_tv.e_row);
    episode.tag1 = checked;
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_EPISODE_TAG1, var_send);
}

///电视剧集tag2提交
void Widget::on_checkBox_TE_Tag2_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = true;
    TvEpisodeData episode = gIPD.tv_ep.eps.at(gIPD.index_tv.e_row);
    episode.tag2 = checked;
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_EPISODE_TAG2, var_send);
}

///电视剧集tag3提交
void Widget::on_checkBox_TE_Tag3_clicked(bool checked)
{
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = true;
    TvEpisodeData episode = gIPD.tv_ep.eps.at(gIPD.index_tv.e_row);
    episode.tag3 = checked;
    QVariant var_send;
    var_send.setValue(episode);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_TV_EPISODE_TAG3, var_send);
}
