#ifndef PIANOROLL_H
#define PIANOROLL_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QPointF>
#include "QtMidiFile.h"

namespace Ui {
class PianoRoll;
}

class PianoRollLine : public QObject, QGraphicsRectItem
{
    Q_OBJECT
public:
    PianoRollLine(QObject* parent, QGraphicsScene* s);
    inline ~PianoRollLine() {}

public slots:
    void setTick(qint32 tick);
};

class PianoRollEvent : public QGraphicsRectItem
{
public:
    inline PianoRollEvent() : QGraphicsRectItem(0)
    {
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsMovable);
    }
    inline ~PianoRollEvent() {}

    inline void setSize(qreal x, qreal y, qreal w, qreal h)
    { this->setPos(x,y); this->setRect(0,0,w,h); }

    inline void setNoteOnAndOff(QtMidiEvent* noteOn, QtMidiEvent* noteOff)
    { myNoteOn = noteOn; myNoteOff = noteOff; }

    inline void setColor(QColor c) { myColor = c; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

private:
    QtMidiEvent* myNoteOn;
    QtMidiEvent* myNoteOff;
    QColor myColor;
};

class PianoRoll : public QGraphicsView
{
    Q_OBJECT
    
public:
    explicit PianoRoll(QWidget *parent = 0);
    ~PianoRoll();

    void initEditor(QtMidiFile* f);

    PianoRollLine* initLine();
    void deleteLine();

protected:
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);
    
private:
    Ui::PianoRoll *ui;
    QtMidiFile* file;

    PianoRollLine* line;

    void zoom(qreal factor, QPointF centerPoint);
};

#endif // PianoRoll_H
