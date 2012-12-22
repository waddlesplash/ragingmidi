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

#ifndef SELECTINSTRUMENT_H
#define SELECTINSTRUMENT_H

/****h* RagingMidi/SelectInstrument.h
 * PURPOSE
 *   Creates a dialog with a list of all the
 *   valid MIDI instruments and allows the user
 *   to select one.
 ******
 */

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class SelectInstrument;
}


/****c* SelectInstrument.h/SelectInstrument
 * SYNOPSIS
 */
class SelectInstrument : public QDialog
/********/
{
    Q_OBJECT
    
public:
    explicit SelectInstrument(QWidget *parent = 0);
    ~SelectInstrument();

    QString insName();
    void setInsName(QString name);
    int insNum();
    void setInsNum(int num);
    
private slots:
    void on_searchLE_textChanged(const QString&);

    void on_midiInstr_itemClicked();
    void on_midiInstr_itemDoubleClicked(QTreeWidgetItem*, int);

private:
    Ui::SelectInstrument *ui;
};

#endif // SELECTINSTRUMENT_H
