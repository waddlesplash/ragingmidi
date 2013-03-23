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

#include "Settings.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#   include <QStandardPaths>
#else
#   include <QDesktopServices>
#endif

Settings::Settings(bool dontLoadNow)
{
    sets = new QSettings("waddlesplash","ragingmidi");
    if(!dontLoadNow) { load(); }
}
Settings::~Settings()
{
    delete sets;
}

void Settings::load()
{
    HWA = sets->value("EnableHWA",true).toBool();

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QVariant s = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#else
    QVariant s = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
    FileDlgLoc = sets->value("FileDlgLoc",s).toString();
}
void Settings::save()
{
    sets->setValue("EnableHWA",HWA);
    sets->setValue("FileDlgLoc",FileDlgLoc);
}