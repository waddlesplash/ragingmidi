#-------------------------------------------------
#
# Project created by QtCreator 2012-09-17T09:21:32
#
#-------------------------------------------------

CONFIG += qt
QT  += core gui opengl

TEMPLATE = app
win32 { TARGET = RagingMidi }
!win32 { TARGET = ragingmidi }

include(QtMidi/QtMidi.pri)

SOURCES += Gui/MainWind.cpp \
    Main.cpp \
    Gui/Selectors/SelectInstrument.cpp \
    Gui/Widgets/VirtualPiano.cpp \
    Player.cpp \
    Gui/Selectors/SelectOutput.cpp \
    Gui/PianoRoll.cpp \
    Gui/AllEvents.cpp \
    Gui/Widgets/SeekSlider.cpp \
    Gui/AboutDlg.cpp

HEADERS  += Gui/MainWind.h \
    Gui/Selectors/SelectInstrument.h \
    Gui/Widgets/VirtualPiano.h \
    Player.h \
    Gui/Selectors/SelectOutput.h \
    Gui/PianoRoll.h \
    Gui/AllEvents.h \
    Gui/Widgets/SeekSlider.h \
    Gui/AboutDlg.h \
    config.h

FORMS    += Gui/MainWind.ui \
    Gui/Selectors/SelectInstrument.ui \
    Gui/Widgets/VirtualPiano.ui \
    Gui/Selectors/SelectOutput.ui \
    Gui/PianoRoll.ui \
    Gui/AllEvents.ui \
    Gui/Widgets/SeekSlider.ui \
    Gui/AboutDlg.ui

RESOURCES += Resources/Resources.qrc

# Create the "REVISION" file
exists(../.git/HEAD) {
    system(git rev-parse HEAD >Resources/REVISION.txt)
}
