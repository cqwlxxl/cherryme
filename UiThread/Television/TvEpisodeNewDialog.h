#ifndef TVEPISODENEWDIALOG_H
#define TVEPISODENEWDIALOG_H

#include <QDialog>

namespace Ui {
class TvEpisodeNewDialog;
}

class TvEpisodeNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TvEpisodeNewDialog(QWidget *parent = nullptr);
    ~TvEpisodeNewDialog();

private:
    Ui::TvEpisodeNewDialog *ui;

public:
    void Hi(int pid, int sid);  //显示

private slots:
    void on_pushButton_Ok_clicked();        //新增集
    void on_pushButton_Cancel_clicked();    //取消
    void on_radioButton_OneEpisode_toggled(bool checked);   //单集/多集切换
    void on_lineEdit_EpisodeCount_editingFinished();    //集数编辑完成
    void on_comboBox_EpisodeFillZeroNum_currentIndexChanged(int index); //补零位数变化
    void on_lineEdit_EpisodeBegin_editingFinished();    //开始序号编辑完成
    void on_lineEdit_EpisodeFormat_editingFinished();   //序号格式编辑完成
    void on_comboBox_EpisodeFillZeroNum_activated(int index);   //补零位数改变
    void on_lineEdit_Episode_editingFinished();         //序号编辑完成

private:
    void updateEpisodePreview();    //更新集序号预览

private:
    int     mPid {0};
    int     mSid {0};
    int     mEpCount {12};          //集数
    QString mEpFormat {"[%ep%]"};   //格式
    int     mEpBegin {1};           //开始序号
    int     mEpFillZeroNum {2};     //补零
    QStringList     mEpisodes;
};

#endif // TVEPISODENEWDIALOG_H
