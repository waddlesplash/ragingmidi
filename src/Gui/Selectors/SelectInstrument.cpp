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

#include "SelectInstrument.h"
#include "ui_SelectInstrument.h"

SelectInstrument::SelectInstrument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectInstrument)
{
    ui->setupUi(this);

    ui->acceptBB->setEnabled(false);
}

SelectInstrument::~SelectInstrument()
{
    delete ui;
}

void SelectInstrument::setInsName(QString name)
{
    int top = ui->midiInstr->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        if(curItem->text(0) == name)
        {
            ui->midiInstr->setCurrentItem(curItem);
            return;
        }
    }
}

void SelectInstrument::setInsNum(int num)
{
    QString var = QString::number(num);
    int top = ui->midiInstr->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        if(curItem->text(0) == var)
        {
            ui->midiInstr->setCurrentItem(curItem);
            return;
        }
    }
}

QString SelectInstrument::insName()
{
    return ui->midiInstr->selectedItems().at(0)->text(1);
}

int SelectInstrument::insNum()
{
    return ui->midiInstr->selectedItems().at(0)->text(0).toInt();
}

void SelectInstrument::on_searchLE_textChanged(const QString &)
{
    int top = ui->midiInstr->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        curItem->setHidden(false);
    }
    if(ui->searchLE->text() == "") { return; }

    QStringList terms = ui->searchLE->text().split(" ");
    QString curTerm;

    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        foreach(curTerm,terms)
        {
            if(!curItem->text(1).contains(curTerm,Qt::CaseInsensitive))
            { curItem->setHidden(true); }
        }
    }

}

void SelectInstrument::on_midiInstr_itemClicked()
{
    ui->acceptBB->setEnabled(true);
}

void SelectInstrument::on_midiInstr_itemDoubleClicked(QTreeWidgetItem*, int)
{
    this->accept();
}
