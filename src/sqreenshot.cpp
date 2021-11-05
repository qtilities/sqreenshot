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
#include "sqreenshot.hpp"
#include "dialogmain.hpp"
#include "dialogscreenshot.hpp"
#include "defs.hpp"

#include <QLocale>
#include <QTimer>
#include <QTranslator>

using namespace azd;

Sqreenshot::Sqreenshot(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setOrganizationName("AZDrums");
    setOrganizationDomain("azdrums.org");
    setApplicationName("Sqreenshot");

    dlgShot_ = new DialogScreenShot;
    dlgMain_ = new DialogMain;

    connect(this, &QApplication::aboutToQuit, dlgMain_, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, dlgShot_, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, this, [this]() { settings_.save(); });

    connect(dlgMain_, &DialogMain::sigAccepted, this, [=]() {
        int delay = settings_.delay();
        if (delay && settings_.region() != Region::Selection) // No need to wait for selection.
            delay *= 1000;
        else
            delay = 400; // Minimum time to hide the dialog.

        QTimer::singleShot(delay, dlgShot_, &DialogScreenShot::grab);
        dlgMain_->hide();
    });

    connect(dlgShot_, &DialogScreenShot::rejected, this, [=]() {
        dlgMain_->show();
        dlgShot_->hide();
    });

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "sqreenshot_" + QLocale(locale).name();
        if (translator.load(":/translations/" + baseName)) {
            installTranslator(&translator);
            break;
        }
    }
    dlgMain_->show();
}

int main(int argc, char* argv[])
{
    Sqreenshot a(argc, argv);
    return a.exec();
}
