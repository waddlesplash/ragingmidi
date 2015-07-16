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

#include "TransposeDlg.h"
#include "ui_TransposeDlg.h"

TransposeDlg::TransposeDlg(QWidget* parent, TracksEdit* tracksEdit)
	: QDialog(parent),
	  ui(new Ui::TransposeDlg)
{
	ui->setupUi(this);
	ui->tracksChoose->hideColumn(2);

	foreach (TrackItem* t, tracksEdit->tracks()) {
		if (t->inst() == tr("(no instrument)")) {
			continue;
		}

		QTreeWidgetItem* n = new QTreeWidgetItem(ui->tracksChoose);
		n->setText(0, t->name());
		n->setBackgroundColor(0, t->backgroundColor(TrackItem::Name));
		n->setText(1, t->inst());
		n->setText(2, QString::number(t->track()));
	}
}

TransposeDlg::~TransposeDlg()
{
	delete ui;
}

QList<int> TransposeDlg::tracksToTranspose()
{
	QList<int> ret;
	foreach (QTreeWidgetItem* itm, ui->tracksChoose->selectedItems()) {
		ret.append(itm->text(2).toInt());
	}
	return ret;
}

int TransposeDlg::transposeSteps()
{
	return ui->spinBox->value();
}

void TransposeDlg::on_selectNoneBtn_clicked()
{
	ui->tracksChoose->selectAll();
}

void TransposeDlg::on_selectAllBtn_clicked()
{
	ui->tracksChoose->clearSelection();
}
