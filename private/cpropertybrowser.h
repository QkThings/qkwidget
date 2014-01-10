#ifndef CPROPBROWSER_H
#define CPROPBROWSER_H

#include "cproperty.h"
#include <QList>
#include <QTreeWidget>

class CPropertyBrowser : public QTreeWidget
{
    Q_OBJECT

    friend class CProperty;

signals:
    //value in browser changed
    void valueChanged(CProperty*);

protected:
    
public:
    explicit CPropertyBrowser(QWidget *parent = 0);
    ~CPropertyBrowser();

    void addProperty(CProperty* p, CProperty* parent = 0);
    void clearChildren(CProperty *root);

protected slots:
    void propertyValueChanged(CProperty* p);


};

#endif // CPROPBROWSER_H
