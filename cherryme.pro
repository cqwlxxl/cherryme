QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SqlThread/SqlThread.cpp \
    UiThread/AnimeAnimeNewDialog.cpp \
    UiThread/AnimeAnimeWidgetItem.cpp \
    UiThread/AnimeEpisodeNewDialog.cpp \
    UiThread/AnimeEpisodeWidgetItem.cpp \
    UiThread/AnimeSeasonNewDialog.cpp \
    UiThread/AnimeSeasonWidgetItem.cpp \
    UiThread/InterfacePublicData.cpp \
    main.cpp \
    Widget.cpp

HEADERS += \
    PublicData.h \
    SqlThread/SqlDataDefine.h \
    SqlThread/SqlThread.h \
    UiThread/AnimeAnimeNewDialog.h \
    UiThread/AnimeAnimeWidgetItem.h \
    UiThread/AnimeEpisodeNewDialog.h \
    UiThread/AnimeEpisodeWidgetItem.h \
    UiThread/AnimeSeasonNewDialog.h \
    UiThread/AnimeSeasonWidgetItem.h \
    UiThread/InterfacePublicData.h \
    Widget.h

FORMS += \
    UiThread/AnimeAnimeNewDialog.ui \
    UiThread/AnimeAnimeWidgetItem.ui \
    UiThread/AnimeEpisodeNewDialog.ui \
    UiThread/AnimeEpisodeWidgetItem.ui \
    UiThread/AnimeSeasonNewDialog.ui \
    UiThread/AnimeSeasonWidgetItem.ui \
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
