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
namespace Ui {
class DialogScreenShot;
}
QT_END_NAMESPACE

namespace azd {

class DialogScreenShot : public QDialog {
    Q_OBJECT

public:
    DialogScreenShot(QWidget* parent = nullptr);
    ~DialogScreenShot();

    void grab();

private:
    void resizeEvent(QResizeEvent*) override;
    void reject() override;

    void onSave();
    void updatePreview();

    WId activeWindowId();   // Returns the active window WId
    QRect windowFrame(WId); // Returns the  active window decoration rect
    QPixmap doGrab(const WId&, const QRect&, bool);

    QPixmap pixShot;
    QWidget* parent_;
    Ui::DialogScreenShot* ui;
};
} // namespace azd
