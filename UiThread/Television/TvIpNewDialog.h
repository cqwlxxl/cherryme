#ifndef TVIPNEWDIALOG_H
#define TVIPNEWDIALOG_H

#include <QDialog>

namespace Ui {
class TvIpNewDialog;
}

class TvIpNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TvIpNewDialog(QWidget *parent = nullptr);
    ~TvIpNewDialog();

private:
    Ui::TvIpNewDialog *ui;

public:
    void Hi();  //显示

private slots:
    void on_pushButton_Ok_clicked();        //确认
    void on_pushButton_Cancel_clicked();    //取消
};

#endif // TVIPNEWDIALOG_H
