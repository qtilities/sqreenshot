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

    This file is a modified copy of lxqt/lximage-qt/src/screenshotdialog.cpp
    (commit 71cdceb on 23 Oct 2019) from:

    LXImage-Qt - a simple and fast image viewer
    Copyright (C) 2013  PCMan <pcman.tw@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "dialogscreenshot.hpp"
#include "./ui_dialogscreenshot.h"
#include "dialogselectionarea.hpp"
#include "sqreenshot.hpp"
#include "defs.hpp"

#include <QClipboard>
#include <QDateTime>
#include <QFileDialog>
#include <QIcon>
#include <QImageWriter>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QStandardPaths>
#include <QTimer>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>

#include <glib.h>

using namespace azd;

static bool hasXFixes()
{
    int eventBase, errorBase;
    return XFixesQueryExtension(QX11Info::display(), &eventBase, &errorBase);
}

DialogScreenShot::DialogScreenShot(QWidget* parent)
    : parent_(parent)
    , ui(new Ui::DialogScreenShot)
{
    ui->setupUi(this);

    QScreen* screen =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        this->screen();
#else
        qGuiApp->screenAt(pos());
#endif

    ui->lblScreenShot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->lblScreenShot->setAlignment(Qt::AlignCenter);
    ui->lblScreenShot->setMinimumSize(
        screen->geometry().width() / 8,
        screen->geometry().height() / 8);

    QPushButton* save = ui->buttonBox->button(QDialogButtonBox::Save);
    QPushButton* clip = ui->buttonBox->addButton(tr("Save to clipboard"), QDialogButtonBox::YesRole);

    connect(clip, &QPushButton::pressed, this,
            [this]() { QGuiApplication::clipboard()->setPixmap(pixShot); });

    connect(save, &QPushButton::clicked, this, &DialogScreenShot::onSave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogScreenShot::reject);

    setLayout(ui->layout);
    setWindowIcon(QIcon(":appicon"));
    setWindowTitle(QCoreApplication::applicationName());
}

DialogScreenShot::~DialogScreenShot()
{
    delete ui;
}

void DialogScreenShot::resizeEvent(QResizeEvent* event)
{
    QSize previousSize = pixShot.size();
    previousSize.scale(ui->lblScreenShot->size(), Qt::KeepAspectRatio);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    if (previousSize != ui->lblScreenShot->pixmap(Qt::ReturnByValue).size())
#else
    if (previousSize != ui->lblScreenShot->pixmap()->size())
#endif
        updatePreview();

    QDialog::resizeEvent(event);
}

void DialogScreenShot::reject()
{
    hide();
    QDialog::reject();
}

void DialogScreenShot::onSave()
{
    Settings& settings = static_cast<Sqreenshot*>(qApp)->settings();
    QString destination = settings.directory();
    destination += '/';

    QDateTime dateTime = QDateTime::currentDateTime();
    QString strDateTime = dateTime.toString("-yyyy-MM-dd-hh-mm-ss");
    const QString defaultFormat = QStringLiteral("png");
    destination += tr("Screenshot") + strDateTime + '.' + defaultFormat;

    QStringList mimeTypes;
    const QList<QByteArray> types = QImageWriter::supportedMimeTypes();
    for (const QByteArray& mimeType : types)
        mimeTypes.append(QLatin1String(mimeType));

    QFileDialog fileDialog(this, tr("Save as"), destination);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(destination);
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + defaultFormat);
    fileDialog.setDefaultSuffix(defaultFormat);

    if (fileDialog.exec() == QDialog::Accepted) {
        QString fileName(fileDialog.selectedFiles().at(0));
        if (!pixShot.save(fileName)) {
            QMessageBox::warning(
                this,
                tr("Error"),
                tr("Can't save the image: \"%1\".")
                    .arg(QDir::toNativeSeparators(fileName)));
        } else {
            QFileInfo fileInfo(fileName);
            QString lastUsed = fileInfo.dir().path();
            settings.setDirectory(lastUsed);
        }
    }
    hide();
    reject();
}

