/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "loggersettingswidget.h"
#include "ui_loggersettingswidget.h"

#include <QFileDialog>

LoggerSettingsWidget::LoggerSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerSettingsWidget)
{
    ui->setupUi(this);
    setupConnections();
}

LoggerSettingsWidget::~LoggerSettingsWidget()
{
    delete ui;
}


void LoggerSettingsWidget::setupConnections()
{

}

void LoggerSettingsWidget::exportToCsv(const QString &outputPath)
{
    connect(ui->buttonExport, SIGNAL(clicked()), this, SLOT(slotExport()));
}

void LoggerSettingsWidget::slotExport()
{

}

void LoggerSettingsWidget::slotBrowserOutputFolder()
{
    ui->lineOutFolder->setText(QFileDialog::getExistingDirectory(this, tr("Export to...")));
}
