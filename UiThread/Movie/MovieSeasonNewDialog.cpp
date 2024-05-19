#include "MovieSeasonNewDialog.h"
#include "ui_MovieSeasonNewDialog.h"

MovieSeasonNewDialog::MovieSeasonNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieSeasonNewDialog)
{
    ui->setupUi(this);
}

MovieSeasonNewDialog::~MovieSeasonNewDialog()
{
    delete ui;
}
