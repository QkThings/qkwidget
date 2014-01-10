#ifndef GUI_GLOBALS_H
#define GUI_GLOBALS_H

#include <QtGlobal>
#include <QFont>

const QString GUI_MAINWINDOW_TITLE = "QkDaemon";

#ifdef Q_OS_WIN
const QFont GUI_MONOFONT = QFont("Consolas", 9);
#else
const QFont GUI_MONOFONT = QFont("Monospace", 8);
#endif

#endif // GUI_GLOBALS_H
