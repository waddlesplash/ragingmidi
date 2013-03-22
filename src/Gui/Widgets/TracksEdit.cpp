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
#include <QMidi.h>

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
    balSL->setMinimum(-50);
    balSL->setMaximum(50);
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

    connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(tracksEdit_itemDoubleClicked(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(tracksEdit_itemClicked(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,SLOT(tracksEdit_itemChanged(QTreeWidgetItem*,int)));

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
    int val = 0x40 + (b * 0x3f)/50; 
    int oldVal = -1;
    foreach(QMidiEvent* e, midiFile->eventsForTrack(trk))
    {
        if((e->type() != QMidiEvent::ControlChange) ||
		   (e->number() != /*Coarse pan */10)) { continue; }
		 
        if(oldVal == -1) { oldVal = e->value(); continue; }
        if(oldVal != e->value()) {
            int ret = QMessageBox::warning(this->parentWidget(),tr("Different Balance!"),
                                           tr("<b>This track already has Balance event.</b><br/>"
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
		   (e->number() != /*Coarse pan */10)) { continue; }
        e->setValue(val);
		bChanged = true;
    }
	if (!bChanged) {
		midiFile->createControlChangeEvent(trk, 0, this->tracks().at(trk)->voice(), /* Coarse Pan */10, val);
	}
    QMidi::outSendMsg((0xb0 + this->tracks().at(trk)->voice())|(10<<8)|(val<<16));
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

void TracksEdit::setupTracks(QMidiFile *f)
{
    ignoreEvents = true;
    midiFile = f;
    this->clear();
    foreach(int curTrack,midiFile->tracks())
    {
        TrackItem* i = this->createTrack(curTrack);

        bool didInstr = false, didVoice = false, didName = false, didVol = false;
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
                QMidi::outSetInstr(e->voice(),e->number());
                didInstr = true;
            }
            else if(!didName && (e->type() == QMidiEvent::Meta) &&
                    (e->number() == 0x03))
            { i->setName(e->data()); didName = true; } // Name

            if(didInstr && didVoice && didName) { break; }
        }

        if(!didInstr)
        { i->setInst(tr("(no instrument)")); }
        if(!didName)
        { i->setName(tr("Track %1","track number").arg(curTrack)); }
    }
    ignoreEvents = false;
    resizeColsToContents();
    myTrackStatus.clear();
    updateTrackOn();
}

void TracksEdit::deleteCurTrack()
{
    TrackItem* i = static_cast<TrackItem*>(this->selectedItems().at(0));
    if(!i) { return; }

    int trackNum = i->track();
    i->~QTreeWidgetItem();
    foreach(QMidiEvent*e,midiFile->eventsForTrack(trackNum))
    {
        midiFile->removeEvent(e);
        delete e;
    }
    midiFile->removeTrack(trackNum);
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
        if(!on) { QMidi::outStopAll(itm->voice()); }
    }

    if(soloTracks.size() < 1) { return; }

    foreach(int i,myTrackStatus.keys()) {
        if(soloTracks.contains(i)) {
            myTrackStatus.insert(i,true);
            continue;
        }
        myTrackStatus.insert(i,false);
    }
    QMidi::outStopAll();
    piano->clearTrackColors();
}

void TracksEdit::tracksEdit_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    TrackItem* itm = static_cast<TrackItem*>(item);
    if(column == TrackItem::Name) {
        Qt::ItemFlags oldFlags = itm->flags();
        itm->setFlags(oldFlags | Qt::ItemIsEditable);
        this->editItem(itm,column);
        itm->setFlags(oldFlags);
    } else if(column == TrackItem::Inst) {
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
            QMidi::outSetInstr(itm->voice(),ins->insNum());
            if(didChange) { emit somethingChanged(); }
        }
    }
}

void TracksEdit::tracksEdit_itemClicked(QTreeWidgetItem *item, int column)
{
    TrackItem* itm = static_cast<TrackItem*>(item);
    if(column == TrackItem::On) {
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
               (e->data() != itm->name().toUtf8()))
            {
                e->setData(itm->name().toUtf8());
                emit somethingChanged();
                return;
            }
        }
    }
}

