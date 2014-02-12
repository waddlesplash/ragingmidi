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
#include <QFile>

#include "Selectors/SelectInstrument.h"
#include "Selectors/SelectOutput.h"
#include "Dialogs/AllEvents.h"
#include "Dialogs/TransposeDlg.h"
#include "Dialogs/AboutDlg.h"
#include "Dialogs/PrefsDlg.h"

QMidiOut* MainWind::midiOut;
QMap<int,QColor>* MainWind::trackColors;
QMap<int,bool>* MainWind::trackStatus;
Settings* MainWind::settings;

MainWind::MainWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWind)
{
    // Important stuff
    settings = new Settings(); // This first because some UI stuff calls it
    ui->setupUi(this);
    ui->pianoRoll->init(ui->pianoRollControls, ui->controlsLayout);

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

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->playToolbar->addWidget(spacer);
    ui->playToolbar->addSeparator();
    ui->playToolbar->addWidget(ui->timeEdit);

    // Variables
    midiFile = 0;
    player = 0;
    midiFileLoc = "";

    // Connections
    connect(ui->tracksEdit,SIGNAL(somethingChanged()),this,SLOT(somethingChanged()));
    connect(ui->timeEdit,SIGNAL(tickChanged(int)),ui->songPosSlider,SLOT(setValue(int)));
    connect(ui->songPosSlider,SIGNAL(valueChanged(int)),ui->timeEdit,SLOT(setTick(int)));
    connect(ui->pianoRoll,SIGNAL(somethingChanged()),this,SLOT(somethingChanged()));

    // Final UI setup
    this->show();
    ui->piano->setMaximumHeight(ui->piano->height());

    // Open MIDI out
    midiOut = new QMidiOut(this);
#ifdef Q_OS_WIN
    midiOut->connect("0"); /* Microsoft GS Wavetable Synth */
#else
    SelectOutput selOut(this);
    if(selOut.exec() == QDialog::Accepted) {
        midiOut->connect(selOut.midiOutId());
    } else {
        initOK = false;
        return;
    }
#endif

    // Load file if specified on the commandline
    foreach(QString arg, QApplication::arguments()) {
        if(QFile::exists(arg)) {
            if(openMidiFile(arg)) { break; }
        }
    }

    initOK = true;
}

MainWind::~MainWind()
{
    delete ui;
    delete settings;
    if(initOK) { midiOut->disconnect(); }
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
    if(player) { on_actionStop_triggered(); }
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
    settings->save();
}

void MainWind::somethingChanged()
{
    this->setWindowModified(true);
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

    QString f = QFileDialog::getOpenFileName(this,tr("Open file..."),
                                             settings->getFileDlgLoc(),
                                             tr("MIDI files (*.mid *.midi)"));
    if(!f.isEmpty()) { openMidiFile(f); }
    else { return; }
}
bool MainWind::openMidiFile(QString filename)
{
    QMidiFile* newMidiFile = new QMidiFile();
    if(!newMidiFile->load(filename)) {
        return false;
    }

    if(player) { on_actionStop_triggered(); }
    if(midiFile) { delete midiFile; }
    midiFile = newMidiFile;
    this->setWindowModified(false);

    QMidiFile* f = midiFile->oneTrackPerVoice();
    if(f != 0) {
        midiFile = f;
        this->setWindowModified(true);
    }

    ui->tracksEdit->setupTracks(midiFile,ui->songPosSlider);
    ui->songPosSlider->setValue(0);
    ui->timeEdit->setMidiFile(midiFile);
    ui->songPosSlider->setMaximum(midiFile->events()->last()->tick());

    // do the real work
    ui->pianoRoll->initEditor(midiFile);

    // update stuff
    QFileInfo i(filename);
    settings->setFileDlgLoc(i.absoluteDir().path());
    midiFileLoc = filename;
    this->setWindowTitle(tr("%1[*] - Raging MIDI","%1 = filename").arg(i.fileName()));
    return true;
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
    QString f = QFileDialog::getSaveFileName(this,tr("Save file..."),
                                             settings->getFileDlgLoc(),
                                             tr("MIDI files (*.mid *.midi)"));
    if(!f.isEmpty()) { midiFile->save(f); }
    else { return; }

    QFileInfo i(f);
    settings->setFileDlgLoc(i.absoluteDir().path());
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
            QList<QMidiEvent*> evn = midiFile->eventsForTrack(track);
            foreach(QMidiEvent* e, evn) {
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
void MainWind::on_actionPreferences_triggered()
{
    PrefsDlg p;
    p.exec();
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

    player = new Player(midiFile,ui->songPosSlider->value());
    player->moveToThread(player);
    connect(player,SIGNAL(finished()),this,SLOT(on_actionStop_triggered()));
    player->start();

    connect(player,SIGNAL(tickChanged(qint32)),
            ui->pianoRoll->initLine(ui->songPosSlider->value()),SLOT(setTick(qint32)));
    connect(player,SIGNAL(tickChanged(qint32)),ui->songPosSlider,SLOT(setValue(qint32)));

    connect(player,SIGNAL(addTrackColor(int,int)),ui->piano,SLOT(addTrackColor(int,int)));
    connect(player,SIGNAL(removeTrackColor(int,int)),ui->piano,SLOT(removeTrackColor(int,int)));
    connect(player,SIGNAL(clearTrackColors(int)),ui->piano,SLOT(clearTrackColors(int)));
}
void MainWind::on_actionStop_triggered()
{
    if(!player) { return; }

    player->stop();
    player->wait();
    delete player;
    player = 0;
    midiOut->stopAll();
    ui->piano->clearTrackColors();
    ui->pianoRoll->deleteLine();
}
void MainWind::on_actionRewind_triggered()
{
    if(player) { on_actionStop_triggered(); }
    ui->songPosSlider->setValue(0);
}

void MainWind::on_songPosSlider_sliderMoveFinished(int value)
{
    if(!player) { return; }
    on_actionStop_triggered();
    ui->songPosSlider->setValue(value);
    QApplication::processEvents(); // otherwise this won't work
    on_actionPlay_triggered();
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
        midiOut->disconnect();
        midiOut->connect(selOut.midiOutId());
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

void MainWind::on_actionQuit_triggered()
{
    QApplication::exit();
}
