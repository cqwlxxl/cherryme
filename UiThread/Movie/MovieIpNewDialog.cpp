#include "MovieIpNewDialog.h"
#include "ui_MovieIpNewDialog.h"

MovieIpNewDialog::MovieIpNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieIpNewDialog)
{
    ui->setupUi(this);
}

MovieIpNewDialog::~MovieIpNewDialog()
{
    delete ui;
}
