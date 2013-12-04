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

#include "PrefsDlg.h"
#include "ui_PrefsDlg.h"

#include "../MainWind.h"

PrefsDlg::PrefsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDlg)
{
    ui->setupUi(this);

    Settings* s = MainWind::settings;
    ui->hwaChk->setChecked(s->getHWA());

#ifdef QT_NO_OPENGL
    ui->hwaChk->setDisabled(true);
    ui->hwaChk->setChecked(false);
#endif
}

PrefsDlg::~PrefsDlg()
{
    delete ui;
}

void PrefsDlg::applyPrefs()
{
    Settings* s = MainWind::settings;
    s->setHWA(ui->hwaChk->isChecked());
    s->save();
}

void PrefsDlg::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
        applyPrefs();
        this->close();
    } else if(ui->buttonBox->standardButton(button) == QDialogButtonBox::Cancel) {
        this->close();
    } else if(ui->buttonBox->standardButton(button) == QDialogButtonBox::Apply) {
        applyPrefs();
    }
}
