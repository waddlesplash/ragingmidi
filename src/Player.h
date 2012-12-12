#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

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
        doPause = false;
        doStop = false;
        sTick = tick;
    }
    ~Player()
    {}

    inline void pause() { doPause = true; }
    void resume();
    inline bool isPaused() { return doPause; }
    inline void stop() { doStop = true; }

protected:
    void run();

signals:
    void tickChanged(qint32 tick);

private:
    QtMidiEvent* e;
    QtMidiFile* f;
    VirtualPiano* piano;

    /* No mutex needed here, as these
     * only use one byte. However,
     * QWaitCondition *requires* a mutex. */
    bool doPause;
    QWaitCondition pauseCond;
    QMutex pauseSync;
    bool doStop;

    qint32 sTick;

private slots:
    void handleEvent();
};

#endif // PLAYER_H
