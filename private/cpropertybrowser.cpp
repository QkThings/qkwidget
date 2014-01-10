#include "cpropertybrowser.h"

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
