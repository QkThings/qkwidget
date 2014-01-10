#include "ptreeitemproperty.h"
#include "cproperty.h"

pTreeItemProperty::pTreeItemProperty(CProperty *property, pTreeItemProperty *parent) :
    QTreeWidgetItem(parent)
{
    m_property = property;
}

CProperty* pTreeItemProperty::property()
{
    return m_property;
}

/*QList<pTreeItemProperty*> pTreeItemProperty::takeChildren()
{
    QList<pTreeItemProperty*> list;
    foreach(QTreeWidgetItem *i, QTreeWidgetItem::takeChildren())
    {
        list.append((pTreeItemProperty*) i);
    }
}*/
