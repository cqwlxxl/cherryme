#include "Widget.h"

#include <QApplication>
#include <QThread>

#include "UiThread/InterfacePublicData.h"
#include "SqlThread/SqlThread.h"

InterfacePublicData gIPD;   //界面公共数据

int main(int argc, char *argv[])
{
    //控制图片缩放质量
    if(QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    {
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    }

    QApplication a(argc, argv);
    //////
    //向Qt注册自定义的数据格式，用于信号和槽
    qRegisterMetaType<SqlOperateType>("SqlOperateType");

    //数据库线程
    SqlThread *sql = new SqlThread(nullptr);
    QThread *threadSql = new QThread(nullptr);
    sql->moveToThread(threadSql);
    QObject::connect(&gIPD, &InterfacePublicData::SIGNALSendQuery, sql, &SqlThread::SLOTReceiveQuery, Qt::UniqueConnection);
    QObject::connect(sql, &SqlThread::SIGNALSendQueryData, &gIPD, &InterfacePublicData::SIGNALReceiveQueryData, Qt::UniqueConnection);
    //开启线程
    threadSql->start();
    //////
    Widget w;
    w.show();
    return a.exec();
}
