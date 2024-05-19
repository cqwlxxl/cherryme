#ifndef ANIMEIPNEWDIALOG_H
#define ANIMEIPNEWDIALOG_H

#include <QDialog>

namespace Ui {
class AnimeIpNewDialog;
}

class AnimeIpNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnimeIpNewDialog(QWidget *parent = nullptr);
    ~AnimeIpNewDialog();

private:
    Ui::AnimeIpNewDialog *ui;

public:
    void Hi();  //显示

private slots:
    void on_pushButton_Ok_clicked();        //确认
    void on_pushButton_Cancel_clicked();    //取消
};

#endif // ANIMEIPNEWDIALOG_H
