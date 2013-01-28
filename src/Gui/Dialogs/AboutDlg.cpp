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

#include "AboutDlg.h"
#include "ui_AboutDlg.h"

#include "../../config.h"

#include <QDesktopServices>
#include <QFile>

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);

    ui->versionLbl->setText(tr("version %1","%1 = version num.").arg(RM_VERSION));

    QFile f(":/REVISION.txt");
    f.open(QFile::ReadOnly);
    QString rev(f.readAll());
    rev = rev.trimmed();
    f.close();
    QString link;
    link += "<a href=\"https://github.com/waddlesplash/ragingmidi/commit/"+ rev;
    link += "\">"+ rev.mid(0,10) +"</a>";
    ui->fromRevLbl->setText(tr("from revision %1","%1 = revision hash").arg(link));

    ui->compForQtLbl->setText(QT_VERSION_STR);
    ui->runOnQtLbl->setText(qVersion());
    int size = sizeof(void*)*8;
    QString arch = tr("%1-bit","32-bit or 64-bit").arg(size);
    ui->archLbl->setText(arch);

    ui->textBrowser->setText(tr("<u><b>Created By</b></u><br />"
                                "<a href=\"https://github.com/waddlesplash\"><u>@waddlesplash</u></a><br />"
                                "<br />"
                                "<u><b>Contributors</b></u><br />"
                                "<a href=\"https://github.com/forrestcavalier\"><u>Forrest Cavalier III</u></a><br />"
                                "<br />"
                                "<u><b>Using Code By</b></u><br />"
                                "David G. Slomin, <a href=\"http://www.sreal.com/~div/midi-utilities/\"><u>MIDI Utilities</u></a>."));
}

AboutDlg::~AboutDlg()
{
    delete ui;
}

void AboutDlg::on_textBrowser_anchorClicked(const QUrl &arg1)
{
    QDesktopServices::openUrl(arg1);
}

void AboutDlg::on_fromRevLbl_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void AboutDlg::on_urlLbl_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
