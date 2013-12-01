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

#ifndef QT_NO_OPENGL
#   include <QGLWidget>
#endif

#include <QMenu>
#include <QPushButton>
#include <math.h> // for pow()
#include "MainWind.h"

#define NOTE_HEIGHT 9 /* pixels */

/* Static vars. */
bool PianoRoll::canMoveItems;
PianoRoll* PianoRoll::me;

/*******************************************************/

PianoRollLine::PianoRollLine(QObject* parent)
    : QObject(parent), QGraphicsRectItem(0)
{
    setBrush(Qt::black);
    setRect(0,0,1,127*NOTE_HEIGHT);
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
    /* 0-scene()->height() ensures that the vertical viewport location is
     * NOT changed when this function fires. */

    this->scene()->update(x,y,w,h);
}

/*******************************************************/

PianoRoll::PianoRoll(QWidget *parent) :
    QGraphicsView(parent)
{
    darker = QBrush(QColor("#c2e6ff"));
    lighter1 = QBrush(QColor("#eaf6ff"));
    lighter2 = QBrush(QColor("#daffd3"));

    this->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    moveTool_toggled(false);

    connect(MainWind::settings,SIGNAL(somethingChanged(QString)),this,SLOT(handleChange(QString)));
#ifndef QT_NO_OPENGL
    if(MainWind::settings->getHWA()) { this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers))); }
#endif

    midiFile = 0;
    line = 0;
    me = this;
}

void PianoRoll::init(QWidget* controlsContainer, QGridLayout* controlsLayout)
{
    QPushButton* a = new QPushButton(controlsContainer);
    a->setCheckable(true);
    a->setChecked(true);
    a->setAutoExclusive(true);
    a->setText(tr("Navigation tool"));
    controlsLayout->addWidget(a,1,1);

    a = new QPushButton(controlsContainer);
    a->setCheckable(true);
    a->setChecked(false);
    a->setAutoExclusive(true);
    a->setText(tr("Move tool"));
    connect(a,SIGNAL(toggled(bool)),this,SLOT(moveTool_toggled(bool)));
    controlsLayout->addWidget(a,1,2);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    controlsLayout->addWidget(spacer,1,3);
}

void PianoRoll::handleChange(QString a)
{
#ifndef QT_NO_OPENGL
    if(a == "HWA") {
        if(MainWind::settings->getHWA()) { this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers))); }
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

void PianoRoll::finishMove()
{
    /* This calls all the finishMove() events
     * in all selected items. For some reason,
     * not all the mouse events are fired on
     * all items when more than one item is moved */
    foreach(QGraphicsItem* i, this->scene()->selectedItems()) {
        PianoRollEvent* e = (PianoRollEvent*)i;
        e->finishMove();
    }
}

void PianoRoll::initEditor(QMidiFile* f)
{
    if(this->scene()) {
        this->scene()->clear();
        delete this->scene();
    }
    this->setScene(new QGraphicsScene(this));
    midiFile = f;
    PianoRollEvent* edEv = 0;
    QMidiEvent* noteOn = 0;

    QMap<int,QMidiEvent*> lastNoteOn;

    QList<QMidiEvent*>* events = midiFile->events();
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

                qreal y = (127 - e->note())*NOTE_HEIGHT;
                qreal w = (e->tick() - noteOn->tick())/2.0;
                edEv->setSize(noteOn->tick()/2.0,y,w,NOTE_HEIGHT);
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

static int pianoKeyColor[12] = {
    0 /* white */, 1 /* black */, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0
};

void PianoRoll::drawBackground(QPainter *painter, const QRectF &rect)
{
    qreal width = rect.width();

    painter->setClipRect(rect);
    painter->setPen(Qt::NoPen);
    for(int i = 0; i < 128; i++) {
        if(i == 60) { // Middle C
            painter->setBrush(QBrush(QColor("#80ff80")));
        } else {
            int octave = (i - 5) / 12;
            painter->setBrush(pianoKeyColor[i % 12] ? darker : ((octave % 2) ? lighter1 : lighter2));
        }
        painter->drawRect(QRectF(rect.x(),i*NOTE_HEIGHT,width,NOTE_HEIGHT));
    }
}

void PianoRoll::moveTool_toggled(bool v)
{
    if(v) {
        canMoveItems = true;
        this->setDragMode(QGraphicsView::RubberBandDrag);
    } else {
        canMoveItems = false;
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        if(!this->scene()) { return; } /* Otherwise, this crashes the program */
        foreach(QGraphicsItem* s, this->scene()->selectedItems()) {
            s->setSelected(false);
        }
    }
}

/*******************************************************/

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
    PianoRoll::me->finishMove();
}
void PianoRollEvent::finishMove()
{
    int note = 127-(y()/NOTE_HEIGHT);
    qint32 tick = x()*2;
    qint32 dur = myNoteOff->tick()-myNoteOn->tick();
    if(tick == myNoteOn->tick()) { return; }
    else { emit somethingChanged(); }

    if(note > 127) { note = 127; }
    if(note < 0) { note = 0; }
    if(tick < 0) { tick = 0; }
    setY((127-note)*NOTE_HEIGHT);
    setX(tick/2.0);

    myNoteOn->setNote(note);
    myNoteOn->setTick(tick);
    myNoteOff->setNote(note);
    myNoteOff->setTick(tick+dur);
}

void PianoRollEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(myColor);
    if(this->isSelected()) {
        painter->setPen(Qt::DashLine);
    } else {
        painter->setPen(Qt::SolidLine);
    }
    if(this->rect().width() > 3) {
        painter->drawRoundedRect(rect(),3,3);
    } else {
        painter->drawRoundedRect(rect(),1,1);
    }
}
