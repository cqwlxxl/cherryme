#include "Widget.h"
#include "ui_Widget.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>

#include "UiThread/Anime/AnimeIpWidgetItem.h"

Q_DECLARE_METATYPE(AnimeData)
Q_DECLARE_METATYPE(AnimeSeasonData)
Q_DECLARE_METATYPE(AnimeEpisodeData)
Q_DECLARE_METATYPE(AnimeRecentData)

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
    case SOT_LOGIN_ANIME:
        mConnectedMysql = var.toBool();
        ui->pushButton_ConnectMysql->setText(mConnectedMysql?QString("%2:%1(%3)").arg(MYSQL_PORT).arg(MYSQL_IP, tr("点击断开")):tr("连接服务器"));
        ui->pushButton_ConnectMysql->setChecked(mConnectedMysql);
        if(mConnectedMysql)
        {   //连接上服务器了
            emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_RECENT, mLimit);
            on_pushButton_FindAnime_clicked();
        }
        else
        {   //断开服务器
            mAnimeRecents.clear();
            mAnimeRecentMode.enable = false;
            ui->label_A_LockRecent->setVisible(false);
            ui->label_A_UnlockRecent->setVisible(false);
            setAnimeRecentLabel();
            showBarAnimeId(-1);
            ui->listWidget_AP->clear();
            ui->listWidget_AS->clear();
            ui->listWidget_AE->clear();
        }
        break;
    case SOT_GET_ANIME_IP:
    {
        gIPD.anime.animes = var.value<QList<AnimeData> >();
        gIPD.anime.items.clear();
        gIPD.anime.widgets.clear();
        ui->listWidget_AP->clear();
        ui->listWidget_AS->clear();
        ui->listWidget_AE->clear();
        if(gIPD.anime.animes.size() == 0)
        {
            break;
        }
        for(int i = 0; i < gIPD.anime.animes.size(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();  //初始化item
            AnimeIpWidgetItem *widget = new AnimeIpWidgetItem(gIPD.anime.animes.at(i), ui->listWidget_AP);
            item->setSizeHint(QSize(0, widget->height()));  //设置自定义item高度
            ui->listWidget_AP->insertItem(i, item);
            ui->listWidget_AP->setItemWidget(item, widget);
            gIPD.anime.items.append(item);
            gIPD.anime.widgets.append(widget);
        }
        if(mAnimeRecentMode.enable)
        {   //最近模式
            on_listWidget_AP_itemClicked(gIPD.anime.items.at(0));
        }
        else if(gIPD.index_anime.p_click)
        {
            gIPD.index_anime.p_click = false;
            //查看是否还有显示
            bool has {false};
            for(int j = 0; j < gIPD.anime.animes.size(); j++)
            {
                if(gIPD.anime.animes.at(j).pid == gIPD.index_anime.pid)
                {
                    has = true;
                    break;
                }
            }
            if(has)
            {
                ui->listWidget_AP->verticalScrollBar()->setValue(gIPD.index_anime.p_pos);
                ui->listWidget_AP->setCurrentRow(gIPD.index_anime.p_row);
                on_listWidget_AP_itemClicked(gIPD.anime.items.at(gIPD.index_anime.p_row));
            }
            else
            {
                gIPD.index_anime.s_click = false;
                gIPD.index_anime.e_click = false;
            }
        }
        break;
    }
    case SOT_GET_ANIME_RECENT:
    {
        mAnimeRecents = var.value<QList<AnimeRecentData> >();
        setAnimeRecentLabel();
    }
        break;
    case SOT_ANIME_IP_PAGE:
    {
        QStringList strs = var.toStringList();
        mAAPageTotal = strs[1].toInt();
        ui->lineEdit_AP_Page->setText(strs[0]);
        ui->label_AP_PageTotal->setText("/"+strs[1]);
        ui->label_AP_Total->setText(QString(tr("共%1个系列")).arg(strs[2]));
    }
        break;
    case SOT_GET_ANIME_SEASON:
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
            ui->listWidget_AS->verticalScrollBar()->setValue(gIPD.index_anime.s_pos);
            ui->listWidget_AS->setCurrentRow(gIPD.index_anime.s_row);
            on_listWidget_AS_itemClicked(gIPD.anime_season.items.at(gIPD.index_anime.s_row));
        }
        break;
    case SOT_ANIME_SEASON_PAGE:
    {
        QStringList strs = var.toStringList();
        mASPageTotal = strs[1].toInt();
        ui->lineEdit_AS_Page->setText(strs[0]);
        gIPD.index_anime.s_page = ui->lineEdit_AS_Page->text().toInt();
        ui->label_AS_PageTotal->setText("/"+strs[1]);
        ui->label_AS_Total->setText(QString(tr("共%1季")).arg(strs[2]));
    }
        break;
    case SOT_GET_ANIME_EPISODE:
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
    case SOT_ANIME_EPISODE_PAGE:
    {
        QStringList strs = var.toStringList();
        mAEPageTotal = strs[1].toInt();
        ui->lineEdit_AE_Page->setText(strs[0]);
        gIPD.index_anime.e_page = ui->lineEdit_AE_Page->text().toInt();
        ui->label_AE_PageTotal->setText("/"+strs[1]);
        ui->label_AE_Total->setText(QString(tr("共%1话")).arg(strs[2]));
    }
        break;
    case SOT_UPDATE_ANIME:
        getAnimeIp(ui->lineEdit_AP_Page->text().toInt());
        break;
    case SOT_UPDATE_ANIME_EPISODE_SEE:
        emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_RECENT, mLimit);
        getAnimeIp(ui->lineEdit_AP_Page->text().toInt());
        break;
    case SOT_DELETE_ANIME_RECENT:
        emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_RECENT, mLimit);
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

