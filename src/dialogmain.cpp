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
#include "dialogmain.hpp"
#include "./ui_dialogmain.h"
#include "dialogscreenshot.hpp"
#include "sqreenshot.hpp"
#include "defs.hpp"

#include <QButtonGroup>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QToolButton>

using namespace azd;

DialogMain::DialogMain(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogMain)
    , btnScreen(new QToolButton(this))
    , btnWindow(new QToolButton(this))
    , btnSelect(new QToolButton(this))
    , btnGroup(new QButtonGroup(this))
{
    ui->setupUi(this);

    ui->gbxRegion->setLayout(ui->layButtons);

    btnScreen->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnWindow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnSelect->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    btnScreen->setIcon(QIcon(":screen"));
    btnWindow->setIcon(QIcon(":window"));
    btnSelect->setIcon(QIcon(":selection"));

    btnScreen->setIconSize(QSize(48, 48));
    btnWindow->setIconSize(QSize(48, 48));
    btnSelect->setIconSize(QSize(48, 48));

    btnScreen->setText(tr("Screen"));
    btnWindow->setText(tr("Window"));
    btnSelect->setText(tr("Selection"));

    btnScreen->setCheckable(true);
    btnWindow->setCheckable(true);
    btnSelect->setCheckable(true);

    btnGroup->addButton(btnScreen, Region::Screen);
    btnGroup->addButton(btnWindow, Region::Window);
    btnGroup->addButton(btnSelect, Region::Selection);

    ui->layButtons->addStretch(1);
    ui->layButtons->addWidget(btnGroup->button(Region::Screen));
    ui->layButtons->addWidget(btnGroup->button(Region::Window));
    ui->layButtons->addWidget(btnGroup->button(Region::Selection));
    ui->layButtons->addStretch(1);

    QColor dark = btnScreen->palette().color(QPalette::Active, QPalette::Dark);

    setStyleSheet(
        QStringLiteral(
            "QToolButton {width: 100px; height: 68px;"
            "border-width: 1px 0 1px 0; border-color: #1e1e1e; border-style: solid;border-color:rgb(%1, %2, %3)}"
            "QToolButton:checked {background-color: rgb(%1, %2, %3)}"
            "QToolButton:pressed {background-color: rgb(%1, %2, %3)}")
            .arg(dark.red())
            .arg(dark.green())
            .arg(dark.red()));

    btnScreen->setStyleSheet(QStringLiteral(
        "QToolButton {border-width: 1px;"
        "border-top-left-radius: 6px;"
        "border-bottom-left-radius: 6px;}"));

    btnSelect->setStyleSheet(QStringLiteral(
        "QToolButton {border-width: 1px;"
        "border-top-right-radius: 6px;"
        "border-bottom-right-radius: 6px;}"));

    Settings& settings = static_cast<Sqreenshot*>(qApp)->settings();
    ui->chkCursor->setChecked(settings.withMouseCursor());
    ui->chkFrame->setChecked(settings.withDecoration());
    ui->sbxDelay->setValue(settings.delay());

    connect(ui->btnHelp, &QPushButton::clicked, this, &DialogMain::onAboutClicked);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect(btnGroup, &QButtonGroup::idClicked, this, &DialogMain::onButtonClicked);
#else
    connect(btnGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, &DialogMain::onButtonClicked);
#endif
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogMain::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, QCoreApplication::instance(), &QCoreApplication::quit);

    connect(ui->chkCursor, &QCheckBox::toggled, this,
            [&](bool checked) { settings.setWithMouseCursor(checked); });

    connect(ui->chkFrame, &QCheckBox::toggled, this,
            [&](bool checked) { settings.setWithDecoration(checked); });

    connect(ui->sbxDelay, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [&](int value) { settings.setDelay(value); });

    btnScreen->click();
    setLayout(ui->layout);
    setWindowIcon(QIcon(":appicon"));
    setWindowTitle(QCoreApplication::applicationName());
}

DialogMain::~DialogMain()
{
    delete ui;
}

void DialogMain::accept()
{
    // FIXME
    accepted();
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
void DialogMain::onButtonClicked(int region)
#else
void DialogMain::onButtonClicked(QAbstractButton* button)
#endif
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    int region = btnGroup->id(button);
#endif
    Settings& settings = static_cast<Sqreenshot*>(qApp)->settings();
    settings.setRegion(region);
    ui->gbxOptions->setDisabled(region == Region::Selection);
    ui->chkFrame->setEnabled(region == Region::Window);
}
void DialogMain::onAboutClicked()
{
    QFile f(":/about.html");
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << tr("Error loading about file") << '\n';
        return;
    }
    QTextStream in(&f);
    QString text = in.readAll();
    f.close();
    QMessageBox::about(this, tr("About"), text);
}
