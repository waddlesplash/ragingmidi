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

#include "AllEvents.h"
#include "ui_AllEvents.h"

void GuiMidiEvent::init(QtMidiEvent *e)
{
    setText(0,QString::number(e->tick())); // Tick
    setText(1,QString::number(e->voice())); // Voice
    setText(2,QString::number(e->note())); // Note
    setText(3,QString::number(e->velocity())); // Velocity
    setText(4,"[TODO]"); // TODO: Length

    switch(e->type()) {
    case QtMidiEvent::NoteOn:
        setText(5,QObject::tr("note on")); break;
    case QtMidiEvent::NoteOff:
        setText(5,QObject::tr("note off")); break;
    case QtMidiEvent::KeyPressure:
        setText(5,QObject::tr("key press.","key pressure")); break;
    case QtMidiEvent::ChannelPressure:
        setText(5,QObject::tr("chan. press.","channel pressure")); break;
    case QtMidiEvent::ControlChange:
        setText(5,QObject::tr("control")); break;
    case QtMidiEvent::ProgramChange:
        setText(5,QObject::tr("program")); break;
    case QtMidiEvent::PitchWheel:
        setText(5,QObject::tr("pitch wheel")); break;
    case QtMidiEvent::Meta:
        setText(5,QObject::tr("meta")); break;
    case QtMidiEvent::Meta_Lyric:
        setText(5,QObject::tr("meta/lyric")); break;
    case QtMidiEvent::Meta_Tempo:
        setText(5,QObject::tr("meta/tempo")); break;
    case QtMidiEvent::Meta_TimeSignature:
        setText(5,QObject::tr("meta/time sign.","meta/time signature")); break;
    case QtMidiEvent::SysEx:
        setText(5,QObject::tr("sys. ex.","system exclusive")); break;
    default: break;
    }

    setText(6,e->data());
}

AllEvents::AllEvents(QWidget *parent, QtMidiFile* f) :
    QDialog(parent), ui(new Ui::AllEvents)
{
    ui->setupUi(this);
    ui->groupBox->hide();
    if(!f) { return; }

    GuiMidiEvent* i;
    foreach(QtMidiEvent*e, f->events()) {
        i = new GuiMidiEvent(ui->treeWidget);
        i->init(e);
    }

    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->sortItems(0,Qt::AscendingOrder);
}

AllEvents::~AllEvents()
{
    delete ui;
}
