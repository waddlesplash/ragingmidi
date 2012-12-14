#ifndef PLAYER_H
#define PLAYER_H

#include <QThread>

#include <QtMidiFile.h>
#include "Gui/Widgets/VirtualPiano.h"

class Player : public QThread
{
    Q_OBJECT
public:
    inline Player(QtMidiFile* fil, VirtualPiano* p, qint32 tick = 0)
        : QThread(0)
    {
        f = fil;
        piano = p;
        doStop = false;
        sTick = tick;
    }
    inline ~Player() {}

    inline void stop() { doStop = true; }

protected:
    void run();

signals:
    void tickChanged(qint32 tick);

private:
    QtMidiEvent* e;
    QtMidiFile* f;
    VirtualPiano* piano;

    bool doStop;

    qint32 sTick;

private slots:
    void handleEvent();
};

#endif // PLAYER_H
