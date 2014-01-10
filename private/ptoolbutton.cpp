#include "ptoolbutton.h"

#include "ptoolbutton.h"

pToolButton::pToolButton(QWidget *parent) :
    QToolButton(parent)
{
    setAutoRaise(true);
    setContextMenuPolicy(Qt::NoContextMenu);
}
