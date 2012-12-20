#include "TracksEdit.h"
#include "ui_TracksEdit.h"

#include <QColor>
#include <QtMidi.h>

#include "../Selectors/SelectInstrument.h"

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
}

TracksEdit::~TracksEdit()
{
    delete ui;
}

TrackItem* TracksEdit::createTrack(int trackNum)
{
    TrackItem* ret = new TrackItem(this,trackNum);
    ret->setBackgroundColor(TrackItem::Name,QColor(colorNames.at(trackNum)));
    myTrackColors.insert(trackNum,QColor(colorNames.at(trackNum)));
    ret->setType(tr("Instrument"));
    ret->setOn(tr("on"));
    ret->setDevice("<automatic>");
    return ret;
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

void TracksEdit::setupTracks(QtMidiFile *f)
{
    midiFile = f;
    this->clear();
    foreach(int curTrack,midiFile->tracks())
    {
        TrackItem* i = this->createTrack(curTrack);

        bool didInstr = false, didVoice = false, didMeta = false;
        foreach(QtMidiEvent* e, midiFile->eventsForTrack(curTrack))
        {
            if(!didVoice && e->type() == QtMidiEvent::NoteOn)
            {
                i->setVoice(e->voice());
                didVoice = true;
            }
            else if(!didInstr && e->type() == QtMidiEvent::ProgramChange)
            {
                int instr = e->number();
                SelectInstrument sel(this);
                sel.setInsNum(instr);
                i->setInst(sel.insName());
                QtMidi::outSetInstr(e->voice(),e->number());
                didInstr = true;
            }
            else if(!didMeta && (e->type() == QtMidiEvent::Meta) &&
                    (e->number() == 0x03))
            { i->setName(e->data()); didMeta = true; } // Name

            if(didInstr && didVoice && didMeta) { break; }
        }

        if(!didInstr)
        { i->setInst(tr("(no instrument)")); }
    }
}

void TracksEdit::updateTrackOn()
{
    bool isOneSolo = false;
    int soloTrack = 0;
    foreach(TrackItem* itm,tracks()) {
        if((itm->on() == tr("solo")) && !isOneSolo) {
            isOneSolo = true;
            soloTrack = itm->track();
            piano->clearTrackColors(itm->track());
        }
        bool on = (itm->on() == tr("on"));
        myTrackStatus.insert(itm->track(),on);
        if(!on) { QtMidi::outStopAll(itm->voice()); }
    }

    if(!isOneSolo) { return; }

    foreach(int i,myTrackStatus.keys()) {
        if(i == soloTrack) {
            myTrackStatus.insert(i,true);
            continue;
        }
        myTrackStatus.insert(i,false);
    }
    QtMidi::outStopAll();
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
        SelectInstrument* ins = new SelectInstrument(this);
        ins->setModal(true);
        ins->setInsName(itm->inst());
        if(ins->exec() == QDialog::Accepted) {
            itm->setInst(ins->insName());
            foreach(QtMidiEvent*e,midiFile->eventsForTrack(itm->track())) {
                if (e->type() == QtMidiEvent::ProgramChange)
                { e->setNumber(ins->insNum()); }
            }
            QtMidi::outSetInstr(itm->voice(),ins->insNum());
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
    }
    VirtualPiano::voiceToUse = itm->voice();
}

