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

#ifndef PIANOROLL_H
#define PIANOROLL_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QPointF>

#include <QtMidiFile.h>

/****h* RagingMidi/PianoRoll.h
 * PURPOSE
 *   Events and painting for the "PianoRoll" widget which
 *   displays a scrolling piano roll of the notes on tracks.
 ******
 */

namespace Ui {
class PianoRoll;
}

/****c* PianoRoll.h/PianoRollLine
 * SYNOPSIS
 */
class PianoRollLine : public QObject, QGraphicsRectItem
/**
 * DESCRIPTION
 *   The scrolling line that shows the current play location.
 ******
 */
{
    Q_OBJECT
public:
    PianoRollLine(QObject* parent, QGraphicsScene* s);

public slots:
    void setTick(qint32 tick);
};


/****c* PianoRoll.h/PianoRollEvent
 * SYNOPSIS
 */
class PianoRollEvent : public QGraphicsRectItem
/**
 * DESCRIPTION
 *  Rectangle that shows the location of one pair of
 *  NoteOn and NoteOff events in the piano roll.
 ******
 */
{
public:
    inline PianoRollEvent() : QGraphicsRectItem(0)
    {
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsMovable);
    }

    inline void setSize(qreal x, qreal y, qreal w, qreal h)
    { this->setPos(x,y); this->setRect(0,0,w,h); }

    inline void setNoteOnAndOff(QtMidiEvent* noteOn, QtMidiEvent* noteOff)
    { myNoteOn = noteOn; myNoteOff = noteOff; }

    inline void setColor(QColor c) { myColor = c; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

private:
    QtMidiEvent* myNoteOn;
    QtMidiEvent* myNoteOff;
    QColor myColor;
};


/****c* PianoRoll.h/PianoRoll
 * SYNOPSIS
 */
class PianoRoll : public QGraphicsView
/**
 * DESCRIPTION
 *   Widget that handles the display and the
 *   management of PianoRollEvent[s].
 ******
 */
{
    Q_OBJECT
    
public:
    explicit PianoRoll(QWidget *parent = 0);
    ~PianoRoll();

    void initEditor(QtMidiFile* f);

    PianoRollLine* initLine(qint32 tick);
    void deleteLine();

protected:
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);
    
private:
    Ui::PianoRoll *ui;
    QtMidiFile* file;

    PianoRollLine* line;

    void zoom(qreal factor, QPointF centerPoint);
};

#endif // PianoRoll_H
