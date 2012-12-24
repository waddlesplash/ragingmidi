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

#ifndef TRACKSEDIT_H
#define TRACKSEDIT_H

/****h* RagingMidi/TracksEdit.h
 * PURPOSE
 *   Subclass of QTreeWidget and related
 *   classes to create a widget to edit tracks
 *   with.
 ******
 */

#include <QTreeWidget>
#include <QtMidiFile.h>
#include <QStringList>
#include <QSlider>
#include <QMap>

#include "VirtualPiano.h"

namespace Ui {
class TracksEdit;
}

/****c* TracksEdit.h/TrackSlider
 * SYNOPSIS
 */
class TrackSlider : public QSlider
/**
 * DESCRIPTION
 *   Subclass of QSlider that can store what MIDI track
 *   it was created for.
 ******
 */
{
    Q_OBJECT
public:
    inline TrackSlider(QWidget* p = 0)
        : QSlider(p) { setOrientation(Qt::Horizontal); myTrack = 0; }

    inline int track() { return myTrack; }
    inline void setTrack(int t) { myTrack = t; }

protected:
    inline void wheelEvent(QWheelEvent *) {}

private:
    int myTrack;
};

/****c* TracksEdit.h/TrackItem
 * SYNOPSIS
 */
class TrackItem : public QTreeWidgetItem
/**
 * DESCRIPTION
 *   Subclass of QTreeWidgetItem that is
 *   designed to store MIDI track data.
 ******
 */
{
public:
    TrackItem(QTreeWidget* tree = 0, int track = 0);

    enum ColumnIds {
        Name = 0,
        Type = 1,
        On = 2,
        Device = 3,
        Voice = 4,
        Inst = 5,
        Vol = 6,
        Bal = 7,
        TrackNumber = 8
    };

    inline void setName(QString name) { setText(Name,name); }
    inline QString name() { return text(Name); }

    inline void setType(QString type) { setText(Type,type); }
    inline QString type() { return text(Type); }

    inline void setOn(QString on) { setText(On,on); }
    inline QString on() { return text(On); }

    inline void setDevice(QString device) { setText(Device,device); }
    inline QString device() { return text(Device); }

    inline void setVoice(int voice) { setText(Voice,QString::number(voice+1)); }
    inline int voice() { return text(Voice).toInt()-1; }

    inline void setInst(QString ins) { setText(Inst,ins); }
    inline QString inst() { return text(Inst); }

    inline void setVol(int vol) { volSL->setValue(vol); }
    inline int vol() { return volSL->value(); }
    inline TrackSlider* volSlider() { return volSL; }

    inline void setBal(int bal) { balSL->setValue(bal); }
    inline int bal() { return balSL->value(); }
    inline TrackSlider* balSlider() { return balSL; }

    inline int track() { return text(TrackNumber).toInt(); }

private:
    int myTrackId;
    TrackSlider* volSL;
    TrackSlider* balSL;
};

/****c* TracksEdit.h/TracksEdit
 * SYNOPSIS
 */
class TracksEdit : public QTreeWidget
/**
 * DESCRIPTION
 *   Subclass of QTreeWidget that is
 *   designed to use the above two classes
 *   to edit MIDI track data.
 ******
 */
{
    Q_OBJECT
    
public:
    explicit TracksEdit(QWidget *parent = 0);
    ~TracksEdit();

    QMap<int,QColor>* trackColors() { return &myTrackColors; }
    QMap<int,bool>* trackStatus() { return &myTrackStatus; }

    void init(VirtualPiano* p);
    void setupTracks(QtMidiFile* f);

    TrackItem* createTrack(int trackNum);
    QList<TrackItem*> tracks();

    void deleteCurTrack();

signals:
    void somethingChanged();
    
private slots:
    void trackItem_volChanged(int v);
    void trackItem_balChanged(int b);

    void tracksEdit_itemClicked(QTreeWidgetItem *item, int column);
    void tracksEdit_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::TracksEdit *ui;
    QMap<int,QColor> myTrackColors;
    QMap<int,bool> myTrackStatus;
    bool ignoreEvents;

    QtMidiFile *midiFile;
    VirtualPiano *piano;
    QStringList colorNames;

    void updateTrackOn();
};

#endif // TRACKSEDIT_H
