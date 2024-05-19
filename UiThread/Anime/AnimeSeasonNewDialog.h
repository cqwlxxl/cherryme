#ifndef ANIMESEASONNEWDIALOG_H
#define ANIMESEASONNEWDIALOG_H

#include <QDialog>

namespace Ui {
class AnimeSeasonNewDialog;
}

class AnimeSeasonNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnimeSeasonNewDialog(QWidget *parent = nullptr);
    ~AnimeSeasonNewDialog();

private:
    Ui::AnimeSeasonNewDialog *ui;

public:
    void Hi(int pid);  //显示

private slots:
    void on_pushButton_Ok_clicked();        //确认
    void on_pushButton_Cancel_clicked();    //取消
    void on_checkBox_ReleaseDateEnable_clicked(bool checked);   //发布日期有效
    void on_checkBox_CollectIt_clicked(bool checked);           //要收藏

private:
    int     mPid {0};
};

#endif // ANIMESEASONNEWDIALOG_H
