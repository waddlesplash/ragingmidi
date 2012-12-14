#include "MainWind.h"
#include "ui_MainWind.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>

#include <QtMidi.h>
#include "Selectors/SelectInstrument.h"
#include "Selectors/SelectOutput.h"
#include "AllEvents.h"

QMap<int,QColor> MainWind::trackColors;
QMap<int,bool> MainWind::trackStatus;
SeekSlider* MainWind::playLocSilder;

MainWind::MainWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWind)
{
    appSettings = new QSettings("waddlesplash","ragingmidi");

    ui->setupUi(this);

    // More UI setup
    ui->tracksEdit->hideColumn(8);
    ui->playToolbar->insertWidget(ui->actionTEMP,ui->songPosSlider);
    ui->playToolbar->removeAction(ui->actionTEMP);
    playLocSilder = ui->songPosSlider;

    // Variables
    midiFile = 0;
    player = 0;

    this->show();
    SelectOutput selOut(this);
    if(selOut.exec() == QDialog::Accepted)
    { QtMidi::initMidiOut(selOut.midiOutId()); }
    else
    { QApplication::exit(); }
}

MainWind::~MainWind()
{
    delete ui;
    QtMidi::closeMidiOut();
}

void MainWind::on_actionOpen_triggered()
{
    QVariant s = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString f = QFileDialog::getOpenFileName(this,tr("Open file..."),
                                             appSettings->value("LastFileDlgLoc",s).toString(),
                                             tr("MIDI files (*.mid *.midi)"));
    if(!f.isEmpty()) { openMidiFile(f); }
    else { return; }

    QFileInfo i(f);
    appSettings->setValue("LastFileDlgLoc",i.absoluteDir().path());
}

void MainWind::openMidiFile(QString filename)
{
    if(player) { on_actionStop_triggered(); }
    if(midiFile) { delete midiFile; }
    midiFile = new QtMidiFile();
    midiFile->load(filename);

    ui->tracksEdit->clear();

    QStringList colorNames = QColor::colorNames();
    colorNames.removeOne("black");
    colorNames.removeOne("white");
    colorNames.removeOne("azure");
    colorNames.removeOne("aliceblue");
    if(colorNames.size() < midiFile->tracks().size())
    { /* TODO: Not enough colors! */ return; }

    foreach(int curTrack,midiFile->tracks())
    {
        QTreeWidgetItem* i = new QTreeWidgetItem(ui->tracksEdit);
        i->setBackgroundColor(0,QColor(colorNames.at(curTrack)));
        trackColors.insert(curTrack,QColor(colorNames.at(curTrack)));

        i->setText(1,tr("Instrument")); // Type
        i->setText(2,tr("on")); // On?
        i->setText(3,"<automatic>"); // Device
        i->setText(8,QString::number(curTrack)); // Track

        bool didInstr = false, didVoice = false, didMeta = false;
        foreach(QtMidiEvent*e,midiFile->eventsForTrack(curTrack))
        {
            if(!didVoice && e->type() == QtMidiEvent::NoteOn)
            {
                i->setText(4,QString::number(e->voice()+1));
                didVoice = true;
            }
            else if(!didInstr && e->type() == QtMidiEvent::ProgramChange)
            {
                int instr = e->number();
                SelectInstrument sel(this);
                sel.setInsNum(instr);
                i->setText(5,sel.insName());
                QtMidi::outSetInstr(e->voice(),e->number());
                didInstr = true;
            }
            else if(!didMeta && (e->type() == QtMidiEvent::Meta) &&
                    (e->number() == 0x03))
            { i->setText(0,e->data()); didMeta = true; } // Name

            if(didInstr && didVoice && didMeta) { break; }
        }

        if(!didInstr)
        { i->setText(5,tr("(no instrument)")); }
    }
    ui->songPosSlider->setMaximum(midiFile->events().last()->tick());

    // do the real work
    ui->pianoRoll->initEditor(midiFile);
    updateTrackOn();
}


void MainWind::on_actionSave_triggered()
{
    QVariant s = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString f = QFileDialog::getSaveFileName(this,tr("Save file..."),
                                             appSettings->value("LastFileDlgLoc",s).toString(),
                                             tr("MIDI files (*.mid *.midi)"));
    if(!f.isEmpty()) { midiFile->save(f); }
    else { return; }

    QFileInfo i(f);
    appSettings->setValue("LastFileDlgLoc",i.absoluteDir().path());
}