/////连接|断开服务器
void Widget::on_pushButton_ConnectMysql_clicked(bool checked)
{
    emit gIPD.SIGNALSendQuery(SOT_LOGIN_ANIME, checked);
    if(checked)
    {
        ui->pushButton_ConnectMysql->setText(tr("连接中.."));
    }
}

///init
void Widget::qian()
{
    mAnimeAnimeNewDialog = new AnimeIpNewDialog(this);
    mAnimeSeasonNewDialog = new AnimeSeasonNewDialog(this);
    mAnimeEpisodeNewDialog = new AnimeEpisodeNewDialog(this);

    ui->label_BarA_PidText->setVisible(false);
    ui->label_BarA_SidText->setVisible(false);
    ui->label_BarA_EidText->setVisible(false);
    ui->label_BarA_Pid->setVisible(false);
    ui->label_BarA_Sid->setVisible(false);
    ui->label_BarA_Eid->setVisible(false);
    ui->dateEdit_AS_ReleaseDate->setVisible(false);
    ui->checkBox_AS_CollectOk->setEnabled(false);
    ui->stackedWidget_Anime->setCurrentWidget(ui->page_ADefault);

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

    connect(&gIPD, &InterfacePublicData::SIGNALSendQuery, this, &Widget::slotSendQuery, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALReceiveQueryData, this, &Widget::slotReceiveQueryData, Qt::UniqueConnection);
    connect(&gIPD, &InterfacePublicData::SIGNALAnimeEpisodeSee, this, &Widget::slotAnimeEpisodeSee, Qt::UniqueConnection);

    //初始化后操作
    QTimer::singleShot(0, this, [this]{
        showBarAnimeId(-1);
    });
}

///获取动漫
void Widget::getAnimeIp(int page)
{
    showBarAnimeId(0);
    QStringList strs;
    strs << QString::number(page)
         << mFindAnimeSql;
    emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_IP, strs);   //获取动漫
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
    emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_SEASON, strs);   //获取动漫季
}

///获取动漫话
void Widget::getAnimeEpisode(int page)
{
    showBarAnimeId(2);
    QStringList strs;
    strs << QString::number(gIPD.index_anime.pid)
         << QString::number(gIPD.index_anime.sid)
         << QString::number(page);
    emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_EPISODE, strs);   //获取动漫话
}

///显示id条
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
    case 1:     //点击了aa
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
        emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_RECENT, mAnimeRecents.at(index).id);
    }
}

