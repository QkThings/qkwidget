#include "eventwidget.h"
#include "ui_eventwidget.h"
#include "gui_globals.h"
#include <QTime>

EventWidget::EventWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    ui->timestamp->setFont(GUI_MONOFONT);
    ui->label->setFont(GUI_MONOFONT);
    ui->source->setFont(GUI_MONOFONT);
    ui->message->setFont(GUI_MONOFONT);
}

EventWidget::~EventWidget()
{
    delete ui;
}

void EventWidget::setEvent(QkDevice::Event *event, int address)
{
    ui->timestamp->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->label->setText(event->label());
    ui->source->setText(QString().sprintf("%04X", address));
    ui->message->setText(event->message());
    ui->arguments->clear();
    foreach(float arg, event->args())
        ui->arguments->addItem(QString::number(arg));
}
