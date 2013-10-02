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

#include "TracksEdit.h"
#include "ui_TracksEdit.h"

#include <QColor>
#include <QMessageBox>
#include <QMidiOut.h>
#include "../MainWind.h"

#include "../Selectors/SelectInstrument.h"

void TrackSlider::setValue(int v)
{
    valToRevertTo = v;
    QSlider::setValue(v);
}

void TrackSlider::revert()
{
    blockSignals(true);
    setValue(valToRevertTo);
    blockSignals(false);
}

TrackPreview::TrackPreview(QWidget *parent, int track, QMidiFile *f)
    : QWidget(parent)
{
    setMinimumSize(QSize(300,20));
    trackNum = track;
    midiFile = f;
    curTick = 0;
}

void TrackPreview::tickChanged(int t)
{
    if(curTick == t) { return; }
    curTick = t;
    repaint();
}

#define SCALE_FACTOR 32 // 1/32nd actual size
void TrackPreview::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setBrush(Qt::SolidPattern);
    p.setClipRegion(event->region());
    int w = size().width(), h = size().height();
    QList<QMidiEvent*>* events = midiFile->events();

    p.setPen(Qt::gray);
    p.drawLine(0,0,w,0); // Top
    // p.drawLine(0,h-1,w,h-1); // Bottom (don't need)
    p.drawLine(0,0,0,h); // Left

    int startTick = curTick-((w*SCALE_FACTOR)/2); // Scale is 1/4, but we want center
    int endTick = curTick+((w*SCALE_FACTOR)/2);
    if(endTick > events->last()->tick()) {
        endTick = events->last()->tick();
        startTick = endTick - (w*SCALE_FACTOR);
    }
    if(startTick < 0) { endTick += startTick*-1; startTick = 0; }

    qreal x = (curTick-startTick)/SCALE_FACTOR;
    p.drawRect(x,0,2,h);
    p.setPen(Qt::black);

    for(int i = 0;i<events->count(); i++) {
        QMidiEvent* e = events->at(i);
        if((e->tick() < startTick) ||
                (e->type() != QMidiEvent::NoteOn) ||
                (e->track() != trackNum)) { continue; }
        if(e->tick() > endTick) { return; }

        qreal y = ((127.0-e->note())/127.0)*h;
        if(y > h) { y = h; } if(y < 0) { y = 0; }
        p.drawRect(QRectF((e->tick()-startTick)/SCALE_FACTOR,y,2,2));
    }
}

TrackItem::TrackItem(QTreeWidget *tree, int track)
    : QTreeWidgetItem(tree)
{
    this->setText(TrackNumber,QString::number(track));

    volSL = new TrackSlider(this->treeWidget());
    volSL->setTracking(false);
    volSL->setMinimum(0);
    volSL->setValue(100);
    volSL->setMaximum(100);
    this->treeWidget()->setItemWidget(this,Vol,volSL);

    balSL = new TrackSlider(this->treeWidget());
    balSL->setTracking(false);
    balSL->setMinimum(0);
    balSL->setMaximum(127);
    balSL->setValue(64); // but its 0-127, not 128
    this->treeWidget()->setItemWidget(this,Bal,balSL);
}

