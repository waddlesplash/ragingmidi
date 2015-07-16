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

#ifndef PREFSDLG_H
#define PREFSDLG_H

/*!
 * \file PrefsDlg.h
 * \brief Allows the user to configure various settings about how Raging MIDI behaves.
 */

#include <QDialog>
#include <QAbstractButton>

namespace Ui
{
class PrefsDlg;
}

/*!
 * \brief Provides the preferences editor dialog.
 */
class PrefsDlg : public QDialog
{
	Q_OBJECT

public:
	explicit PrefsDlg(QWidget* parent = 0);
	~PrefsDlg();

private slots:
	void on_buttonBox_clicked(QAbstractButton* button);

private:
	Ui::PrefsDlg* ui;

	void applyPrefs();
};

#endif // PREFSDLG_H
