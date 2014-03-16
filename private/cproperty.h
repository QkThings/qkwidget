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