TracksEdit::TracksEdit(QWidget *parent) :
    QTreeWidget(parent),
    ui(new Ui::TracksEdit)
{
    ui->setupUi(this);
    this->hideColumn(TrackItem::TrackNumber);

    colorNames = QColor::colorNames();
    colorNames.removeOne("black");
    colorNames.removeOne("white");
    colorNames.removeOne("azure");
    colorNames.removeOne("aliceblue");

    connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(tracksEdit_itemClicked(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,SLOT(tracksEdit_itemChanged(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(itemEntered(QTreeWidgetItem*,int)),
            this,SLOT(tracksEdit_itemEntered(QTreeWidgetItem*,int)));

    this->setMouseTracking(true); // Otherwise, itemEntered() won't fire
    resizeColsToContents();
}

TracksEdit::~TracksEdit()
{
    delete ui;
}

void TracksEdit::resizeColsToContents()
{
    for(int i = 0;i<this->columnCount();i++)
    { this->resizeColumnToContents(i); }
}

TrackItem* TracksEdit::createTrack(int trackNum)
{
    TrackItem* ret = new TrackItem(this,trackNum);
    ret->setBackgroundColor(TrackItem::Name,QColor(colorNames.at(trackNum)));
    myTrackColors.insert(trackNum,QColor(colorNames.at(trackNum)));
    ret->setType(tr("Instrument"));
    ret->setOn(tr("on"));
    ret->setDevice("<automatic>");

    ret->volSlider()->setTrack(trackNum);
    ret->balSlider()->setTrack(trackNum);
    connect(ret->volSlider(),SIGNAL(valueChanged(int)),this,SLOT(trackItem_volChanged(int)));
    connect(ret->balSlider(),SIGNAL(valueChanged(int)),this,SLOT(trackItem_balChanged(int)));

    return ret;
}
void TracksEdit::deleteTrack(int trackNum)
{
    /* Deletes specified track and all items in it. */
    TrackItem* i = tracks().at(trackNum);
    i->~QTreeWidgetItem();
    foreach(QMidiEvent*e,midiFile->eventsForTrack(trackNum))
    {
        midiFile->removeEvent(e);
        delete e;
    }
    midiFile->removeTrack(trackNum);
}
void TracksEdit::deleteCurTrack()
{
    /* Deletes the current track. Uses deleteTrack(). */
    TrackItem* i = static_cast<TrackItem*>(this->selectedItems().at(0));
    if(!i) { return; }

    int trackNum = i->track();
    deleteTrack(trackNum);
}
void TracksEdit::removeTrack(int trackNum)
{
    /* Deletes the TrackItem for the specified track.
     * Does not delete the track itself. */
    TrackItem* i = tracks().at(trackNum);
    i->~QTreeWidgetItem();
}

void TracksEdit::trackItem_volChanged(int v)
{
    if(ignoreEvents) { return; }
    TrackSlider* sl = qobject_cast<TrackSlider*>(sender());
    if(!sl) { return; }

    int trk = sl->track();
    int vel = 127.0*(v/100.0);
    int oldVel = -1;
    foreach(QMidiEvent* e, midiFile->eventsForTrack(trk))
    {
        if(e->type() != QMidiEvent::NoteOn) { continue; }
        if(oldVel == -1) { oldVel = e->velocity(); continue; }
        if(oldVel != e->velocity()) {
            int ret = QMessageBox::warning(this->parentWidget(),tr("Different volumes!"),
                                           tr("<b>Some of the notes in this track have different volumes than others.</b><br/>"
                                              "Are you sure you want to modify the volume?"),
                                           QMessageBox::Ok,QMessageBox::Cancel);
            if(ret == QMessageBox::Ok) { break; }
            else { sl->revert(); return; }
        }
    }
    foreach(QMidiEvent* e, midiFile->eventsForTrack(trk))
    {
        if(e->type() != QMidiEvent::NoteOn) { continue; }
        e->setVelocity(vel);
    }
    emit somethingChanged();
}
void TracksEdit::trackItem_balChanged(int b)
{
    if(ignoreEvents) { return; }
    TrackSlider* sl = qobject_cast<TrackSlider*>(sender());
    if(!sl) { return; }

    int trk = sl->track();
    int oldVal = -1;
    foreach(QMidiEvent* e, midiFile->eventsForTrack(trk))
    {
        if((e->type() != QMidiEvent::ControlChange) ||
                (e->number() != /* coarse pan */10)) { continue; }

        if(oldVal == -1) { oldVal = e->value(); continue; }
        if(oldVal != e->value()) {
            int ret = QMessageBox::warning(this->parentWidget(),tr("Different Balances!"),
                                           tr("<b>This track has varying Balance events!</b><br/>"
                                              "Are you sure you want to modify the balance?"),
                                           QMessageBox::Ok,QMessageBox::Cancel);
            if(ret == QMessageBox::Ok) { break; }
            else { sl->revert(); return; }
        }
    }
    bool bChanged = false;
    foreach(QMidiEvent* e, midiFile->eventsForTrack(trk))
    {
        if((e->type() != QMidiEvent::ControlChange) ||
                (e->number() != /* coarse pan */10)) { continue; }
        e->setValue(b);
        bChanged = true;
    }

    int voice = this->tracks().at(trk)->voice();
    if (!bChanged) {
        midiFile->createControlChangeEvent(trk, 0, voice, /* Coarse Pan */10, b);
    }
    MainWind::midiOut->controlChange(voice, /* Coarse Pan */10, b);
    emit somethingChanged();
}

QList<TrackItem*> TracksEdit::tracks()
{
    QList<TrackItem*> ret;
    QTreeWidgetItem *c;
    for(int i = 0;i<this->topLevelItemCount();i++) {
        c = this->topLevelItem(i);
        ret.append(static_cast<TrackItem*>(c));
    }
    return ret;
}

void TracksEdit::init(VirtualPiano* p)
{
    piano = p;
}

void TracksEdit::setupTracks(QMidiFile *f, QSlider *songPosSlider)
{
    ignoreEvents = true;
    midiFile = f;
    this->clear();
    foreach(int curTrack,midiFile->tracks())
    {
        TrackItem* i = this->createTrack(curTrack);

        bool didInstr = false, didVoice = false, didName = false, didVol = false,
                didBal = false;
        foreach(QMidiEvent* e, midiFile->eventsForTrack(curTrack))
        {
            if(!didVoice && e->type() == QMidiEvent::NoteOn)
            {
                i->setVoice(e->voice());
                if(e->voice() == 9) { i->setInst(tr("Drums")); didInstr = true; }
                didVoice = true;
            }
            if(!didVol && e->type() == QMidiEvent::NoteOn)
            {
                i->setVol((e->velocity()/127.0)*100);
                didVol = true;
            }
            else if(!didInstr && e->type() == QMidiEvent::ProgramChange)
            {
                int instr = e->number();
                SelectInstrument sel(this);
                sel.setInsNum(instr);
                i->setInst(sel.insName());
                MainWind::midiOut->setInstr(e->voice(),e->number());
                didInstr = true;
            }
            else if((e->type() == QMidiEvent::ControlChange) ||
                    (e->number() == /* Coarse Pan */10))
            {
                MainWind::midiOut->controlChange(e->voice(), /* Coarse Pan */10, e->value());
                i->setBal(e->value());
                didBal = true;
            }
            else if(!didName && (e->type() == QMidiEvent::Meta) &&
                    (e->number() == 0x03))
            {
                i->setName(QString::fromLatin1(e->data()));
                didName = true;
            }

            if(didInstr && didVoice && didName && didBal) { break; }
        }

        if((curTrack == 0) && !didInstr) {
            /* Track 0 and no instrument means that this
             * file's version is >0. T0 on that is a control track. */
            removeTrack(curTrack); // Remove the list item for this track
            continue;
        }

        if(!didInstr)
        { i->setInst(tr("(no instrument)")); }
        if(!didName)
        { i->setName(tr("Track %1","track number").arg(curTrack)); }
        if(!didBal && didVoice)
        { MainWind::midiOut->controlChange(i->voice(), /* Coarse Pan */10, 64); }

        TrackPreview* trPrev = new TrackPreview(this,i->track(),f);
        this->setItemWidget(i,TrackItem::Preview,trPrev);
        connect(songPosSlider,SIGNAL(valueChanged(int)),
                trPrev,SLOT(tickChanged(int)));
        connect(songPosSlider,SIGNAL(sliderMoved(int)),
                trPrev,SLOT(tickChanged(int)));
    }
    ignoreEvents = false;
    resizeColsToContents();
    myTrackStatus.clear();
    updateTrackOn();
}

void TracksEdit::updateTrackOn()
{
    QList<int> soloTracks;
    foreach(TrackItem* itm,tracks()) {
        if(itm->on() == tr("solo")) {
            soloTracks.append(itm->track());
            piano->clearTrackColors(itm->track());
        }
        bool on = (itm->on() == tr("on"));
        myTrackStatus.insert(itm->track(),on);
        if(!on) { MainWind::midiOut->stopAll(itm->voice()); }
    }

    if(soloTracks.size() < 1) { return; }

    foreach(int i,myTrackStatus.keys()) {
        if(soloTracks.contains(i)) {
            myTrackStatus.insert(i,true);
            continue;
        }
        myTrackStatus.insert(i,false);
    }
    MainWind::midiOut->stopAll();
    piano->clearTrackColors();
}

void TracksEdit::tracksEdit_itemClicked(QTreeWidgetItem *item, int column)
{
    TrackItem* itm = static_cast<TrackItem*>(item);
    if(column == TrackItem::Name) {                      /* NAME */
        Qt::ItemFlags oldFlags = itm->flags();
        itm->setFlags(oldFlags | Qt::ItemIsEditable);
        this->editItem(itm,column);
        itm->setFlags(oldFlags);
    } else if(column == TrackItem::On) {                 /* ON? */
        if(itm->on() == tr("on")) {
            itm->setOn(tr("mute"));
            itm->setBackgroundColor(TrackItem::On,QColor("#a52a2a"));
            itm->setForeground(TrackItem::On,QColor(Qt::white));
            updateTrackOn();
        } else if(itm->on() == tr("mute")) {
            itm->setOn(tr("solo"));
            item->setBackgroundColor(TrackItem::On,QColor(Qt::darkBlue));
            updateTrackOn();
        } else {
            itm->setOn(tr("on"));
            itm->setBackgroundColor(TrackItem::On,QColor(Qt::white));
            itm->setForeground(TrackItem::On,QColor(Qt::black));
            updateTrackOn();
        }
        this->resizeColumnToContents(TrackItem::On);
    } else if(column == TrackItem::Inst) {                  /* INSTRUMENTS */
        if(itm->voice() == 9) { return; } // drums, don't change
        SelectInstrument* ins = new SelectInstrument(this);
        ins->setModal(true);
        ins->setInsName(itm->inst());
        if(ins->exec() == QDialog::Accepted) {
            itm->setInst(ins->insName());
            int insNum = ins->insNum();
            bool didChange = false;
            foreach(QMidiEvent*e,midiFile->eventsForTrack(itm->track())) {
                if (e->type() == QMidiEvent::ProgramChange) {
                    didChange = (e->number() != insNum) || didChange;
                    e->setNumber(insNum);
                }
            }
            MainWind::midiOut->setInstr(itm->voice(),ins->insNum());
            if(didChange) { emit somethingChanged(); }
        }
    }
    VirtualPiano::voiceToUse = itm->voice();
}

void TracksEdit::tracksEdit_itemChanged(QTreeWidgetItem* item, int column)
{
    if(ignoreEvents) { return; }

    TrackItem* itm = static_cast<TrackItem*>(item);
    if(column == TrackItem::Name)
    {
        foreach(QMidiEvent* e, midiFile->eventsForTrack(itm->track()))
        {
            if((e->type() == QMidiEvent::Meta) &&
               (e->number() == 0x03) &&
               (e->data() != itm->name().toLatin1()))
            {
                e->setData(itm->name().toLatin1());
                emit somethingChanged();
                return;
            }
        }
        /* Due to the `return` above, if we're here, there IS no meta event
         * for the track name. So we have to create one. */
        midiFile->createMetaEvent(itm->track(),0,0x03,itm->name().toLatin1());
    }
}

void TracksEdit::tracksEdit_itemEntered(QTreeWidgetItem *item, int col)
{
    if((col == TrackItem::Name) || (col == TrackItem::Inst)) {
        this->setCursor(QCursor(Qt::PointingHandCursor));
    } else {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}
