QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SqlThread/SqlThread.cpp \
    UiThread/Anime/AnimeAnimeNewDialog.cpp \
    UiThread/Anime/AnimeAnimeWidgetItem.cpp \
    UiThread/Anime/AnimeEpisodeNewDialog.cpp \
    UiThread/Anime/AnimeEpisodeWidgetItem.cpp \
    UiThread/Anime/AnimeSeasonNewDialog.cpp \
    UiThread/Anime/AnimeSeasonWidgetItem.cpp \
    UiThread/InterfacePublicData.cpp \
    main.cpp \
    Widget.cpp

HEADERS += \
    PublicData.h \
    SqlThread/SqlDataDefine.h \
    SqlThread/SqlThread.h \
    UiThread/Anime/AnimeAnimeNewDialog.h \
    UiThread/Anime/AnimeAnimeWidgetItem.h \
    UiThread/Anime/AnimeEpisodeNewDialog.h \
    UiThread/Anime/AnimeEpisodeWidgetItem.h \
    UiThread/Anime/AnimeSeasonNewDialog.h \
    UiThread/Anime/AnimeSeasonWidgetItem.h \
    UiThread/InterfacePublicData.h \
    Widget.h

FORMS += \
    UiThread/Anime/AnimeAnimeNewDialog.ui \
    UiThread/Anime/AnimeAnimeWidgetItem.ui \
    UiThread/Anime/AnimeEpisodeNewDialog.ui \
    UiThread/Anime/AnimeEpisodeWidgetItem.ui \
    UiThread/Anime/AnimeSeasonNewDialog.ui \
    UiThread/Anime/AnimeSeasonWidgetItem.ui \
    Widget.ui

TRANSLATIONS += \
    Language/zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

RC_FILE = cherryme_resource.rc
