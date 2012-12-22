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

#ifndef SEEKSLIDER_H
#define SEEKSLIDER_H

/****h* RagingMidi/SeekSlider.h
 * PURPOSE
 *   Creates the slider widget used to seek
 *   around midi files.
 ******
 */

#include <QSlider>

namespace Ui {
class SeekSlider;
}

/****c* SeekSlider.h/SeekSlider
 * SYNOPSIS
 */
class SeekSlider : public QSlider
/********/
{
    Q_OBJECT
    
public:
    explicit SeekSlider(QWidget *parent = 0);
    ~SeekSlider();

    inline bool doUpdate() { return enableUpdate; }

protected:
    inline void mousePressEvent(QMouseEvent *e)
    { enableUpdate = false; QSlider::mousePressEvent(e); }
    inline void mouseReleaseEvent(QMouseEvent *e)
    { enableUpdate = true; QSlider::mouseReleaseEvent(e); }
    
private:
    Ui::SeekSlider *ui;
    bool enableUpdate;
    int newPos;
};

#endif // SEEKSLIDER_H
