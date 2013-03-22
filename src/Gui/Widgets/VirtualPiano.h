/*
 * Raging MIDI (https://github.com/waddlesplash/ragingmidi).
 *
 * Copyright (c) 2012-2013 WaddleSplash & contributors (see AUTHORS.txt).
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

#ifndef VIRTUALPIANO_H
#define VIRTUALPIANO_H

/****h* RagingMidi/VirtualPiano.h
 * PURPOSE
 *   Creates an interactive virtual piano
 *   that plays MIDI notes and shows which ones
 *   are being played in the song.
 ******
 */

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPaintEvent>
#include <QList>
#include <QMap>
#include <QColor>
#include <QReadWriteLock>

#include <QMidi.h>

namespace Ui {
class VirtualPiano;
}

/****c* VirtualPiano.h/VirtualPianoKey
 * SYNOPSIS
 */
class VirtualPianoKey : public QObject, QGraphicsRectItem
/**
 * DESCRIPTION
 *   Draws one key of the virtual piano, either black or white.
 *   It is told which MIDI tracks are currently playing it, and
 *   it highlights the key with that knowledge.
 ******
 */
{
    Q_OBJECT
public:
    explicit VirtualPianoKey(QObject *parent = 0, int midiKey = 0, QString noteName = "", bool isBlackKey = false);

    inline void setSize(int x, int y, int w, int h)
    { this->setRect(0,0,w,h); this->setPos(x,y); }

    QList<QColor> trackColors();
    void setTrackColors(QList<QColor> newColors);
    void addTrackColor(int track);
    void removeTrackColor(int track);

private:
    bool isBlack;
    bool isHovering;
    bool isClicking;
    int myMidiKey;
    QString myNoteName;

    QList<QColor> myColors;
    QReadWriteLock lock;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);

    inline void hoverEnterEvent(QGraphicsSceneHoverEvent *)
    { isHovering = true; this->scene()->update(x(),y(),rect().x(),rect().y()); }
    inline void hoverLeaveEvent(QGraphicsSceneHoverEvent *)
    { isHovering = false; this->scene()->update(x(),y(),rect().x(),rect().y()); }

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

/****c* VirtualPiano.h/VirtualPiano
 * SYNOPSIS
 */
class VirtualPiano : public QGraphicsView
/**
 * DESCRIPTION
 *   Creates 128 VirtualPianoKey[s], places them in
 *   their correct positons, and keeps track of what
 *   key maps to what note.
 ******
 */
{
    Q_OBJECT
    
public:
    explicit VirtualPiano(QWidget *parent = 0);

    inline VirtualPianoKey* key(int num) { return keys.value(num); }
    void clearTrackColors();

    static int voiceToUse;

public slots:
    void addTrackColor(int note, int track);
    void removeTrackColor(int note, int track);
    void clearTrackColors(int track);

private slots:
    void handleChange(QString a);
    
private:
    QMap<int,VirtualPianoKey*> keys;

    int k;
    int curKey;

    void addWhiteKey(QGraphicsScene* scene, QString noteName);
    void addBlackKey(QGraphicsScene* scene, QString noteName);
};

#endif // VIRTUALPIANO_H
