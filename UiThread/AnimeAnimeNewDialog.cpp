#include "AnimeAnimeNewDialog.h"
#include "UiThread/InterfacePublicData.h"
#include "ui_AnimeAnimeNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"

Q_DECLARE_METATYPE(AnimeData)

AnimeAnimeNewDialog::AnimeAnimeNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimeAnimeNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

AnimeAnimeNewDialog::~AnimeAnimeNewDialog()
{
    delete ui;
}

///显示
void AnimeAnimeNewDialog::Hi()
{
    ui->lineEdit_Name->clear();
    ui->lineEdit_Keywords->clear();
    ui->checkBox_Zhuifan->setChecked(false);
    this->exec();
}

///确认
void AnimeAnimeNewDialog::on_pushButton_Ok_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed().replace("'", "''");
    QString keywords = ui->lineEdit_Keywords->text().trimmed().replace("'", "''");
    ui->lineEdit_Name->setText(name);
    ui->lineEdit_Keywords->setText(keywords);
    if(name.isEmpty())
    {
        QMessageBox::information(this, tr("名称未填"), tr("请填写动漫名称"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    AnimeData anime;
    anime.name = name;
    anime.keywords = keywords;
    anime.zhuifan = ui->checkBox_Zhuifan->isChecked();
    //
    QVariant var_send;
    var_send.setValue(anime);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_ANIME_ANIME, var_send);
    this->accept();
}

///取消
void AnimeAnimeNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}
