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
};

#endif // MOVIEIPNEWDIALOG_H