void DialogScreenShot::updatePreview()
{
    ui->lblScreenShot->setPixmap(
        pixShot.scaled(
            ui->lblScreenShot->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
}

void DialogScreenShot::grab()
{
    Settings& settings = static_cast<Sqreenshot*>(qApp)->settings();
    int region = settings.region();
    WId wid = 0;                 // TODO: defaults for
    QRect rect { 0, 0, -1, -1 }; // the current desktop?

    if (region == Region::Selection) {
        DialogSelectionArea dlgSelectArea;
        if (dlgSelectArea.exec() == QDialog::Accepted)
            pixShot = dlgSelectArea.selectionPixmap();
    } else {
        if (region == Region::Window) {
            WId activeWid = activeWindowId();
            if (activeWid) {
                if (settings.withDecoration())
                    rect = windowFrame(activeWid);
                else
                    wid = activeWid;
            }
        }
        pixShot = doGrab(wid, rect, settings.withMouseCursor());
    }
    updatePreview();
    show();
}

WId DialogScreenShot::activeWindowId()
{
    WId root = WId(QX11Info::appRootWindow());
    Atom atom = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", false);
    unsigned long type, resultLen, rest;
    int format;
    WId result = 0;
    unsigned char* data = nullptr;
    if (XGetWindowProperty(QX11Info::display(), root, atom, 0, 1, false,
                           XA_WINDOW, &type, &format, &resultLen, &rest, &data)
        == Success) {
        result = *reinterpret_cast<long*>(data);
        XFree(data);
    }
    return result;
}

QRect DialogScreenShot::windowFrame(WId wid)
{
    QRect result;
    XWindowAttributes wa;
    if (XGetWindowAttributes(QX11Info::display(), wid, &wa)) {
        Window child;
        int x, y;
        // translate to root coordinate
        XTranslateCoordinates(QX11Info::display(), wid, wa.root, 0, 0, &x, &y, &child);
#if 0
        qDebug("%d, %d, %d, %d", x, y, wa.width, wa.height);
#endif
        result.setRect(x, y, wa.width, wa.height);

        // get the frame widths added by the window manager
        Atom atom = XInternAtom(QX11Info::display(), "_NET_FRAME_EXTENTS", false);
        unsigned long type, resultLen, rest;
        int format;
        unsigned char* data = nullptr;
        if (XGetWindowProperty(QX11Info::display(), wid, atom, 0, G_MAXLONG, false,
                               XA_CARDINAL, &type, &format, &resultLen, &rest, &data)
            == Success) {
        }
        if (data) { // left, right, top, bottom
            long* offsets = reinterpret_cast<long*>(data);
            result.setLeft(result.left() - offsets[0]);
            result.setRight(result.right() + offsets[1]);
            result.setTop(result.top() - offsets[2]);
            result.setBottom(result.bottom() + offsets[3]);
            XFree(data);
        }
    }
    return result;
}

QPixmap DialogScreenShot::doGrab(const WId& wid, const QRect& rect, bool takeCursor)
{
    QPixmap pixmap;
    QScreen* screen =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        this->screen();
#else
        qGuiApp->screenAt(pos());
#endif
    if (!screen)
        return pixmap;

    pixmap = screen->grabWindow(wid, rect.x(), rect.y(), rect.width(), rect.height());

    if (!takeCursor || !hasXFixes())
        return pixmap;

    XFixesCursorImage* cursor = XFixesGetCursorImage(QX11Info::display());
    if (!cursor)
        return pixmap;

    if (!cursor->pixels) { // pixels should be an ARGB array
        XFree(cursor);
        return pixmap;
    }
    QImage cursorImage;
    if (sizeof(long) == 4) {
        // FIXME: will we encounter byte-order problems here?
        cursorImage = QImage(
            (uchar*)cursor->pixels,
            cursor->width,
            cursor->height,
            QImage::Format_ARGB32);
    } else {
        // XFixes returns long integers which is not 32 bit on 64 bit systems.
        long len = cursor->width * cursor->height;
        quint32* buf = new quint32[len];
        for (long i = 0; i < len; ++i) {
            buf[i] = (quint32)cursor->pixels[i];
        }
        cursorImage = QImage(
            (uchar*)buf, cursor->width, cursor->height, QImage::Format_ARGB32,
            [](void* b) { delete[](quint32*) b; },
            buf);
    }
    // paint the cursor on the current image
    QPainter painter(&pixmap);
    painter.drawImage(cursor->x - cursor->xhot, cursor->y - cursor->yhot, cursorImage);

    XFree(cursor);

    return pixmap;
}
