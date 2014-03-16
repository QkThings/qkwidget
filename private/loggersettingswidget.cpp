#include "loggersettingswidget.h"
#include "ui_loggersettingswidget.h"

#include <QFileDialog>

LoggerSettingsWidget::LoggerSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerSettingsWidget)
{
    ui->setupUi(this);
    setupConnections();
}

LoggerSettingsWidget::~LoggerSettingsWidget()
{
    delete ui;
}


void LoggerSettingsWidget::setupConnections()
{

}

void LoggerSettingsWidget::exportToCsv(const QString &outputPath)
{
    connect(ui->buttonExport, SIGNAL(clicked()), this, SLOT(slotExport()));
}

void LoggerSettingsWidget::slotExport()
{

}

void LoggerSettingsWidget::slotBrowserOutputFolder()
{
    ui->lineOutFolder->setText(QFileDialog::getExistingDirectory(this, tr("Export to...")));
}