///事件过滤
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_A_UnlockRecent)
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

///动漫点击
void Widget::on_listWidget_AP_itemClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_AP->row(item);
    AnimeData anime = gIPD.anime.animes.at(row);
    gIPD.index_anime.p_row = row;
    gIPD.index_anime.pid = gIPD.anime.animes.at(row).pid;
    gIPD.index_anime.p_pos = ui->listWidget_AP->verticalScrollBar()->value();
    showBarAnimeId(1);
    getAnimeSeason(gIPD.index_anime.s_click?gIPD.index_anime.s_page:1);
    ui->checkBox_AP_Display->setChecked(anime.display);
    ui->checkBox_AA_Zhuifan->setChecked(anime.zhuifan);
    ui->lineEdit_AP_Name->setText(anime.name);
    ui->lineEdit_AP_Keyword->setText(anime.keywords);
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
}

///动漫上一页
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

///动漫下一页
void Widget::on_pushButton_AP_NextPage_clicked()
{
    if(mConnectedMysql)
    {
        int page = ui->lineEdit_AP_Page->text().toInt() + 1;
        if(page <= mAAPageTotal)
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
            emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_RECENT, mLimit);
            on_pushButton_FindAnime_clicked();
        }
        else
        {
            ui->checkBox_Limit->setChecked(false);
        }
    }
    else
    {
        mLimit = true;
        emit gIPD.SIGNALSendQuery(SOT_GET_ANIME_RECENT, mLimit);
        on_pushButton_FindAnime_clicked();
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
void Widget::on_checkBox_AA_Zhuifan_clicked(bool checked)
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.p_row);
    anime.zhuifan = checked;
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_ZHUIFAN, var_send);
}

///动漫公开提交
void Widget::on_checkBox_AP_Display_clicked(bool checked)
{
    gIPD.index_anime.p_click = false;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.p_row);
    anime.display = checked;
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_DISPLAY, var_send);
}

///动漫季公开提交
void Widget::on_checkBox_AS_Display_clicked(bool checked)
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
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
    if(arg1 != gIPD.anime.animes.at(gIPD.index_anime.p_row).name)
    {
        ui->lineEdit_AP_Name->setStyleSheet("#lineEdit_AP_Name{background-color:#fcae74;}");
        ui->pushButton_AP_NaneOk->setEnabled(true);
    }
    else
    {
        ui->lineEdit_AP_Name->setStyleSheet("");
        ui->pushButton_AP_NaneOk->setEnabled(false);
    }
}

///动漫名称提交
void Widget::on_pushButton_AP_NaneOk_clicked()
{
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    ui->lineEdit_AP_Name->setStyleSheet("");
    ui->pushButton_AP_NaneOk->setEnabled(false);
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.p_row);
    anime.name = ui->lineEdit_AP_Name->text().trimmed();
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_NAME, var_send);
}

///动漫关键词改变
void Widget::on_lineEdit_AP_Keyword_textChanged(const QString &arg1)
{
    if(arg1 != gIPD.anime.animes.at(gIPD.index_anime.p_row).keywords)
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
    AnimeData anime = gIPD.anime.animes.at(gIPD.index_anime.p_row);
    anime.keywords = ui->lineEdit_AP_Keyword->text().trimmed();
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_UPDATE_ANIME_IP_KEYWORDS, var_send);
}

///新增动漫
void Widget::on_pushButton_AP_New_clicked()
{
    mAnimeAnimeNewDialog->Hi();
}

///删除动漫
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
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除动漫《%1》?\n注意: 其包含的所有季度和话数据均会被关联删除!")).arg(gIPD.anime.animes.at(row).name), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Ok)
        {
            emit gIPD.SIGNALSendQuery(SOT_DELETE_ANIME_IP, gIPD.anime.animes.at(row).pid);
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
        int ret = QMessageBox::warning(this, tr("警告"), QString(tr("确认删除话《[%1] %2》?")).arg(gIPD.anime_ep.eps.at(row).episode, gIPD.anime_ep.eps.at(row).title), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);
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

