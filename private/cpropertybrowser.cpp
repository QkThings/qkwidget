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

#include <QHeaderView>
#include <QDebug>

CPropertyBrowser::CPropertyBrowser(QWidget *parent) :
    QTreeWidget(parent)
{
    setAllColumnsShowFocus(false);
    setColumnCount(2);
    QStringList headers;
    headers << "Property" << "Value";
    setHeaderLabels(headers);
}

CPropertyBrowser::~CPropertyBrowser()
{

}

void CPropertyBrowser::addProperty(CProperty* p, CProperty* parent)
{
    if(parent == 0) {
        addTopLevelItem(p->item());
    }
    else{
        parent->item()->addChild(p->item());
    }
    setItemWidget(p->item(), 1, p->widget());
    connect(p, SIGNAL(valueChanged(CProperty*)), this, SLOT(propertyValueChanged(CProperty*)));
}

void CPropertyBrowser::clearChildren(CProperty *root)
{
    if(root == 0) return;

    QWidget *widget;

    QList<QTreeWidgetItem*> list = root->item()->takeChildren();
    foreach(QTreeWidgetItem *item, list)
    {
        pTreeItemProperty *itemProperty = (pTreeItemProperty*) item;
        widget = (QWidget*)itemWidget(item, 0);
        if(widget != 0)
        {
            removeItemWidget(item, 0);
            delete widget;
        }
    }
    qDeleteAll(list.begin(), list.end());
}


void CPropertyBrowser::propertyValueChanged(CProperty* p)
{
    emit valueChanged(p);
}
