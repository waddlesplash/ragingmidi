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

#include "VirtualPiano.h"

#include "../MainWind.h"

#ifndef QT_NO_OPENGL
#   include <QGLWidget>
#endif

int VirtualPiano::voiceToUse;

VirtualPianoKey::VirtualPianoKey(QObject *parent, int midiKey, QString noteName, bool isBlackKey)
    : QObject(parent), QGraphicsRectItem(0)
{
    isBlack = isBlackKey;
    if(isBlack) { setZValue(1); }
    isHovering = false; isClicking = false;
    this->setAcceptHoverEvents(true);
    this->setAcceptedMouseButtons(Qt::LeftButton);

    myMidiKey = midiKey;
    myNoteName = noteName;
}

void VirtualPianoKey::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    isClicking = true; this->update();
    QtMidi::outNoteOn(myMidiKey,VirtualPiano::voiceToUse);
}
void VirtualPianoKey::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    isClicking = false; this->update();
    QtMidi::outNoteOff(myMidiKey,VirtualPiano::voiceToUse);
}

void VirtualPianoKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int x = rect().x(); int y = rect().y();
    int w = rect().width(); int h = rect().height();

    painter->setPen(Qt::gray);

    if(isHovering && !isClicking) {
        painter->setBrush(QColor("#8080ff"));
    } else if(isClicking) {
        painter->setBrush(Qt::blue);
    } else {
        if(isBlack) { painter->setBrush(Qt::black); }
        else if(myMidiKey == 60) { painter->setBrush(QColor("#80ff80")); }
        else { painter->setBrush(Qt::white); }
    }

    painter->drawRect(rect());

    QList<QColor> colors = trackColors();

    if(colors.size() > 0) {
        int rectH = h/colors.size();
        int nextY = y;
        for(int i = 0;i<colors.size();i++)
        {
            painter->setPen(colors.at(i));
            painter->setBrush(colors.at(i));
            if(i == colors.size()-1) // x+1&w-2 so that the key has an outline
            { painter->drawRect(x+1,nextY,w-2,h-nextY); }
            else
            { painter->drawRect(x+1,nextY,w-2,rectH); }
            nextY += rectH;
        }
    }

    if(isBlack) { return; }

    // Draw the key name (not for Black keys)
    painter->setPen(Qt::black);
    QFont f = painter->font();
    f.setPointSize(8);
    painter->setFont(f);
    painter->drawText(x+7,y+53,myNoteName);
}

QList<QColor> VirtualPianoKey::trackColors()
{
    lock.lockForRead();
    QList<QColor> ret = myColors;
    lock.unlock();
    return ret;
}
void VirtualPianoKey::setTrackColors(QList<QColor> newColors)
{
    lock.lockForWrite();
    myColors = newColors;
    lock.unlock();
    this->update();
}
void VirtualPianoKey::addTrackColor(int track)
{
    QColor trackC = MainWind::trackColors->value(track);
    lock.lockForRead();
    if(!myColors.contains(trackC))
    {
        lock.unlock();
        lock.lockForWrite();
        myColors.append(trackC);
        this->scene()->update(x(),y(),rect().x(),rect().y());
    }
    lock.unlock();
}
void VirtualPianoKey::removeTrackColor(int track)
{
    QColor trackC = MainWind::trackColors->value(track);
    lock.lockForRead();
    if(myColors.contains(trackC))
    {
        lock.unlock();
        lock.lockForWrite();
        myColors.removeOne(trackC);
        this->scene()->update(x(),y(),rect().x(),rect().y());
    }
    lock.unlock();
}


VirtualPiano::VirtualPiano(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setMinimumHeight(80);
#ifndef QT_NO_OPENGL
    this->setViewport(new QGLWidget());
#endif

    QGraphicsScene* scene = new QGraphicsScene(this);
    this->setScene(scene);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    k = 0;
    curKey = 0;

    for(int i=0;i<10;i++) {
        addWhiteKey(scene,"C");
        addBlackKey(scene,"C#"); // or Db
        addWhiteKey(scene,"D");
        addBlackKey(scene,"D#"); // or Eb
        addWhiteKey(scene,"E");
        addWhiteKey(scene,"F");
        addBlackKey(scene,"F#"); // or Gb
        addWhiteKey(scene,"G");
        addBlackKey(scene,"G#"); // or Ab
        addWhiteKey(scene,"A");
        addBlackKey(scene,"A#"); // or Bb
        addWhiteKey(scene,"B");
    }
    // The last octave has 4
    // less keys.
    addWhiteKey(scene,"C");
    addBlackKey(scene,"C#"); // or Db
    addWhiteKey(scene,"D");
    addBlackKey(scene,"D#"); // or Eb
    addWhiteKey(scene,"E");
    addWhiteKey(scene,"F");
    addBlackKey(scene,"F#"); // or Gb
    addWhiteKey(scene,"G");

    voiceToUse = 0;
}

void VirtualPiano::addWhiteKey(QGraphicsScene* scene, QString noteName)
{
    VirtualPianoKey* newKey = new VirtualPianoKey(this,curKey,noteName,false);
    qreal loc = k*20;
    newKey->setSize(loc,0,20,60);
    scene->addItem((QGraphicsItem*)newKey);
    keys.insert(curKey,newKey);
    k += 1;
    curKey += 1;
}

void VirtualPiano::addBlackKey(QGraphicsScene *scene, QString noteName)
{
    VirtualPianoKey*newKey = new VirtualPianoKey(this,curKey,noteName,true);
    qreal loc = (k*20)-5;
    newKey->setSize(loc,0,10,40);
    scene->addItem((QGraphicsItem*)newKey);
    keys.insert(curKey,newKey);
    curKey += 1;
}

void VirtualPiano::clearTrackColors()
{
    foreach(VirtualPianoKey*key,keys.values())
    { key->setTrackColors(QList<QColor>()); }
}

void VirtualPiano::clearTrackColors(int track)
{
    foreach(VirtualPianoKey*key,keys.values())
    { key->removeTrackColor(track); }
}
