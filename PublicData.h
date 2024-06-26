#ifndef PUBLICDATA_H
#define PUBLICDATA_H

#include <QLocale>
#include <QSettings>

#define SOFTWARE_VERSION    "0.0.0"         //软件版本
#define SOFTWARE_DATE       "0000.00.00"    //软件更新时间

#define CONFIG_FILE_PATH    "src/config/config.ini"             //配置文件路径

enum AnimeType
{
    AnimeDefault = 0,       //无
    AnimeMovie,             //剧场版
    AnimeTv,                //TV
    AnimeSep,               //特别篇
    AnimeCollection,        //总集篇
    AnimeOva,               //OVA
    AnimeOad,               //OAD
    AnimeSp,                //特典
    AnimeWeb                //WEB
};

struct PublicUseData
{
    QString     softwareName {"Cherry Me !!"};  //软件名称
    QLocale     locale {QLocale::Chinese};      //本地化语言
    QString     lang {"zh_CN"};                 //语言
};

extern PublicUseData    gPUD;

#endif // PUBLICDATA_H
