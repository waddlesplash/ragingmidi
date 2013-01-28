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

#include "SelectOutput.h"
#include "ui_SelectOutput.h"

#include <QMidi.h>
#include <QStringList>
#include <QPushButton>

SelectOutput::SelectOutput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectOutput)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    ui->refreshBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    on_refreshBtn_clicked();

    ui->midiOutNames->resizeColumnToContents(0);
    ui->midiOutNames->resizeColumnToContents(1);
}

SelectOutput::~SelectOutput()
{
    delete ui;
}

QString SelectOutput::midiOutId()
{
    return ui->midiOutNames->selectedItems().at(0)->text(0);
}

void SelectOutput::on_searchLE_textChanged(const QString &)
{
    int top = ui->midiOutNames->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiOutNames->topLevelItem(i);
        curItem->setHidden(false);
    }
    if(ui->searchLE->text() == "") { return; }

    QStringList terms = ui->searchLE->text().split(" ");
    QString curTerm;

    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiOutNames->topLevelItem(i);
        foreach(curTerm,terms)
        {
            if(!curItem->text(1).contains(curTerm,Qt::CaseInsensitive))
            { curItem->setHidden(true); }
        }
    }

}

void SelectOutput::on_midiOutNames_itemClicked()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void SelectOutput::on_midiOutNames_itemDoubleClicked(QTreeWidgetItem *, int)
{
    this->accept();
}

void SelectOutput::on_refreshBtn_clicked()
{
    ui->midiOutNames->clear();
    QMap<QString,QString> outDev = QMidi::outDeviceNames();
    QStringList ids = outDev.keys();
    QTreeWidgetItem* newItem;
    foreach(QString id,ids)
    {
        newItem = new QTreeWidgetItem(ui->midiOutNames);
        newItem->setText(0,id);
        newItem->setText(1,outDev.value(id));
    }
}
