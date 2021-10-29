/*
    Sqreenshot - A simple screenshot tool.
    Copyright (C) 2021  Andrea Zanellato <redtid3@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    SPDX-License-Identifier: GPL-3.0-only
*/
#include "settings.hpp"

#include <QApplication>
#include <QSettings>

namespace Default {
QString directory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
const bool withDecoration = true;
const bool withMouseCursor = false;
const int delay = 0;
const int region = 0;
} // namespace Default

using namespace azd;

Settings::Settings()
    : directory_(Default::directory)
    , withDecoration_(Default::withDecoration)
    , withMouseCursor_(Default::withMouseCursor)
    , delay_(Default::delay)
    , region_(Default::region)
{
    load();
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.beginGroup("Options");
    directory_ = settings.value(QStringLiteral("LastUsedDirectory"), Default::directory).toString();
    withDecoration_ = settings.value(QStringLiteral("WindowDecoration"), Default::withDecoration).toBool();
    withMouseCursor_ = settings.value(QStringLiteral("ShowMouseCursor"), Default::withMouseCursor).toBool();
    delay_ = settings.value(QStringLiteral("Delay"), Default::delay).toInt();
    settings.endGroup();
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.beginGroup("Options");
    settings.setValue(QStringLiteral("LastUsedDirectory"), directory_);
    settings.setValue(QStringLiteral("WindowDecoration"), withDecoration_);
    settings.setValue(QStringLiteral("ShowMouseCursor"), withMouseCursor_);
    settings.setValue(QStringLiteral("Delay"), delay_);
    settings.endGroup();
}
