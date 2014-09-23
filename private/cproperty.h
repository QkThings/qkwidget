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

#ifndef CPROPERTY_H
#define CPROPERTY_H

#include <QVariant>
#include <QTreeWidgetItem>

#include "ptreeitemproperty.h"

class QToolButton;

class CProperty : public QObject
{
    Q_OBJECT

signals:
    void valueChanged(CProperty* p);
    
public:
    enum Type
    {
        Hex,
        Int,
        Double,
        Bool,
        Enum,
        Text,
        Pixmap,
        Label,
        DateTime,
        Time
    };

    CProperty(QString label, Type type, CProperty *top = 0);
    ~CProperty();

    void setEnabled(bool enabled);
    void setLabel(QString label);

    void setValue(QVariant value);
    void setEnumList(QStringList list);

    void setUserData(QVariant data);
    QVariant userData();

    QToolButton* multiFunctionButton();

    Type type();
    QString label();
    QVariant value();
    QWidget* widget();
    QStringList enumList();
    pTreeItemProperty *item();

protected slots:
    void slotValueChanged();
    
private:
    QString m_label;
    Type m_type;
    QVariant m_userData;

    QWidget *m_mainWidget;
    QWidget *m_valueWidget;
    QToolButton *m_multiFunctionButton;
    pTreeItemProperty* m_item;
};

#endif // CPROPWIDGET_H
