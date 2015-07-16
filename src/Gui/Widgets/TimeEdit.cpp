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

#include "TimeEdit.h"
#include "ui_TimeEdit.h"

#include <QTime>

TimeEdit::TimeEdit(QWidget* parent)
	: QStackedWidget(parent),
	  ui(new Ui::TimeEdit)
{
	ui->setupUi(this);
	file = 0;
}

TimeEdit::~TimeEdit()
{
	delete ui;
}

void TimeEdit::setMidiFile(QMidiFile* f)
{
	file = f;
	ui->curTimeLbl->setMidiFile(f);
	setTick(0);
}

void TimeEdit::mouseReleaseEvent(QMouseEvent* e)
{
	if (!file) {
		return;
	}
	if (this->currentIndex() == 0) {
		this->setCurrentIndex(1);
		this->setCursor(QCursor(Qt::ArrowCursor));
		e->accept();
	} else {
		QStackedWidget::mouseReleaseEvent(e);
	}
}

void TimeEdit::on_editorWidget_editingFinished()
{
	this->setCurrentIndex(0);
	this->setCursor(QCursor(Qt::PointingHandCursor));
	qint32 tick = this->tick();
	this->setTick(tick, true);
	emit tickChanged(tick);
}

void TimeEdit::setTick(int tick, bool dontUpdateTEW)
{
	ui->curTimeLbl->setTick(tick);
	if (dontUpdateTEW) {
		return;
	}
	ui->editorWidget->setTime(QTime::fromString(ui->curTimeLbl->text(), "m:ss.zzz"));
}
qint32 TimeEdit::tick()
{
	QTime t = ui->editorWidget->time();
	int ms = t.msecsTo(QTime()) * -1;
	return file->tickFromTime(ms / 1000.0);
}
