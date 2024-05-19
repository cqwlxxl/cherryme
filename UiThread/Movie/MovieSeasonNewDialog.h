#ifndef MOVIESEASONNEWDIALOG_H
#define MOVIESEASONNEWDIALOG_H

#include <QDialog>

namespace Ui {
class MovieSeasonNewDialog;
}

class MovieSeasonNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieSeasonNewDialog(QWidget *parent = nullptr);
    ~MovieSeasonNewDialog();

private:
    Ui::MovieSeasonNewDialog *ui;
};

#endif // MOVIESEASONNEWDIALOG_H
