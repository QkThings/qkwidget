#include "loggersettingswidget.h"
#include "ui_loggersettingswidget.h"

LoggerSettingsWidget::LoggerSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerSettingsWidget)
{
    ui->setupUi(this);
}

LoggerSettingsWidget::~LoggerSettingsWidget()
{
    delete ui;
}
