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

#include <QDialog>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QButtonGroup;
class QToolButton;
namespace Ui {
class DialogMain;
}
QT_END_NAMESPACE

namespace azd {

class DialogMain : public QDialog {
    Q_OBJECT

public:
    DialogMain(QWidget* parent = nullptr);
    ~DialogMain();

signals:
    void sigAccepted();

private:
    void onAboutClicked();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    void onButtonClicked(int);
#else
    void onButtonClicked(QAbstractButton*);
#endif
    Ui::DialogMain* ui;
    QToolButton *btnScreen, *btnWindow, *btnSelect;
    QButtonGroup* btnGroup;
};
} // namespace azd
