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
#include "QtMidiFile.h"

namespace Ui {
class PianoRoll;
}

class PianoRollLine : public QObject, QGraphicsRectItem
{
    Q_OBJECT
public:
    PianoRollLine(QObject* parent, QGraphicsScene* s);
    inline ~PianoRollLine() {}

public slots:
    void setTick(qint32 tick);
};

class PianoRollEvent : public QGraphicsRectItem
{
public:
    inline PianoRollEvent() : QGraphicsRectItem(0)
    {
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsMovable);
    }
    inline ~PianoRollEvent() {}

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

class PianoRoll : public QGraphicsView
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
