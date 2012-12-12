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
