QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SqlThread/SqlThread.cpp \
    UiThread/Anime/AnimeEpisodeNewDialog.cpp \
    UiThread/Anime/AnimeEpisodeWidgetItem.cpp \
    UiThread/Anime/AnimeIpNewDialog.cpp \
    UiThread/Anime/AnimeIpWidgetItem.cpp \
    UiThread/Anime/AnimeSeasonNewDialog.cpp \
    UiThread/Anime/AnimeSeasonWidgetItem.cpp \
    UiThread/InterfacePublicData.cpp \
    UiThread/Movie/MovieIpNewDialog.cpp \
    UiThread/Movie/MovieIpWidgetItem.cpp \
    UiThread/Movie/MovieSeasonNewDialog.cpp \
    UiThread/Movie/MovieSeasonWidgetItem.cpp \
    UiThread/Television/TvEpisodeNewDialog.cpp \
    UiThread/Television/TvEpisodeWidgetItem.cpp \
    UiThread/Television/TvIpNewDialog.cpp \
    UiThread/Television/TvIpWidgetItem.cpp \
    UiThread/Television/TvSeasonNewDialog.cpp \
    UiThread/Television/TvSeasonWidgetItem.cpp \
    main.cpp \
    Widget.cpp

HEADERS += \
    PublicData.h \
    SqlThread/SqlDataDefine.h \
    SqlThread/SqlThread.h \
    UiThread/Anime/AnimeEpisodeNewDialog.h \
    UiThread/Anime/AnimeEpisodeWidgetItem.h \
    UiThread/Anime/AnimeIpNewDialog.h \
    UiThread/Anime/AnimeIpWidgetItem.h \
    UiThread/Anime/AnimeSeasonNewDialog.h \
    UiThread/Anime/AnimeSeasonWidgetItem.h \
    UiThread/InterfacePublicData.h \
    UiThread/Movie/MovieIpNewDialog.h \
    UiThread/Movie/MovieIpWidgetItem.h \
    UiThread/Movie/MovieSeasonNewDialog.h \
    UiThread/Movie/MovieSeasonWidgetItem.h \
    UiThread/Television/TvEpisodeNewDialog.h \
    UiThread/Television/TvEpisodeWidgetItem.h \
    UiThread/Television/TvIpNewDialog.h \
    UiThread/Television/TvIpWidgetItem.h \
    UiThread/Television/TvSeasonNewDialog.h \
    UiThread/Television/TvSeasonWidgetItem.h \
    Widget.h

FORMS += \
    UiThread/Anime/AnimeEpisodeNewDialog.ui \
    UiThread/Anime/AnimeEpisodeWidgetItem.ui \
    UiThread/Anime/AnimeIpNewDialog.ui \
    UiThread/Anime/AnimeIpWidgetItem.ui \
    UiThread/Anime/AnimeSeasonNewDialog.ui \
    UiThread/Anime/AnimeSeasonWidgetItem.ui \
    UiThread/Movie/MovieIpNewDialog.ui \
    UiThread/Movie/MovieIpWidgetItem.ui \
    UiThread/Movie/MovieSeasonNewDialog.ui \
    UiThread/Movie/MovieSeasonWidgetItem.ui \
    UiThread/Television/TvEpisodeNewDialog.ui \
    UiThread/Television/TvEpisodeWidgetItem.ui \
    UiThread/Television/TvIpNewDialog.ui \
    UiThread/Television/TvIpWidgetItem.ui \
    UiThread/Television/TvSeasonNewDialog.ui \
    UiThread/Television/TvSeasonWidgetItem.ui \
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
