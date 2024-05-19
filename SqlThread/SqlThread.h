#ifndef SQLTHREAD_H
#define SQLTHREAD_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include "SqlDataDefine.h"

class SqlThread : public QObject
{
    Q_OBJECT
public:
    explicit SqlThread(QObject *parent = nullptr);
    ~SqlThread();

signals:
    void SIGNALSendQueryData(SqlOperateType operate, QVariant var);

public slots:
    void SLOTReceiveQuery(SqlOperateType operate, QVariant var);     //解析数据语句

private:
    bool login();   //登录
    bool logout();  //注销
    void calcAnimeSee(int aid, int sid);    //计算进度
    void calcAnimeSee(int aid);             //计算进度，季变化
    void calcAnimePoint(int aid);           //计算评分
    void calcAnimeCollect(int aid);         //计算收藏
    void calcAnimeTag1(int aid, int sid);   //计算tag1
    void calcAnimeTag2(int aid, int sid);   //计算tag2
    void calcAnimeTag3(int aid, int sid);   //计算tag3
    void calcAnimeRecent(int aid, int sid); //更新最近观看

private:
    QSqlDatabase        mDB;    //数据库
    QSqlQuery           mQuery; //数据库查询

};

#endif // SQLTHREAD_H
