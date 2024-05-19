#include "TvEpisodeNewDialog.h"
#include "ui_TvEpisodeNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"
#include "UiThread/InterfacePublicData.h"

Q_DECLARE_METATYPE(TvEpisodeData)

TvEpisodeNewDialog::TvEpisodeNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TvEpisodeNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

TvEpisodeNewDialog::~TvEpisodeNewDialog()
{
    delete ui;
}

///显示
void TvEpisodeNewDialog::Hi(int pid, int sid)
{
    mPid = pid;
    mSid = sid;
    updateEpisodePreview();
    this->exec();
}

///新增集
void TvEpisodeNewDialog::on_pushButton_Ok_clicked()
{
    if(mEpisodes.size() == 0)
    {
        return;
    }

    QList<TvEpisodeData> eps;
    TvEpisodeData ep;
    ep.pid = mPid;
    ep.sid = mSid;
    ep.title = ui->lineEdit_Title->text();
    ep.tag1 = ui->checkBox_Tag1->isChecked();
    ep.tag2 = ui->checkBox_Tag2->isChecked();
    ep.tag3 = ui->checkBox_Tag3->isChecked();
    for(int i = 0; i < mEpisodes.size(); i++)
    {
        TvEpisodeData ep_temp = ep;
        ep_temp.episode = mEpisodes.at(i);
        if(ui->checkBox_UseCnTitles->isChecked() && mTitles.size() > i)
        {
            ep_temp.title = mTitles.at(i);
        }
        eps.append(ep_temp);
    }
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = true;
    gIPD.index_tv.e_click = false;
    QVariant var_send;
    var_send.setValue(eps);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_TV_EPISODE, var_send);
    this->accept();
}

///取消
void TvEpisodeNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}

///单集/多集切换
void TvEpisodeNewDialog::on_radioButton_OneEpisode_toggled(bool checked)
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

///集数编辑完成
void TvEpisodeNewDialog::on_lineEdit_EpisodeCount_editingFinished()
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

///更新集序号预览
void TvEpisodeNewDialog::updateEpisodePreview()
{
    mEpisodes.clear();
    mTitles.clear();
    if(ui->radioButton_OneEpisode->isChecked())
    {   //单集
        mEpisodes.append(ui->lineEdit_Episode->text());
        if(ui->checkBox_UseCnTitles->isChecked())
        {
            mTitles.append(getCnTitles(ui->lineEdit_Episode->text().toInt()));
        }
        ui->label_EpisodePreview->setText(mEpisodes.first());
    }
    else
    {   //多集
        for(int i = 0; i < mEpCount; i++)
        {
            QString ep = mEpFormat;
            mEpisodes.append(ep.replace("[%ep%]", QString("%1").arg(mEpBegin+i, mEpFillZeroNum, 10, QLatin1Char('0'))));
            if(ui->checkBox_UseCnTitles->isChecked())
            {
                mTitles.append(getCnTitles(mEpBegin+i));
            }
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

///获取中文集数标题
QString TvEpisodeNewDialog::getCnTitles(int index)
{
    QString str = "";
    int a = index / 10;
    int b = index % 10;
    QString first;
    QString second;
    if(index >= 1 && index <= 99)
    {
        switch(a)
        {
        case 0:     first = "";         break;
        case 1:     first = "十";       break;
        case 2:     first = "二十";     break;
        case 3:     first = "三十";     break;
        case 4:     first = "四十";     break;
        case 5:     first = "五十";     break;
        case 6:     first = "六十";     break;
        case 7:     first = "七十";     break;
        case 8:     first = "八十";     break;
        case 9:     first = "九十";     break;
        default:                        break;
        }
        switch(b)
        {
        case 0:     second = "";        break;
        case 1:     second = "一";      break;
        case 2:     second = "二";      break;
        case 3:     second = "三";      break;
        case 4:     second = "四";      break;
        case 5:     second = "五";      break;
        case 6:     second = "六";      break;
        case 7:     second = "七";      break;
        case 8:     second = "八";      break;
        case 9:     second = "九";      break;
        default:                        break;
        }
        str = QString("第%1%2集").arg(first, second);
    }
    return str;
}

///补零位数变化
void TvEpisodeNewDialog::on_comboBox_EpisodeFillZeroNum_currentIndexChanged(int index)
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
void TvEpisodeNewDialog::on_lineEdit_EpisodeBegin_editingFinished()
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
void TvEpisodeNewDialog::on_lineEdit_EpisodeFormat_editingFinished()
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
void TvEpisodeNewDialog::on_comboBox_EpisodeFillZeroNum_activated(int index)
{
    Q_UNUSED(index)
    updateEpisodePreview();
}

///序号编辑完成
void TvEpisodeNewDialog::on_lineEdit_Episode_editingFinished()
{
    ui->label_EpisodePreview->setText(ui->lineEdit_Episode->text());
    updateEpisodePreview();
}

