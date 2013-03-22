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

#ifndef SETTINGS_H
#define SETTINGS_H

/****h* RagingMidi/Settings.h
 * PURPOSE
 *   Stores the application's settings
 *   and reads and writes them from storage.
 ******
 */

#include <QSettings>

/* Some people say this is "abusing the preprocessor", but it should work
 * on any modern compiler. Two '#'s means print the literal, one '#' means
 * print the literal as a string. */
#define GetSetVal(name,type) inline type get##name() { return name; } \
    inline void set##name(type newVal) { name = newVal; emit somethingChanged(#name); }

/****c* Settings.h/Settings
 * SYNOPSIS
 */
class Settings : public QObject
/**
 * DESCRIPTION
 *   Provides the settings editor class.
 *   All functions and variables in this
 *   class are static, because there is
 *   only one instance of this class per
 *   application instance.
 ******
 */
{
    Q_OBJECT
public:
    Settings(bool dontLoadNow = false);
    ~Settings();

    void load();
    void save();

    /* Note that Qt Creator gets very confused about
     * indentation here, so don't run the auto-indenter
     * on this file. */
    GetSetVal(FileDlgLoc,QString)
    GetSetVal(HWA,bool)

signals:
    void somethingChanged(QString name);

private:
    QSettings* sets;

    QString FileDlgLoc;
    bool HWA;
};

#endif // SETTINGS_H
