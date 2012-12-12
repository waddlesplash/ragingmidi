#ifndef VIRTUALPIANO_H
#define VIRTUALPIANO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPaintEvent>
#include <QList>
#include <QMap>
#include <QColor>
#include <QReadWriteLock>

#include <QtMidi.h>

namespace Ui {
class VirtualPiano;
}

class VirtualPianoKey : public QObject, QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit VirtualPianoKey(QObject *parent = 0, int midiKey = 0, QString noteName = "", bool isBlackKey = false);
    inline ~VirtualPianoKey() {}

    inline void setSize(int x, int y, int w, int h)
    { this->setRect(0,0,w,h); this->setPos(x,y); }

    QList<QColor> trackColors();
    void setTrackColors(QList<QColor> newColors);
    void addTrackColor(int track);
    void removeTrackColor(int track);

private:
    bool isBlack;
    bool isHovering;
    bool isClicking;
    int myMidiKey;
    QString myNoteName;

    QList<QColor> myColors;
    QReadWriteLock lock;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);

    inline void hoverEnterEvent(QGraphicsSceneHoverEvent *)
    { isHovering = true; this->scene()->update(x(),y(),rect().x(),rect().y()); }
    inline void hoverLeaveEvent(QGraphicsSceneHoverEvent *)
    { isHovering = false; this->scene()->update(x(),y(),rect().x(),rect().y()); }

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

class VirtualPiano : public QGraphicsView
{
    Q_OBJECT
    
public:
    explicit VirtualPiano(QWidget *parent = 0);
    ~VirtualPiano();

    inline VirtualPianoKey* key(int num) { return keys.value(num); }

    void clearTrackColors();
    void clearTrackColors(int track);

    static int voiceToUse;
    
private:
    Ui::VirtualPiano *ui;
    QMap<int,VirtualPianoKey*> keys;

    int k;
    int curKey;

    void addWhiteKey(QGraphicsScene* scene, QString noteName);
    void addBlackKey(QGraphicsScene* scene, QString noteName);
};

#endif // VIRTUALPIANO_H
