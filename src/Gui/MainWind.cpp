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

#include "MainWind.h"
#include "ui_MainWind.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>

#include <QtMidi.h>
#include "Selectors/SelectInstrument.h"
#include "Selectors/SelectOutput.h"
#include "Dialogs/AllEvents.h"
#include "Dialogs/TransposeDlg.h"
#include "Dialogs/AboutDlg.h"

QMap<int,QColor>* MainWind::trackColors;
QMap<int,bool>* MainWind::trackStatus;
SeekSlider* MainWind::playLocSilder;

MainWind::MainWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWind)
{
    appSettings = new QSettings("waddlesplash","ragingmidi");

    ui->setupUi(this);

    // More UI setup
    ui->tracksEdit->init(ui->piano);
    trackColors = ui->tracksEdit->trackColors();
    trackStatus = ui->tracksEdit->trackStatus();

    ui->playToolbar->insertWidget(ui->actionTEMP,ui->songPosSlider);
    ui->playToolbar->removeAction(ui->actionTEMP);
    playLocSilder = ui->songPosSlider;

    // Variables
    midiFile = 0;
    player = 0;

    this->show();
    ui->piano->setMaximumHeight(ui->piano->height());

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

    ui->tracksEdit->setupTracks(midiFile);
    ui->songPosSlider->setValue(0);
    ui->songPosSlider->setMaximum(midiFile->events().last()->tick());

    // do the real work
    ui->pianoRoll->initEditor(midiFile);
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

void MainWind::on_actionTranspose_triggered()
{
    if(player) { on_actionStop_triggered(); }
    TransposeDlg t(this, ui->tracksEdit);
    if(t.exec() == QDialog::Accepted) {
        QList<int> tracks = t.tracksToTranspose();
        int steps = t.transposeSteps();
        if(steps == 0) { return; }

        ui->statusBar->showMessage(tr("Transposing..."));
        this->setEnabled(false);
        foreach(int track,tracks) {
            QList<QtMidiEvent*> evn = midiFile->eventsForTrack(track);
            foreach(QtMidiEvent* e, evn) {
                if(e->isNoteEvent()) {
                    int n = e->note()+steps;
                    if(n < 0) { n = 0; }
                    if(n > 127) { n = 127; }
                    e->setNote(n);
                }
            }
        }
        ui->pianoRoll->initEditor(midiFile);
        this->setEnabled(true);
        ui->statusBar->clearMessage();
    }
}

void MainWind::on_actionTrackAdd_triggered()
{

}
void MainWind::on_actionTrackRemove_triggered()
{
    ui->tracksEdit->deleteCurTrack();
    ui->pianoRoll->initEditor(midiFile);
}

void MainWind::on_actionPlay_triggered()
{
    if(!midiFile) { return; }
    if(player) { return; }

    player = new Player(midiFile,ui->piano,ui->songPosSlider->value());
    player->moveToThread(player);
    connect(player,SIGNAL(finished()),this,SLOT(on_actionStop_triggered()));
    player->start();

    connect(player,SIGNAL(tickChanged(qint32)),
            ui->pianoRoll->initLine(ui->songPosSlider->value()),SLOT(setTick(qint32)));
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
    AboutDlg a(this);
    a.exec();
}
