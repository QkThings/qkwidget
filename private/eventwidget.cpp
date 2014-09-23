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

#include "eventwidget.h"
#include "ui_eventwidget.h"
#include "gui_globals.h"
#include <QTime>

EventWidget::EventWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    ui->label->setFont(QFont("Roboto-Regular", 9));
    ui->source->setFont(QFont(MONOFONT_NAME, 9));
    ui->timestamp->setFont(QFont(MONOFONT_NAME, 9));
    ui->message->setFont(QFont("OpenSans-Regular", 9));
}

EventWidget::~EventWidget()
{
    delete ui;
}

void EventWidget::setEvent(QkDevice::Event *event, int address)
{
    ui->timestamp->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->label->setText(event->label());
    ui->source->setText(QString().sprintf("%04X", address));
    QString messageWithArgs = insertArgsOnMessage(event->message(), event->args());
    ui->message->setText(messageWithArgs);
    ui->arguments->clear();
    foreach(float arg, event->args())
        ui->arguments->addItem(QString::number(arg));
}

QString EventWidget::insertArgsOnMessage(QString msg, QList<float> args)
{
    for(int i=0; i<args.count(); i++)
        msg.replace(QRegularExpression("\\%" + QString::number(i)), QString::number(args[i]));

    return msg;
}
