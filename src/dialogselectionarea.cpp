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
#include "dialogselectionarea.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

#include <QDebug>

using namespace azd;

DialogSelectionArea::DialogSelectionArea(QWidget* parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , showTooltip_(true)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowState(windowState() | Qt::WindowFullScreen);

    setCursor(Qt::CrossCursor);
    QScreen* screen =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        this->screen();
#else
        qGuiApp->screenAt(pos());
#endif
    pixScreen_ = screen->grabWindow(0);
}

void DialogSelectionArea::mousePressEvent(QMouseEvent* event)
{
    rctSelection_.setTopLeft(event->globalPos());
    showTooltip_ = false;
}

void DialogSelectionArea::mouseMoveEvent(QMouseEvent* event)
{
    rctSelection_.setBottomRight(event->globalPos());
    update();
}

void DialogSelectionArea::mouseReleaseEvent(QMouseEvent*)
{
    pixSelection_ = pixScreen_.copy(rctSelection_.normalized());
    setCursor(Qt::ArrowCursor);
    accept();
}

void DialogSelectionArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixScreen_);

    QPainterPath path;
    path.addRect(rect());
    path.addRect(rctSelection_);
    painter.fillPath(path, QColor::fromRgb(0, 0, 0, 128));

    painter.setPen(Qt::black);
    painter.drawRect(rctSelection_);

    if (showTooltip_)
        doShowTooltip(painter);
}

void DialogSelectionArea::doShowTooltip(QPainter& painter)
{
    QString text = tr("Press \"Esc\" key or left click to exit.");
    QScreen* screen =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        this->screen();
#else
        qGuiApp->screenAt(pos());
#endif
    QRect rct = screen->geometry();
    rct.setHeight(rct.height() / 10);
    rct = painter.boundingRect(rct, Qt::AlignCenter, text);
    rct.adjust(-12, -12, 12, 12);

    QColor bg = this->palette().color(QPalette::Active, QPalette::ToolTipBase);
    QColor fg = this->palette().color(QPalette::Active, QPalette::ToolTipText);

    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawRect(rct);
    painter.drawText(rct, Qt::AlignCenter, text);
}