void MainWind::updateTrackOn()
{
    bool isOneSolo = false;
    int soloTrack = 0;
    QTreeWidgetItem* itm;
    for(int i = 0;i<ui->tracksEdit->topLevelItemCount();i++)
    {
        itm = ui->tracksEdit->topLevelItem(i);
        if((itm->text(2) == tr("solo")) && !isOneSolo)
        {
            isOneSolo = true;
            soloTrack = itm->text(8).toInt();
            ui->piano->clearTrackColors(itm->text(8).toInt());
        }
        bool on = (itm->text(2) == tr("on"));
        trackStatus.insert(itm->text(8).toInt(),on);
        if(!on) { QtMidi::outStopAll(itm->text(4).toInt()-1); }
    }

    if(!isOneSolo) { return; }

    foreach(int i,trackStatus.keys())
    {
        if(i == soloTrack)
        {
            trackStatus.insert(i,true);
            continue;
        }
        trackStatus.insert(i,false);
    }
    QtMidi::outStopAll();
    ui->piano->clearTrackColors();
}

void MainWind::on_tracksEdit_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column == 0)
    {
        Qt::ItemFlags oldFlags = item->flags();
        item->setFlags(oldFlags | Qt::ItemIsEditable);
        ui->tracksEdit->editItem(item,column);
        item->setFlags(oldFlags);
    }
    else if(column == 5)
    {
        SelectInstrument* ins = new SelectInstrument(this);
        ins->setModal(true);
        ins->setInsName(item->text(5));
        if(ins->exec() == QDialog::Accepted)
        {
            item->setText(5,ins->insName());
            foreach(QtMidiEvent*e,midiFile->eventsForTrack(item->text(8).toInt()))
            {
                if (e->type() == QtMidiEvent::ProgramChange)
                { e->setNumber(ins->insNum()); }
            }
            QtMidi::outSetInstr(item->text(4).toInt()-1,ins->insNum());
        }
    }
}

void MainWind::on_tracksEdit_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 2)
    {
        if(item->text(2) == tr("on"))
        {
            item->setText(2,tr("mute"));
            item->setBackgroundColor(2,QColor("#a52a2a"));
            item->setForeground(2,QColor(Qt::white));
            updateTrackOn();
        }
        else if(item->text(2) == tr("mute"))
        {
            item->setText(2,tr("solo"));
            item->setBackgroundColor(2,QColor(Qt::darkBlue));
            updateTrackOn();
        }
        else
        {
            item->setText(2,tr("on"));
            item->setBackgroundColor(2,QColor(Qt::white));
            item->setForeground(2,QColor(Qt::black));
            updateTrackOn();
        }
    }
    VirtualPiano::voiceToUse = item->text(4).toInt()-1;
}

void MainWind::on_actionTrackAdd_triggered()
{

}

void MainWind::on_actionPlay_triggered()
{
    if(!midiFile) { return; }

    player = new Player(midiFile,ui->piano,ui->songPosSlider->value());
    player->moveToThread(player);
    connect(player,SIGNAL(finished()),this,SLOT(on_actionStop_triggered()));
    player->start();

    connect(player,SIGNAL(tickChanged(qint32)),
            ui->pianoRoll->initLine(),SLOT(setTick(qint32)));
}
void MainWind::on_actionStop_triggered()
{
    if(!player) { return; }

    player->stop();
    player->wait();
    delete player;
    player = 0;
    QtMidi::outStopAll();
    ui->piano->clearTrackColors();
    ui->pianoRoll->deleteLine();
}
void MainWind::on_actionRewind_triggered()
{
    if(player) { on_actionStop_triggered(); }
    ui->songPosSlider->setValue(0);
}

void MainWind::on_songPosSlider_sliderReleased()
{/* // FIXME: doesnt work yet... just stops and does not start, and does not seek....
    if(!player) { return; }
    on_actionStop_triggered();
    on_actionPlay_triggered();
*/}

void MainWind::on_actionViewAllEvents_triggered()
{
    AllEvents e(this,midiFile);
    e.exec();
}

void MainWind::on_actionAbout_Raging_Midi_triggered()
{
    // TODO: about box
}
