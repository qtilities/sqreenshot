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

#include "settings.hpp"

#include <QApplication>

namespace azd {

class DialogMain;
class DialogScreenShot;

class Sqreenshot : public QApplication {
    Q_OBJECT

public:
    Sqreenshot(int& argc, char** argv);
    Settings& settings() { return settings_; }

private:
    void grab();

    Settings settings_;
    DialogMain* dlgMain_;
    DialogScreenShot* dlgShot_;
    int mode_;
};
} // namespace azd
