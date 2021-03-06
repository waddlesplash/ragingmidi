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

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>
#include <QUrl>

namespace Ui
{
class AboutDlg;
}

/*!
 * \brief The about dialog.
 *
 * Shows a dialog that says the current version of Raging MIDI (and revision ID) along with
 *   copyright information.
 */
class AboutDlg : public QDialog
{
	Q_OBJECT

public:
	explicit AboutDlg(QWidget* parent = 0);
	~AboutDlg();

private slots:
	void on_authorsText_anchorClicked(const QUrl& arg1);
	void on_fromRevLbl_linkActivated(const QString& link);
	void on_urlLbl_linkActivated(const QString& link);

private:
	Ui::AboutDlg* ui;
};

#endif // ABOUTDLG_H
