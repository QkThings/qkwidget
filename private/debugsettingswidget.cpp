#include "debugsettingswidget.h"
#include "ui_debugsettingswidget.h"

DebugSettingsWidget::DebugSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugSettingsWidget)
{
    ui->setupUi(this);
}

DebugSettingsWidget::~DebugSettingsWidget()
{
    delete ui;
}
