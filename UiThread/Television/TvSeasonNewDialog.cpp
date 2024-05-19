#include "TvSeasonNewDialog.h"
#include "ui_TvSeasonNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"
#include "UiThread/InterfacePublicData.h"

Q_DECLARE_METATYPE(TvSeasonData)

TvSeasonNewDialog::TvSeasonNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TvSeasonNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

TvSeasonNewDialog::~TvSeasonNewDialog()
{
    delete ui;
}

///显示
void TvSeasonNewDialog::Hi(int pid)
{
    mPid = pid;
    ui->lineEdit_Name->clear();
    ui->checkBox_Display->setChecked(false);
    ui->checkBox_ReleaseDateEnable->setChecked(false);
    ui->dateEdit_ReleaseDate->setDate(QDate::currentDate());
    ui->dateEdit_ReleaseDate->setVisible(false);
    ui->comboBox_Point->setCurrentIndex(0);
    ui->checkBox_CollectIt->setChecked(false);
    ui->checkBox_CollectOk->setChecked(false);
    ui->checkBox_CollectOk->setEnabled(false);
    this->exec();
}

///确认
void TvSeasonNewDialog::on_pushButton_Ok_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed();
    ui->lineEdit_Name->setText(name);
    if(name.isEmpty())
    {
        QMessageBox::information(this, tr("名称未填"), tr("请填写部名称"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    TvSeasonData season;
    season.pid = mPid;
    season.name = name;
    season.display = ui->checkBox_Display->isChecked();
    season.release_date_valid = ui->checkBox_ReleaseDateEnable->isChecked();
    season.release_date = ui->dateEdit_ReleaseDate->date();
    season.point = ui->comboBox_Point->currentIndex();
    if(!ui->checkBox_CollectIt->isChecked())
    {
        season.collect = 0;
    }
    else if(!ui->checkBox_CollectOk->isChecked())
    {
        season.collect = 1;
    }
    else
    {
        season.collect = 2;
    }
    //
    gIPD.index_tv.p_click = true;
    gIPD.index_tv.s_click = false;
    gIPD.index_tv.e_click = false;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_TV_SEASON, var_send);
    this->accept();
}

///取消
void TvSeasonNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}

///发布日期有效
void TvSeasonNewDialog::on_checkBox_ReleaseDateEnable_clicked(bool checked)
{
    ui->dateEdit_ReleaseDate->setVisible(checked);
}

///要收藏
void TvSeasonNewDialog::on_checkBox_CollectIt_clicked(bool checked)
{
    if(checked)
    {
        ui->checkBox_CollectOk->setEnabled(true);
    }
    else
    {
        ui->checkBox_CollectOk->setChecked(false);
        ui->checkBox_CollectOk->setEnabled(false);
    }
}

