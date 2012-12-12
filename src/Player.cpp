#include <QDateTime>
#include <QtMidi.h>

#include "Player.h"
#include "Gui/MainWind.h"

void Player::resume()
{
    pauseSync.lock();
    doPause = false;
    pauseCond.wakeAll();
    pauseSync.unlock();
}

void Player::handleEvent()
{
    if(MainWind::trackStatus.value(e->track(),true) == false)
    { return; }

    if (e->type() == QtMidiEvent::SysEx) { // TODO: sysex
    } else {
        qint32 message = e->message();
        QtMidi::outSendMsg(message);
    }

    // Update the piano, pianoroll, and slider
    if(e->type() == QtMidiEvent::NoteOn) {
        piano->key(e->note())->addTrackColor(e->track());
        if(MainWind::playLocSilder->doUpdate()) {
            MainWind::playLocSilder->setValue(e->tick());
        }
        emit tickChanged(e->tick());
    } else if(e->type() == QtMidiEvent::NoteOff) {
        piano->key(e->note())->removeTrackColor(e->track());
    } else if(e->type() == QtMidiEvent::ControlChange) {
        piano->clearTrackColors(e->track());
    }
}

void Player::run()
{
    qint64 start_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
    //if(sTick > 0) { start_time -= (f->timeFromTick(sTick)*1000); }
    foreach(e,f->events()) {
        if(e->isNoteEvent() && (e->tick() < sTick)) { continue; }

        if(e->type() != QtMidiEvent::Meta) {
            qint64 event_time = f->timeFromTick(e->tick()-sTick) * 1000;
            qint64 current_time = QDateTime::currentDateTime().toMSecsSinceEpoch();

            qint32 waitTime = event_time - (current_time - start_time);
            if(waitTime > 0) {
                msleep(waitTime);
            }
            handleEvent();
        }
        pauseSync.lock();
        if(doPause) {
            qint64 offset = start_time - QDateTime::currentDateTime().toMSecsSinceEpoch();
            pauseCond.wait(&pauseSync);
            start_time = QDateTime::currentDateTime().toMSecsSinceEpoch()+offset;
        }
        pauseSync.unlock();

        if(doStop)
        { return; }
    }
}
