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

#ifndef SELECTOUTPUT_H
#define SELECTOUTPUT_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui
{
class SelectOutput;
}

/*!
 * \brief Shows a list of currently available MIDI output devices and requires the user to select
 * one.
 */
class SelectOutput : public QDialog
{
	Q_OBJECT

public:
	explicit SelectOutput(QWidget* parent = 0);
	~SelectOutput();

	QString midiOutId();

private slots:
	void on_searchLE_textChanged(const QString&);

	void on_midiOutNames_itemClicked();
	void on_midiOutNames_itemDoubleClicked(QTreeWidgetItem*, int);

	void on_refreshBtn_clicked();

private:
	Ui::SelectOutput* ui;
};

#endif // SELECTOUTPUT_H
