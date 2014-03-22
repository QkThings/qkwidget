#include "eventwidget.h"
#include "ui_eventwidget.h"
#include "gui_globals.h"
#include <QTime>

EventWidget::EventWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    ui->label->setFont(QFont(MONOFONT_NAME, 9));
    ui->source->setFont(QFont(MONOFONT_NAME, 8));
    ui->timestamp->setFont(QFont(MONOFONT_NAME, 8));
    ui->message->setFont(QFont(MONOFONT_NAME, 9));
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
    QString messageWithArgs = insertArgsOnMessage(event->message(), event->args());
    ui->message->setText(messageWithArgs);
    ui->arguments->clear();
    foreach(float arg, event->args())
        ui->arguments->addItem(QString::number(arg));
}

QString EventWidget::insertArgsOnMessage(QString msg, QList<float> args)
{
    for(int i=0; i<args.count(); i++)
        msg.replace(QRegularExpression("\\%" + QString::number(i)), QString::number(args[i]));

    return msg;
}
