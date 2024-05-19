#include "TvIpNewDialog.h"
#include "ui_TvIpNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"
#include "UiThread/InterfacePublicData.h"

Q_DECLARE_METATYPE(TvIpData)

TvIpNewDialog::TvIpNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TvIpNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

TvIpNewDialog::~TvIpNewDialog()
{
    delete ui;
}

///显示
void TvIpNewDialog::Hi()
{
    ui->lineEdit_Name->clear();
    ui->lineEdit_Keywords->clear();
    ui->checkBox_Zhuiju->setChecked(false);
    ui->checkBox_Display->setChecked(false);
    this->exec();
}

///确认
void TvIpNewDialog::on_pushButton_Ok_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed();
    QString keywords = ui->lineEdit_Keywords->text().trimmed();
    ui->lineEdit_Name->setText(name);
    ui->lineEdit_Keywords->setText(keywords);
    if(name.isEmpty())
    {
        QMessageBox::information(this, tr("名称未填"), tr("请填写电视剧名称"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    TvIpData ip;
    ip.name = name;
    ip.keywords = keywords;
    ip.zhuiju = ui->checkBox_Zhuiju->isChecked();
    ip.display = ui->checkBox_Display->isChecked();

    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_TV_IP, var_send);
    this->accept();
}

///取消
void TvIpNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}
