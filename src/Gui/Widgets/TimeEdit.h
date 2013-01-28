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

#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QStackedWidget>
#include <QMouseEvent>
#include <QLabel>

#include <QMidiFile.h>

/****h* RagingMidi/TimeEdit.h
 * PURPOSE
 *   Allows the user to view and edit the
 *   current time position the file is at.
 ******
 */

namespace Ui {
class TimeEdit;
}

/****c* TimeEdit.h/TimeEdit
 * SYNOPSIS
 */
class TimeEdit : public QStackedWidget
/**
 * DESCRIPTION
 *   Provides the time editor/viewer.
 ******
 */
{
    Q_OBJECT
    
public:
    explicit TimeEdit(QWidget *parent = 0);
    ~TimeEdit();

    inline void setMidiFile(QMidiFile* f) { file = f; setTick(0); }

    /* 2nd arg is "don't update Time Editor widget */
    void setTick(qint32 tick, bool dontUpdateTEW = false);
    qint32 tick();

signals:
    void tickChanged(int tick);

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    
private slots:
    void on_editorWidget_editingFinished();

private:
    Ui::TimeEdit *ui;
    QMidiFile* file;
};

#endif // TIMEEDIT_H
