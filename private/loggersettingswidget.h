#ifndef LOGGERSETTINGSWIDGET_H
#define LOGGERSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class LoggerSettingsWidget;
}

class LoggerSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoggerSettingsWidget(QWidget *parent = 0);
    ~LoggerSettingsWidget();

    Ui::LoggerSettingsWidget *ui;

private slots:
    void slotExport();
    void slotBrowserOutputFolder();

private:
    void setupConnections();
    void exportToCsv(const QString &outputPath);
};

#endif // LOGGERSETTINGSWIDGET_H
