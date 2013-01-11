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

MainWind::MainWind(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWind)
{
    // Important stuff
    ui->setupUi(this);
    appSettings = new QSettings("waddlesplash","ragingmidi");

    // Icon setup
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    ui->actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->actionRewind->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    // UI & Static Var. setup
    ui->tracksEdit->init(ui->piano);
    trackColors = ui->tracksEdit->trackColors();
    trackStatus = ui->tracksEdit->trackStatus();

    ui->playToolbar->insertWidget(ui->actionTEMP,ui->songPosSlider);
    ui->playToolbar->removeAction(ui->actionTEMP);
    playLocSilder = ui->songPosSlider;

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->playToolbar->addWidget(spacer);
    ui->playToolbar->addWidget(ui->timeEdit);

    // Variables
    midiFile = 0;
    player = 0;
    midiFileLoc = "";

    // Connections
    connect(ui->tracksEdit,SIGNAL(somethingChanged()),this,SLOT(somethingChanged()));
    connect(ui->timeEdit,SIGNAL(tickChanged(qint32)),this,SLOT(setSongTick(qint32)));

    // Final UI setup
    this->show();
    ui->piano->setMaximumHeight(ui->piano->height());

    // Open MIDI out
    SelectOutput selOut(this);
    if(selOut.exec() == QDialog::Accepted) {
        QtMidi::initMidiOut(selOut.midiOutId());
    } else {
        initOK = false;
        return;
    }

    // Load file if specified on the commandline
    for(int i = 1;i<argc;i++) {
        QFile f(QString(argv[i]));
        if(f.exists()) { openMidiFile(QString(argv[i])); break; }
    }

    initOK = true;
}

MainWind::~MainWind()
{
    delete ui;
    if(initOK) { QtMidi::closeMidiOut(); }
}

int MainWind::confirmUnsaved()
{
    if(this->isWindowModified()) {
        QMessageBox a(this);
#ifndef Q_OS_MAC
        a.setText("<b>"+tr("You have unsaved changes!")+"</b>");
#else
        a.setText(tr("You have unsaved changes!"));
#endif
        a.setInformativeText(tr("Do you want to save your changes?"));
        a.setIcon(QMessageBox::Warning);
        a.setWindowTitle(tr("Unsaved changes!"));
        a.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        a.setDefaultButton(QMessageBox::Save);
        return a.exec();
    } else {
        return -1;
    }
}

void MainWind::closeEvent(QCloseEvent *e)
{
    if(this->isWindowModified()) {
        switch(confirmUnsaved()) {
        case QMessageBox::Save:
            on_actionSave_triggered();
            e->accept();
            break;
        case QMessageBox::Discard:
            e->accept();
            break;
        case QMessageBox::Cancel:
            e->ignore();
            break;
        }
    } else {
        e->accept();
    }
}

void MainWind::somethingChanged()
{
    this->setWindowModified(true);
}

void MainWind::setSongTick(qint32 tick)
{
    ui->songPosSlider->setValue(tick);
}

void MainWind::on_actionOpen_triggered()
{
    if(this->isWindowModified()) {
        switch(confirmUnsaved()) {
        case QMessageBox::Save:
            on_actionSave_triggered();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
            break;
        }
    }

    QVariant s = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString f = QFileDialog::getOpenFileName(this,tr("Open file..."),
                                             appSettings->value("LastFileDlgLoc",s).toString(),
                                             tr("MIDI files (*.mid *.midi)"));
    if(!f.isEmpty()) { openMidiFile(f); }
    else { return; }
}
void MainWind::openMidiFile(QString filename)
{
    if(player) { on_actionStop_triggered(); }
    if(midiFile) { delete midiFile; }
    midiFile = new QtMidiFile();
    midiFile->load(filename);

    ui->tracksEdit->setupTracks(midiFile);
    ui->songPosSlider->setValue(0);
    ui->timeEdit->setMidiFile(midiFile);
    ui->songPosSlider->setMaximum(midiFile->events().last()->tick());

    // do the real work
    ui->pianoRoll->initEditor(midiFile);

    // update stuff
    QFileInfo i(filename);
    appSettings->setValue("LastFileDlgLoc",i.absoluteDir().path());
    midiFileLoc = filename;
    this->setWindowModified(false);
    this->setWindowTitle(tr("%1[*] - Raging MIDI","%1 = filename").arg(i.fileName()));
}
void MainWind::on_actionSave_triggered()
{
    if(!midiFileLoc.isEmpty())
    {
        midiFile->save(midiFileLoc);
        this->setWindowModified(false);
    }
    else
    {
        on_actionSaveAs_triggered();
    }
}
void MainWind::on_actionSaveAs_triggered()
{
    QVariant s = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString f = QFileDialog::getSaveFileName(this,tr("Save file..."),
                                             appSettings->value("LastFileDlgLoc",s).toString(),
                                             tr("MIDI files (*.mid *.midi)"));
    if(!f.isEmpty()) { midiFile->save(f); }
    else { return; }

    QFileInfo i(f);
    appSettings->setValue("LastFileDlgLoc",i.absoluteDir().path());
    midiFileLoc = f;
    this->setWindowModified(false);
    this->setWindowTitle(tr("%1[*] - Raging MIDI","%1 = filename").arg(i.fileName()));
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
        somethingChanged();
        ui->statusBar->clearMessage();
    }
}

void MainWind::on_actionTrackAdd_triggered()
{

}
void MainWind::on_actionTrackRemove_triggered()
{
    if(!midiFile) { return; }
    ui->tracksEdit->deleteCurTrack();
    ui->pianoRoll->initEditor(midiFile);
    somethingChanged();
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
void MainWind::on_songPosSlider_valueChanged(int value)
{
    ui->timeEdit->setTick(value);
}

void MainWind::on_actionViewAllEvents_triggered()
{
    AllEvents e(this,midiFile);
    e.exec();
}

void MainWind::on_actionDeviceReconnect_triggered()
{
    SelectOutput selOut(this);
    if(selOut.exec() == QDialog::Accepted) {
        QtMidi::closeMidiOut();
        QtMidi::initMidiOut(selOut.midiOutId());
    }
}

void MainWind::on_actionAbout_triggered()
{
    AboutDlg a(this);
    a.exec();
}
void MainWind::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}
