#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QWidget>
#include <qkcore.h>

namespace Ui {
class EventWidget;
}

class EventWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EventWidget(QWidget *parent = 0);
    ~EventWidget();
    void setEvent(QkDevice::Event *event, int address = 0);

private:
    Ui::EventWidget *ui;
};

#endif // EVENTWIDGET_H
