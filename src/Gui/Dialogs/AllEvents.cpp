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

#include "AllEvents.h"
#include "ui_AllEvents.h"

#include "../Selectors/SelectInstrument.h"

#include <QInputDialog>
#include <QProgressDialog>
#include <QMessageBox>

void GuiMidiEvent::init(QtMidiEvent *e, SelectInstrument* ins)
{
    setText(0,QString::number(e->tick())); // Tick
    setText(1,QString::number(e->voice())); // Voice
    setText(2,QString::number(e->note())); // Note
    setText(3,QString::number(e->velocity())); // Velocity

    switch(e->type()) {
    case QtMidiEvent::NoteOn:
        setText(4,QObject::tr("note on")); break;
    case QtMidiEvent::NoteOff:
        setText(4,QObject::tr("note off")); break;
    case QtMidiEvent::KeyPressure:
        setText(4,QObject::tr("key press.","key pressure")); break;
    case QtMidiEvent::ChannelPressure:
        setText(4,QObject::tr("chan. press.","channel pressure")); break;
    case QtMidiEvent::ControlChange:
        setText(4,QObject::tr("control")); break;
    case QtMidiEvent::ProgramChange:
        setText(4,QObject::tr("program")); break;
    case QtMidiEvent::PitchWheel:
        setText(4,QObject::tr("pitch wheel")); break;
    case QtMidiEvent::Meta:
        setText(4,QObject::tr("meta")); break;
    case QtMidiEvent::Meta_Lyric:
        setText(4,QObject::tr("meta/lyric")); break;
    case QtMidiEvent::Meta_Tempo:
        setText(4,QObject::tr("tempo")); break;
    case QtMidiEvent::Meta_TimeSignature:
        setText(4,QObject::tr("meta/time sig.","meta/time signature")); break;
    case QtMidiEvent::SysEx:
        setText(4,QObject::tr("sys. ex.","system exclusive")); break;
    default: break;
    }

    if(e->type() == QtMidiEvent::Meta_Tempo) {
        setText(5,QString::number(e->tempo()));
    } else if(e->type() == QtMidiEvent::ControlChange) {
        ins->setInsNum(e->number());
        setText(5,QString("%1 (%2)").arg(e->number()).arg(ins->insName()));
    } else {
        setText(5,QString::fromAscii(e->data()));
    }
}

bool GuiMidiEvent::operator<(const QTreeWidgetItem &other) const
{
    int column = treeWidget()->sortColumn();
    if(column != 0) { return QTreeWidgetItem::operator<(other); }
    return text(column).toInt() < other.text(column).toInt();
}

AllEvents::AllEvents(QWidget *parent, QtMidiFile* f) :
    QDialog(parent), ui(new Ui::AllEvents)
{
    ui->setupUi(this);
    if(!f) { return; }

    SelectInstrument ins;
    QList<QtMidiEvent*> events = f->events();

    QProgressDialog dialog(parent);
    dialog.show();
    dialog.setWindowTitle(tr("Loading..."));
    dialog.setLabelText(tr("Setting up \"All MIDI Events\" dialog..."));
    dialog.setMaximum(events.size());
    dialog.repaint();

    GuiMidiEvent* mI;
    for(int i = 0;i < events.size();i++) {
        dialog.setValue(i);
        mI = new GuiMidiEvent(ui->eventsList);
        mI->init(events.at(i),&ins);
    }

    ui->eventsList->setSortingEnabled(true);
    ui->eventsList->sortItems(0,Qt::AscendingOrder);
    minColSize();

    dialog.hide();
}

AllEvents::~AllEvents()
{
    delete ui;
}

void AllEvents::minColSize()
{
    for(int i = 0; i < ui->eventsList->columnCount(); i++)
    { ui->eventsList->resizeColumnToContents(i); }
}

void AllEvents::on_delCertEvBtn_clicked()
{
    QStringList items;
    items.append(tr("pitch wheel"));
    QString result = QInputDialog::getItem(this,tr("Delete certain events"),
                                           tr("Type of events to delete:"),
                                           items,0,false);
    int res = QMessageBox::warning(this,tr("Warning!"),
                                   tr("Are you SURE you want to delete these events?<br>This CANNOT be undone!"),
                                   QMessageBox::Ok,QMessageBox::Cancel);
    if(res == QMessageBox::Ok) {
        QMessageBox::information(this,"","TODO: actually DELETE events!!");
    }
}
