/*
 * Raging MIDI (https://github.com/waddlesplash/ragingmidi).
 *
 * Copyright (c) 2012 WaddleSplash & contributors (see AUTHORS.txt).
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef MAINWIND_H
#define MAINWIND_H

/****h* RagingMidi/MainWind.h
 * PURPOSE
 *   Main window of Raging MIDI. Responsible
 *   for the layout and communication of the
 *   PianoRoll, VirtualPiano, and TracksEdit.
 ******
 */

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QMap>
#include <QSettings>

#include <QtMidiFile.h>
#include "../Player.h"
#include "Widgets/SeekSlider.h"

namespace Ui {
class MainWind;
}

/****c* MainWind.h/MainWind
 * SYNOPSIS
 */
class MainWind : public QMainWindow
/**
 * DESCRIPTION
 *   The QMainWindow subclass that does everything.
 ******
 */
{
    Q_OBJECT
    
public:
    explicit MainWind(QWidget *parent = 0);
    ~MainWind();

    static QMap<int,QColor>* trackColors;
    static QMap<int,bool>* trackStatus;
    static SeekSlider* playLocSilder;
    
private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();

    void on_actionTranspose_triggered();

    void on_actionTrackAdd_triggered();
    void on_actionTrackRemove_triggered();

    void on_actionPlay_triggered();
    void on_actionStop_triggered();
    void on_actionRewind_triggered();
    void on_songPosSlider_sliderReleased();

    void on_actionViewAllEvents_triggered();

    void on_actionAbout_Raging_Midi_triggered();

private:
    Ui::MainWind *ui;
    QtMidiFile* midiFile;
    Player* player;
    QSettings* appSettings;

    void openMidiFile(QString filename);
};

#endif // MAINWIND_H
