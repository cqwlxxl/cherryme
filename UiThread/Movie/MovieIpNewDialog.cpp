#include "MovieIpNewDialog.h"
#include "ui_MovieIpNewDialog.h"

#include <QMessageBox>

#include "SqlThread/SqlDataDefine.h"
#include "UiThread/InterfacePublicData.h"

Q_DECLARE_METATYPE(MovieIpData)

MovieIpNewDialog::MovieIpNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieIpNewDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);   //去掉标题栏的【？】
}

MovieIpNewDialog::~MovieIpNewDialog()
{
    delete ui;
}

///显示
void MovieIpNewDialog::Hi()
{
    ui->lineEdit_Name->clear();
    ui->lineEdit_Keywords->clear();
    ui->checkBox_Display->setChecked(false);
    this->exec();
}

///确认
void MovieIpNewDialog::on_pushButton_Ok_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed();
    QString keywords = ui->lineEdit_Keywords->text().trimmed();
    ui->lineEdit_Name->setText(name);
    ui->lineEdit_Keywords->setText(keywords);
    if(name.isEmpty())
    {
        QMessageBox::information(this, tr("名称未填"), tr("请填写电影名称"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    MovieIpData ip;
    ip.name = name;
    ip.keywords = keywords;
    ip.display = ui->checkBox_Display->isChecked();

    QVariant var_send;
    var_send.setValue(ip);
    emit gIPD.SIGNALSendQuery(SOT_INSERT_MOVIE_IP, var_send);
    this->accept();
}

///取消
void MovieIpNewDialog::on_pushButton_Cancel_clicked()
{
    this->reject();
}

