#ifndef PTREEITEMPROPERTY_H
#define PTREEITEMPROPERTY_H

#include <QTreeWidgetItem>

class CProperty;

class pTreeItemProperty : public QTreeWidgetItem
{
public:
    explicit pTreeItemProperty(CProperty *property, pTreeItemProperty *parent = 0);
    CProperty* property();
    //QList<pTreeItemProperty*> takeChildren();

signals:
    
public slots:

private:
    CProperty *m_property;
    
};

#endif // PTREEITEMPROPERTY_H
