#ifndef PTREEWIDGETITEM_H
#define PTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QTreeWidget>

class pTreeWidgetItem : public QTreeWidgetItem
{
public:
    pTreeWidgetItem(QTreeWidget *parent = 0);

    void removeChildren();
    
signals:
    
public slots:
    
};

#endif // PTREEWIDGETITEM_H
