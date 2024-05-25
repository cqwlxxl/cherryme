#include "AnimeEpisodeNewDialog.h"
#include "ui_AnimeEpisodeNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"
#include "UiThread/InterfacePublicData.h"

Q_DECLARE_METATYPE(AnimeEpisodeData)

AnimeEpisodeNewDialog::AnimeEpisodeNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimeEpisodeNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

AnimeEpisodeNewDialog::~AnimeEpisodeNewDialog()
{
    delete ui;
}

///显示
void AnimeEpisodeNewDialog::Hi(int pid, int sid)
{
    mPid = pid;
    mSid = sid;
    updateEpisodePreview();
    this->exec();
}

///新增话
void AnimeEpisodeNewDialog::on_pushButton_Ok_clicked()
{
    if(mEpisodes.size() == 0)
    {
        return;
    }

    QList<AnimeEpisodeData> eps;
    AnimeEpisodeData ep;
    ep.pid = mPid;
    ep.sid = mSid;
    ep.type = ui->comboBox_Type->currentIndex();
    ep.title = ui->lineEdit_Title->text();
    ep.origin = ui->lineEdit_TitleOrigin->text();
    ep.tag1 = ui->checkBox_Tag1->isChecked();
    ep.tag2 = ui->checkBox_Tag2->isChecked();
    ep.tag3 = ui->checkBox_Tag3->isChecked();
    for(int i = 0; i < mEpisodes.size(); i++)
    {
        AnimeEpisodeData ep_temp = ep;
        ep_temp.episode = mEpisodes.at(i);
        eps.append(ep_temp);
    }
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = true;
    gIPD.index_anime.e_click = false;
    QVariant var_send;
    var_send.setValue(eps);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_ANIME_EPISODE, var_send);
    this->accept();
}

///取消
void AnimeEpisodeNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}

///单话/多话切换
void AnimeEpisodeNewDialog::on_radioButton_OneEpisode_toggled(bool checked)
{
    ui->label_Episode->setEnabled(checked);
    ui->lineEdit_Episode->setEnabled(checked);

    ui->label_EpisodeCount->setEnabled(!checked);
    ui->lineEdit_EpisodeCount->setEnabled(!checked);
    ui->label_EpisodeFormat->setEnabled(!checked);
    ui->lineEdit_EpisodeFormat->setEnabled(!checked);
    ui->label_EpisodeBegin->setEnabled(!checked);
    ui->lineEdit_EpisodeBegin->setEnabled(!checked);
    ui->label_EpisodeFillZeroNum->setEnabled(!checked);
    ui->comboBox_EpisodeFillZeroNum->setEnabled(!checked);
    updateEpisodePreview();
}

///话数编辑完成
void AnimeEpisodeNewDialog::on_lineEdit_EpisodeCount_editingFinished()
{
    int ep_count = ui->lineEdit_EpisodeCount->text().toInt();
    if(ep_count == mEpCount)
    {
        return;
    }
    if(ep_count <= 1 || ep_count > 100)
    {
        QMessageBox::information(this, tr("集数错误"), tr("范围: 2~100"), QMessageBox::Ok, QMessageBox::Ok);
        ui->lineEdit_EpisodeCount->setText(QString::number(mEpCount));
    }
    else
    {
        mEpCount = ep_count;
        ui->lineEdit_EpisodeCount->setText(QString::number(mEpCount));
        updateEpisodePreview();
    }
}

///更新话序号预览
void AnimeEpisodeNewDialog::updateEpisodePreview()
{
    if(ui->radioButton_OneEpisode->isChecked())
    {   //单话
        mEpisodes.clear();
        mEpisodes.append(ui->lineEdit_Episode->text());
        ui->label_EpisodePreview->setText(mEpisodes.first());
    }
    else
    {   //多话
        mEpisodes.clear();
        for(int i = 0; i < mEpCount; i++)
        {
            QString ep = mEpFormat;
            mEpisodes.append(ep.replace("[%ep%]", QString("%1").arg(mEpBegin+i, mEpFillZeroNum, 10, QLatin1Char('0'))));
        }
        QString ep_preview;
        if(mEpCount == 2)
        {
            ep_preview = QString("%1, %2").arg(mEpisodes.at(0), mEpisodes.at(1));
        }
        else if(mEpCount == 3)
        {
            ep_preview = QString("%1, %2, %3").arg(mEpisodes.at(0), mEpisodes.at(1), mEpisodes.at(2));
        }
        else if(mEpCount == 4)
        {
            ep_preview = QString("%1, %2, %3, %4").arg(mEpisodes.at(0), mEpisodes.at(1), mEpisodes.at(2), mEpisodes.at(3));
        }
        else if(mEpCount >= 5)
        {
            ep_preview = QString("%1, %2, ..., %3, %4").arg(mEpisodes.at(0), mEpisodes.at(1), mEpisodes.at(mEpCount-2), mEpisodes.at(mEpCount-1));
        }
        ui->label_EpisodePreview->setText(ep_preview);
    }
}

///补零位数变化
void AnimeEpisodeNewDialog::on_comboBox_EpisodeFillZeroNum_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:     mEpFillZeroNum = 0;    break;
    case 1:     mEpFillZeroNum = 2;    break;
    case 2:     mEpFillZeroNum = 3;    break;
    case 3:     mEpFillZeroNum = 4;    break;
    default:
        break;
    }
}

///开始序号编辑完成
void AnimeEpisodeNewDialog::on_lineEdit_EpisodeBegin_editingFinished()
{
    int ep_begin = ui->lineEdit_EpisodeBegin->text().toInt();
    if(ep_begin == mEpBegin)
    {
        return;
    }
    if(ep_begin < 0)
    {
        ui->lineEdit_EpisodeBegin->setText(QString::number(mEpBegin));
        QMessageBox::information(this, tr("开始序号不能是负数"), tr("范围: 自然数"), QMessageBox::Ok, QMessageBox::Ok);
    }
    else
    {
        mEpBegin = ep_begin;
        ui->lineEdit_EpisodeBegin->setText(QString::number(mEpBegin));
        updateEpisodePreview();
    }
}

///序号格式编辑完成
void AnimeEpisodeNewDialog::on_lineEdit_EpisodeFormat_editingFinished()
{
    QString format = ui->lineEdit_EpisodeFormat->text();
    if(format == mEpFormat)
    {
        return;
    }
    if(!format.contains("[%ep%]"))
    {
        ui->lineEdit_EpisodeFormat->setText(mEpFormat);
        QMessageBox::information(this, tr("格式有误"), tr("请包含序号数字部分\n数字部分请用 [%ep%] 代替"), QMessageBox::Ok, QMessageBox::Ok);
    }
    else
    {
        mEpFormat = format;
        ui->lineEdit_EpisodeFormat->setText(mEpFormat);
        updateEpisodePreview();
    }
}

///补零位数改变
void AnimeEpisodeNewDialog::on_comboBox_EpisodeFillZeroNum_activated(int index)
{
    Q_UNUSED(index)
    updateEpisodePreview();
}

///序号编辑完成
void AnimeEpisodeNewDialog::on_lineEdit_Episode_editingFinished()
{
    ui->label_EpisodePreview->setText(ui->lineEdit_Episode->text());
    updateEpisodePreview();
}

