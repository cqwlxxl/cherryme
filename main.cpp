#include "Widget.h"

#include <QApplication>
#include <QThread>
#include <QDir>
#include <QTranslator>
#include <QTextCodec>

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
    QDir::setCurrent(QCoreApplication::applicationDirPath());   //设置工作目录为程序所在目录

    //向Qt注册自定义的数据格式，用于信号和槽
    qRegisterMetaType<SqlOperateType>("SqlOperateType");

    //初始化ini配置
    QSettings iniFile(CONFIG_FILE_PATH, QSettings::IniFormat);
    iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));     //设置文本编码
    QString strLang = iniFile.value("ui_setting/lang", "zh_CN").toString();
    gPUD.lang = strLang;

    //加载翻译文件
    QTranslator qtTranslatorQt;
    if(qtTranslatorQt.load("./src/language/qt_" + strLang + ".qm"))
    {   //Qt原生翻译
        a.installTranslator(&qtTranslatorQt);
    }
    QTranslator qtTranslator;
    if(qtTranslator.load("./src/language/" + strLang + ".qm"))
    {   //用户翻译
        a.installTranslator(&qtTranslator);
    }
    if(strLang == "zh_CN")
    {
        gPUD.locale = QLocale::Chinese;
    }

    //数据库线程
    SqlThread *sql = new SqlThread(nullptr);
    QThread *threadSql = new QThread(nullptr);
    sql->moveToThread(threadSql);
    QObject::connect(&gIPD, &InterfacePublicData::SIGNALSendQuery, sql, &SqlThread::SLOTReceiveQuery, Qt::UniqueConnection);
    QObject::connect(sql, &SqlThread::SIGNALSendQueryData, &gIPD, &InterfacePublicData::SIGNALReceiveQueryData, Qt::UniqueConnection);
    //开启线程
    threadSql->start();

    Widget w;
    w.show();
    return a.exec();
}
