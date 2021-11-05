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
#pragma once

#include <QString>

namespace azd {

class Settings {
public:
    Settings();

    void load();
    void save();

    QString directory() const { return directory_; }
    void setDirectory(const QString& directory) { directory_ = directory; }

    bool withDecoration() const { return withDecoration_; }
    void setWithDecoration(bool withDecoration) { withDecoration_ = withDecoration; }

    bool withMouseCursor() const { return withMouseCursor_; }
    void setWithMouseCursor(bool withMouseCursor) { withMouseCursor_ = withMouseCursor; }

    int delay() const { return delay_; }
    void setDelay(int delay) { delay_ = delay; }

    int region() { return region_; }
    void setRegion(int region) { region_ = region; }

private:
    QString directory_;
    bool withDecoration_;
    bool withMouseCursor_;
    int delay_;
    int region_;
};
} // namespace azd
