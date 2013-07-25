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

#include "PianoRoll.h"
#include "ui_PianoRoll.h"

#include "MainWind.h"

#ifndef QT_NO_OPENGL
#   include <QGLWidget>
#endif

#include <QMenu>
#include <math.h> // for pow()

/* Static vars. */
bool PianoRoll::canMoveItems;

PianoRollLine::PianoRollLine(QObject* parent)
    : QObject(parent), QGraphicsRectItem(0)
{
    setBrush(Qt::black);
    setRect(0,0,1,127*7);
    oldTick = 0;
    p = qobject_cast<QGraphicsView*>(parent);
}

void PianoRollLine::setTick(qint32 tick)
{
    if(tick == oldTick) { return; }
    if(tick-oldTick < 15) { return; }
    oldTick = tick;

    int x = this->x(), y = this->y(),
            w = rect().width(), h = rect().height();

    this->setPos(tick/2.0,0);
    if(!scene()) { return; }
    this->scene()->update(tick/2.0,0,1,scene()->height());

    p->ensureVisible(this,p->viewport()->width()/2,0-scene()->height());

    this->scene()->update(x,y,w,h);
}

PianoRoll::PianoRoll(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::PianoRoll)
{
    ui->setupUi(this);
    this->setScene(new QGraphicsScene(this));
    this->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    tools = new QActionGroup(this);
    tools->addAction(ui->actionNavigationTool);
    tools->addAction(ui->actionMoveTool);

    connect(MainWind::settings,SIGNAL(somethingChanged(QString)),this,SLOT(handleChange(QString)));
#ifndef QT_NO_OPENGL
    if(MainWind::settings->getHWA()) { this->setViewport(new QGLWidget()); }
#endif

    file = 0;
    line = 0;
    canMoveItems = false;
}

PianoRoll::~PianoRoll()
{
    delete ui;
}

void PianoRoll::handleChange(QString a)
{
#ifndef QT_NO_OPENGL
    if(a == "HWA") {
        if(MainWind::settings->getHWA()) { this->setViewport(new QGLWidget()); }
        else { this->setViewport(new QWidget()); }
    }
#endif
}

void PianoRoll::handleNoteChange()
{
    emit somethingChanged();
}

PianoRollLine* PianoRoll::initLine(qint32 tick)
{
    line = new PianoRollLine(this);
    line->setTick(tick);
    scene()->addItem((QGraphicsItem*)line);
    return line;
}

void PianoRoll::deleteLine()
{
    scene()->removeItem((QGraphicsItem*)line);
    delete line;
    line = 0;
}

void PianoRoll::initEditor(QMidiFile* f)
{
    scene()->clear();
    file = f;
    PianoRollEvent* edEv = 0;
    QMidiEvent* noteOn = 0;

    QMap<int,QMidiEvent*> lastNoteOn;

    QList<QMidiEvent*>* events = file->events();
    for(int i = 0;i<events->count(); i++) {
        QMidiEvent* e = events->at(i);
        if(e->isNoteEvent()) {
            if(e->type() == QMidiEvent::NoteOff) {
                noteOn = lastNoteOn.value(e->note(),0);
                if(!noteOn) { continue; }
                edEv = new PianoRollEvent();
                edEv->setColor(MainWind::trackColors->value(e->track()));
                connect(edEv,SIGNAL(somethingChanged()),
                        this,SLOT(handleNoteChange()));

                qreal y = (127 - e->note())*7;
                qreal w = (e->tick() - noteOn->tick())/2.0;
                edEv->setSize(noteOn->tick()/2.0,y,w,7);
                edEv->setNoteOnAndOff(noteOn,e);
                scene()->addItem((QGraphicsItem*)edEv);

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
    if(!a) { return; }
    if(a->text() == tr("Zoom 100%"))
    { this->resetTransform(); }
}

void PianoRoll::on_actionMoveTool_toggled(bool v)
{
    canMoveItems = v;
}

PianoRollEvent::PianoRollEvent(QObject *p)
    : QObject(p), QGraphicsRectItem(0)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
}
void PianoRollEvent::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if(PianoRoll::canMoveItems) {
        QGraphicsRectItem::mousePressEvent(e);
    }
}
void PianoRollEvent::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if(PianoRoll::canMoveItems) {
        QGraphicsRectItem::mouseMoveEvent(e);
    }
}
void PianoRollEvent::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(!PianoRoll::canMoveItems) { return; }
    QGraphicsRectItem::mouseReleaseEvent(e);

    int note = 127-(y()/7);
    qint32 tick = x()*2;
    qint32 dur = myNoteOff->tick()-myNoteOn->tick();
    if(tick == myNoteOn->tick()) { return; }
    else { emit somethingChanged(); }

    if(note > 127) { note = 127; }
    if(note < 0) { note = 0; }
    if(tick < 0) { tick = 0; }
    setY((127-note)*7);
    setX(tick/2.0);

    myNoteOn->setNote(note);
    myNoteOn->setTick(tick);
    myNoteOff->setNote(note);
    myNoteOff->setTick(tick+dur);
}
void PianoRollEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(myColor);
    if(this->rect().width() > 3)
    { painter->drawRoundedRect(rect(),3,3); }
    else { painter->drawRoundedRect(rect(),1,1); }
}
