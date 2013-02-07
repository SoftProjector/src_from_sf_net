##**************************************************************************
##
##    softProjector - an open source media projection software
##    Copyleft (Ɔ) 2013  Vladislav Kobzar, Matvey Adzhigirey and Ilya Spivakov
##
##    This program is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation version 3 of the License.
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program.  If not, see <http:##www.gnu.org/licenses/>.
##
##**************************************************************************


QT += sql \
    phonon \
    webkit
TARGET = softProjector
TEMPLATE = app
//CONFIG += x86 ppc x86_64 ppc64 # Compile a universal build

RES_DIR = $${PWD}/unknownsys_bild
win32: RES_DIR = $${PWD}/win32_bild
unix:  RES_DIR = $${PWD}/unix_bild
macx: RES_DIR = $${PWD}/mac_bild

DESTDIR = $${RES_DIR}/bin
OBJECTS_DIR = $${RES_DIR}/obj
MOC_DIR = $${RES_DIR}/moc
UI_DIR = $${RES_DIR}/ui
RCC_DIR = $${RES_DIR}/rcc
OUT_PWD = $${RES_DIR}/bin

SOURCES += main.cpp \
    softprojector.cpp \
    songwidget.cpp \
    biblewidget.cpp \
    editwidget.cpp \
    song.cpp \
    bible.cpp \
    settingsdialog.cpp \
    aboutdialog.cpp \
    addsongbookdialog.cpp \
    highlight.cpp \
    managedatadialog.cpp \
    managedata.cpp \
    announcewidget.cpp \
    helpdialog.cpp \
    songcounter.cpp \
    bibleinformationdialog.cpp \
    settings.cpp \
    generalsettingwidget.cpp \
    biblesettingwidget.cpp \
    songsettingwidget.cpp \
    announcementsettingwidget.cpp \
    printpreviewdialog.cpp \
    displayscreen.cpp \
    controlbutton.cpp \
    passivesettingwidget.cpp \
    theme.cpp \
    picturewidget.cpp \
    slideshow.cpp \
    mediawidget.cpp \
    videoplayerwidget.cpp \
    videoinfo.cpp \
    spfunctions.cpp \
    slideshoweditor.cpp
HEADERS += softprojector.h \
    songwidget.h \
    biblewidget.h \
    editwidget.h \
    song.h \
    bible.h \
    settingsdialog.h \
    aboutdialog.h \
    addsongbookdialog.h \
    highlight.h \
    managedatadialog.h \
    managedata.h \
    announcewidget.h \
    helpdialog.h \
    songcounter.h \
    bibleinformationdialog.h \
    settings.h \
    generalsettingwidget.h \
    biblesettingwidget.h \
    songsettingwidget.h \
    announcementsettingwidget.h \
    printpreviewdialog.h \
    displayscreen.h \
    controlbutton.h \
    passivesettingwidget.h \
    theme.h \
    picturewidget.h \
    slideshow.h \
    mediawidget.h \
    videoplayerwidget.h \
    videoinfo.h \
    spfunctions.h \
    slideshoweditor.h
FORMS += softprojector.ui \
    songwidget.ui \
    biblewidget.ui \
    editwidget.ui \
    settingsdialog.ui \
    aboutdialog.ui \
    addsongbookdialog.ui \
    managedatadialog.ui \
    announcewidget.ui \
    helpdialog.ui \
    songcounter.ui \
    bibleinformationdialog.ui \
    generalsettingwidget.ui \
    biblesettingwidget.ui \
    songsettingwidget.ui \
    announcementsettingwidget.ui \
    printpreviewdialog.ui \
    displayscreen.ui \
    passivesettingwidget.ui \
    picturewidget.ui \
    mediawidget.ui \
    slideshoweditor.ui
TRANSLATIONS += translations/softpro_de.ts\
    translations/softpro_ru.ts\
    translations/softpro_cs.ts\
    translations/softpro_ua.ts
CODECFORTR = UTF-8
RESOURCES += softprojector.qrc

win32 {
    RC_FILE = softprojector.rc
}































