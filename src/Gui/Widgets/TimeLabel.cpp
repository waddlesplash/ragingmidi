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

#include "TimeLabel.h"
#include "ui_TimeLabel.h"

#include <math.h> // for floor()

TimeLabel::TimeLabel(QWidget *parent) :
    QLabel(parent),
    ui(new Ui::TimeLabel)
{
    ui->setupUi(this);
    myTick = 0;
}

TimeLabel::~TimeLabel()
{
    delete ui;
}

void TimeLabel::setTick(qint32 t)
{
    if(!file) { return; }
    myTick = t;
    float time = file->timeFromTick(myTick);
    int min = floor(time/60.0);
    time -= min*60;
    QString tim("%1:%2");
    tim = tim.arg(min).arg(time,6,'f',3,'0');
    this->setText(tim);
}
