#ifndef PTOOLBUTTON_H
#define PTOOLBUTTON_H

#include <QToolButton>

class pToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit pToolButton(QWidget *parent = 0);

    void setNumber(int number) { m_number = number; }
    int number() { return m_number; }

signals:

public slots:


private:
    int m_number;

};

#endif // PTOOLBUTTON_H
