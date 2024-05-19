#ifndef ANIMEEPISODENEWDIALOG_H
#define ANIMEEPISODENEWDIALOG_H

#include <QDialog>

namespace Ui {
class AnimeEpisodeNewDialog;
}

class AnimeEpisodeNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnimeEpisodeNewDialog(QWidget *parent = nullptr);
    ~AnimeEpisodeNewDialog();

private:
    Ui::AnimeEpisodeNewDialog *ui;

public:
    void Hi(int aid, int sid);  //显示

private slots:
    void on_pushButton_Ok_clicked();        //新增话
    void on_pushButton_Cancel_clicked();    //取消
    void on_radioButton_OneEpisode_toggled(bool checked);   //单话/多话切换
    void on_lineEdit_EpisodeCount_editingFinished();    //话数编辑完成
    void on_comboBox_EpisodeFillZeroNum_currentIndexChanged(int index); //补零位数变化
    void on_lineEdit_EpisodeBegin_editingFinished();    //开始序号编辑完成
    void on_lineEdit_EpisodeFormat_editingFinished();   //序号格式编辑完成
    void on_comboBox_EpisodeFillZeroNum_activated(int index);   //补零位数改变
    void on_lineEdit_Episode_editingFinished();         //序号编辑完成

private:
    void updateEpisodePreview();    //更新话序号预览

private:
    int     mAid {0};
    int     mSid {0};
    int     mEpCount {12};          //话数
    QString mEpFormat {"[%ep%]"};   //格式
    int     mEpBegin {1};           //开始序号
    int     mEpFillZeroNum {2};     //补零
    QStringList     mEpisodes;
};

#endif // ANIMEEPISODENEWDIALOG_H
