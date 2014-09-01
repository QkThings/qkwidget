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

#include <QtGlobal>
#include <QFont>

const QString GUI_MAINWINDOW_TITLE = "QkDaemon";

#ifdef Q_OS_WIN
const QFont GUI_MONOFONT = QFont("Consolas", 9);
#else
const QFont GUI_MONOFONT = QFont("Monospace", 8);
#endif

const QString MONOFONT_NAME = "DejaVuSansMono";

#endif // GUI_GLOBALS_H
