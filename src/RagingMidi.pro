#-------------------------------------------------
#
# Project created by QtCreator 2012-09-17T09:21:32
#
#-------------------------------------------------

CONFIG += qt
QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
INCLUDEPATH += $$PWD
include(QMidi/QMidi.pri)

contains(QT_CONFIG, opengl) | contains(QT_CONFIG, opengles2) {
   QT += opengl
} else {
   DEFINES += QT_NO_OPENGL
}

TEMPLATE = app
win32 {
    TARGET = RagingMidi
    RC_FILE = Resources/winres.rc
}
!win32 { TARGET = ragingmidi }

SOURCES += Gui/MainWind.cpp \
    Main.cpp \
    Gui/Selectors/SelectInstrument.cpp \
    Gui/Widgets/VirtualPiano.cpp \
    Player.cpp \
    Gui/Selectors/SelectOutput.cpp \
    Gui/PianoRoll.cpp \
    Gui/Dialogs/AllEvents.cpp \
    Gui/Widgets/SeekSlider.cpp \
    Gui/Dialogs/AboutDlg.cpp \
    Gui/Dialogs/TransposeDlg.cpp \
    Gui/TracksEdit.cpp \
    Gui/Widgets/TimeEdit.cpp \
    Gui/Widgets/TimeLabel.cpp \
    Settings.cpp \
    globals.cpp \
    Gui/Dialogs/PrefsDlg.cpp

HEADERS  += Gui/MainWind.h \
    Gui/Selectors/SelectInstrument.h \
    Gui/Widgets/VirtualPiano.h \
    Player.h \
    Gui/Selectors/SelectOutput.h \
    Gui/PianoRoll.h \
    Gui/Dialogs/AllEvents.h \
    Gui/Widgets/SeekSlider.h \
    Gui/Dialogs/AboutDlg.h \
    Gui/Dialogs/TransposeDlg.h \
    Gui/TracksEdit.h \
    Gui/Widgets/TimeEdit.h \
    Gui/Widgets/TimeLabel.h \
    Settings.h \
	globals.h \
    Gui/Dialogs/PrefsDlg.h

FORMS    += Gui/MainWind.ui \
    Gui/Selectors/SelectInstrument.ui \
    Gui/Selectors/SelectOutput.ui \
    Gui/Dialogs/AllEvents.ui \
    Gui/Widgets/SeekSlider.ui \
    Gui/Dialogs/AboutDlg.ui \
    Gui/Dialogs/TransposeDlg.ui \
    Gui/TracksEdit.ui \
    Gui/Widgets/TimeEdit.ui \
    Gui/Widgets/TimeLabel.ui \
    Gui/Dialogs/PrefsDlg.ui

RESOURCES += Resources/Resources.qrc

# Create the "REVISION" file
exists(../.git/HEAD) {
    system(git rev-parse HEAD >Resources/REVISION.txt)
} !exists(Resources/REVISION.txt) {
    system(echo ???????????????????????????????????????? >Resources/REVISION.txt)
}
