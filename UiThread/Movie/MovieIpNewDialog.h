#ifndef MOVIEIPNEWDIALOG_H
#define MOVIEIPNEWDIALOG_H

#include <QDialog>

namespace Ui {
class MovieIpNewDialog;
}

class MovieIpNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieIpNewDialog(QWidget *parent = nullptr);
    ~MovieIpNewDialog();

private:
    Ui::MovieIpNewDialog *ui;

public:
    void Hi();  //显示

private slots:
    void on_pushButton_Ok_clicked();        //确认
    void on_pushButton_Cancel_clicked();    //取消
};

#endif // MOVIEIPNEWDIALOG_H
