#include "AnimeSeasonNewDialog.h"
#include "UiThread/InterfacePublicData.h"
#include "ui_AnimeSeasonNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"

Q_DECLARE_METATYPE(AnimeSeasonData)

AnimeSeasonNewDialog::AnimeSeasonNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimeSeasonNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

AnimeSeasonNewDialog::~AnimeSeasonNewDialog()
{
    delete ui;
}

///显示
void AnimeSeasonNewDialog::Hi(int pid)
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

///新增动漫季
void AnimeSeasonNewDialog::on_pushButton_Ok_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed();
    ui->lineEdit_Name->setText(name);
    if(name.isEmpty())
    {
        QMessageBox::information(this, tr("名称未填"), tr("请填写季度名称"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    AnimeSeasonData season;
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
    gIPD.index_anime.p_click = true;
    gIPD.index_anime.s_click = false;
    gIPD.index_anime.e_click = false;
    QVariant var_send;
    var_send.setValue(season);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_ANIME_SEASON, var_send);
    this->accept();
}

///取消
void AnimeSeasonNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}

///发布日期有效
void AnimeSeasonNewDialog::on_checkBox_ReleaseDateEnable_clicked(bool checked)
{
    ui->dateEdit_ReleaseDate->setVisible(checked);
}

///要收藏
void AnimeSeasonNewDialog::on_checkBox_CollectIt_clicked(bool checked)
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

