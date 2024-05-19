#ifndef ANIMEANIMENEWDIALOG_H
#define ANIMEANIMENEWDIALOG_H

#include <QDialog>

namespace Ui {
class AnimeAnimeNewDialog;
}

class AnimeAnimeNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnimeAnimeNewDialog(QWidget *parent = nullptr);
    ~AnimeAnimeNewDialog();

private:
    Ui::AnimeAnimeNewDialog *ui;

public:
    void Hi();  //显示

private slots:
    void on_pushButton_Ok_clicked();        //确认
    void on_pushButton_Cancel_clicked();    //取消
};

#endif // ANIMEANIMENEWDIALOG_H
