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

#ifndef PIANOROLL_H
#define PIANOROLL_H

#include <QGraphicsView>
#include <QToolBar>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QGridLayout>
#include <QPointF>

#include <QMidiFile.h>

/*!
 * \file PianoRoll.h
 * \brief Events and painting for the "PianoRoll" widget which displays a scrolling piano roll of
 * the notes on tracks.
 */

namespace Ui
{
class PianoRoll;
}

/*!
 * \brief The scrolling line that shows the current play location.
 */
class PianoRollLine : public QObject, QGraphicsRectItem
{
	Q_OBJECT
public:
	PianoRollLine(QObject* parent = 0);

public slots:
	void setTick(qint32 tick);

private:
	qint32 oldTick;
	QGraphicsView* p;
};

/*!
 * \brief Widget that handles the display and the management of PianoRollEvent[s].
 */
class PianoRoll : public QGraphicsView
{
	Q_OBJECT

public:
	static bool canMoveItems;
	static PianoRoll* me;

	explicit PianoRoll(QWidget* parent = 0);
	~PianoRoll();

	void init(QToolBar* controlsToolbar);
	void initEditor(QMidiFile* f);
	PianoRollLine* initLine(qint32 tick);
	void deleteLine();

	void finishMove();

protected:
	void wheelEvent(QWheelEvent* event);
	void drawBackground(QPainter* painter, const QRectF& rect);

private slots:
	void handleChange(QString a);
	void handleNoteChange();

	void on_actionMove_toggled(bool checked);
	void on_actionZoom100_triggered();

signals:
	void somethingChanged();

private:
	Ui::PianoRoll* ui;
	QMidiFile* midiFile;
	PianoRollLine* line;
	QBrush darker, lighter1, lighter2;

	void zoom(qreal factor, QPointF centerPoint);
};

/*!
 * \brief Rectangle that shows the location of one pair of NoteOn and NoteOff events in the
 * piano roll.
 *
 * NOTE: before changing this class's definition, PLEASE
 * read the comments at PianoRoll::finishMove().
 */
class PianoRollEvent : public QObject, QGraphicsRectItem
{
	Q_OBJECT

public:
	PianoRollEvent(QObject* p = 0);

	inline void setSize(qreal x, qreal y, qreal w, qreal h)
	{
		this->setPos(x, y);
		this->setRect(0, 0, w, h);
	}

	inline void setNoteOnAndOff(QMidiEvent* noteOn, QMidiEvent* noteOff)
	{
		myNoteOn = noteOn;
		myNoteOff = noteOff;
	}

	inline void setColor(QColor c) { myColor = c; }

	void finishMove();

signals:
	void somethingChanged();

protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

	void mousePressEvent(QGraphicsSceneMouseEvent* e);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

private:
	QMidiEvent* myNoteOn;
	QMidiEvent* myNoteOff;
	QColor myColor;
};

#endif // PIANOROLL_H
