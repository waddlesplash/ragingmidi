/*
 * Raging MIDI (https://github.com/waddlesplash/ragingmidi).
 *
 * Copyright (c) 2012 WaddleSplash & contributors (see AUTHORS.txt).
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

#ifndef TRANSPOSEDLG_H
#define TRANSPOSEDLG_H

/****h* RagingMidi/TransposeDlg.h
 * PURPOSE
 *   Shows a dialog that allows the user to
 *   transpose any number of tracks down or up.
 ******
 */

#include <QDialog>
#include <QTreeWidget>
#include <QList>

#include "../Widgets/TracksEdit.h"

namespace Ui {
class TransposeDlg;
}

/****c* TransposeDlg.h/TransposeDlg
 * SYNOPSIS
 */
class TransposeDlg : public QDialog
/********/
{
    Q_OBJECT
    
public:
    explicit TransposeDlg(QWidget *parent = 0, TracksEdit* tracksEdit = 0);
    ~TransposeDlg();

    QList<int> tracksToTranspose();
    int transposeSteps();
    
private slots:
    void on_selectNoneBtn_clicked();
    void on_selectAllBtn_clicked();

private:
    Ui::TransposeDlg *ui;
};

#endif // TRANSPOSEDLG_H
