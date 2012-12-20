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

#include "PianoRoll.h"
#include "ui_PianoRoll.h"

#include "MainWind.h"

#ifndef QT_NO_OPENGL
#   include <QGLWidget>
#endif

#include <QMenu>
#include <QtDebug>
#include <math.h> // for pow()

PianoRollLine::PianoRollLine(QObject* parent, QGraphicsScene* s)
    : QObject(parent), QGraphicsRectItem(0)
{
    setBrush(Qt::black);
    setRect(0,0,1,s->height()-5);
}

void PianoRollLine::setTick(qint32 tick)
{
    int x = this->x(), y = this->y(),
            w = rect().width(), h = rect().height();

    this->setPos(tick/2.0,0);
    this->scene()->update(tick/2.0,0,1,scene()->height());

    QGraphicsView *p = qobject_cast<QGraphicsView*>(parent());
    p->ensureVisible(this,75,0-scene()->height());

    this->scene()->update(x,y,w,h);
}

void PianoRollEvent::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    e->accept();
    int note = y()/7;
    qint32 tick = x()*2;
    qDebug() << tick << note;
    if(note > 127) { note = 127; }
    if(note < 0) { note = 0; }
    if(tick < 0) { tick = 0; }
    qDebug() << tick << note;
    setY(note*7);
    setX(tick/2.0);
    myNoteOn->setNote(note);
    myNoteOn->setTick(tick);
    myNoteOff->setNote(note);
    myNoteOff->setTick(tick+(rect().width()/2.0));
    this->scene()->update(x(),y(),rect().width(),rect().height());
}

void PianoRollEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(myColor);
    if(this->rect().width() > 3)
    { painter->drawRoundedRect(rect(),3,3); }
    else { painter->drawRoundedRect(rect(),1,1); }
}

PianoRoll::PianoRoll(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::PianoRoll)
{
    ui->setupUi(this);

#ifndef QT_NO_OPENGL
    this->setViewport(new QGLWidget());
#endif

    this->setScene(new QGraphicsScene(this));
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    file = 0;
    line = 0;
}

PianoRoll::~PianoRoll()
{
    delete ui;
}

PianoRollLine* PianoRoll::initLine()
{
    line = new PianoRollLine(this,scene());
    scene()->addItem((QGraphicsItem*)line);
    line->setTick(0);
    return line;
}

void PianoRoll::deleteLine()
{
    scene()->removeItem((QGraphicsItem*)line);
    delete line;
    line = 0;
}

void PianoRoll::initEditor(QtMidiFile* f)
{
    scene()->clear();
    file = f;
    PianoRollEvent* edEv = 0;
    QtMidiEvent* noteOn = 0;

    QMap<int,QtMidiEvent*> lastNoteOn;

    foreach(QtMidiEvent*e, file->events()) {
        if(e->isNoteEvent()) {
            if(e->type() == QtMidiEvent::NoteOff) {
                edEv = new PianoRollEvent();
                edEv->setColor(MainWind::trackColors->value(e->track()));
                noteOn = lastNoteOn.value(e->note(),0);
                if(!noteOn) { continue; }

                qreal y = (127 - e->note())*7;
                qreal w = (e->tick() - noteOn->tick())/2.0;
                edEv->setSize(noteOn->tick()/2.0,y,w,7);
                edEv->setNoteOnAndOff(noteOn,e);
                scene()->addItem(edEv);

                lastNoteOn.remove(e->note());
            }
            else
            { lastNoteOn.insert(e->note(),e); }
        }
    }
    this->setSceneRect(QRect());
}
void PianoRoll::wheelEvent(QWheelEvent* e)
{
    // http://www.qtcentre.org/threads/35738#post174006
    if(e->modifiers().testFlag(Qt::ControlModifier)) {
        int numSteps = e->delta() / 15 / 8;

        if(numSteps == 0) {
            e->ignore();
            return;
        }
        qreal sc = pow(1.25, numSteps);
        zoom(sc, mapToScene(e->pos()));
        e->accept();
    }
    else
    { QGraphicsView::wheelEvent(e); }
}

void PianoRoll::zoom(qreal factor, QPointF centerPoint)
{
    scale(factor, factor);
    centerOn(centerPoint);
}

void PianoRoll::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu m(this);
    m.addAction(tr("Zoom 100%"));
    QAction* a = m.exec(mapToGlobal(event->pos()));
    if(a->text() == tr("Zoom 100%"))
    { this->resetTransform(); }
}
