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

#include <QDebug>

QHexSpinBox::QHexSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
    setRange(0, 0xFFFFFFFF);
    validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,8}"), this);
}

QValidator::State QHexSpinBox::validate(QString &text, int &pos) const
{
    return validator->validate(text, pos);
}

QString QHexSpinBox::textFromValue(int value) const
{
    unsigned int uval = (unsigned int) value;

    QString hexStr;
    hexStr.append(QString::number((uval >> 16) & 0xFFFF, 16).rightJustified(4, '0'));
    hexStr.append(QString::number((uval) & 0xFFFF, 16).rightJustified(4, '0'));
    return hexStr.toUpper();
}

int QHexSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toUInt(&ok, 16);
}

