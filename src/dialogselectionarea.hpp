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

namespace azd {

class DialogSelectionArea : public QDialog {
    Q_OBJECT

public:
    explicit DialogSelectionArea(QWidget* parent = nullptr);

    QPixmap selectionPixmap() const { return pixSelection_; }

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void doShowTooltip(QPainter& painter);

    QPixmap pixSelection_;
    QPixmap pixScreen_;
    QRect rctSelection_;
    bool showTooltip_;
};
} // namespace azd
