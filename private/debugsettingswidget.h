#ifndef DEBUGSETTINGSWIDGET_H
#define DEBUGSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class DebugSettingsWidget;
}

class DebugSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebugSettingsWidget(QWidget *parent = 0);
    ~DebugSettingsWidget();

    Ui::DebugSettingsWidget *ui;

private:
};

#endif // DEBUGSETTINGSWIDGET_H
