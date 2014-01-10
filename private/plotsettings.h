#ifndef PLOTSETTINGS_H
#define PLOTSETTINGS_H

#include <QWidget>

namespace Ui {
class PlotSettings;
}

class RTPlotDock;
class Waveform;

class PlotSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlotSettings(QWidget *parent = 0);
    ~PlotSettings();

    void setCurrentPlotDock(RTPlotDock *plotDock);

public slots:
    void setGlobal(bool enabled);
    void slotSetTitle(const QString &title);
    void slotSetTimeWindow(int timeWindow);
    void slotShowHideTitle(bool show);
    void slotShowHideLegend(bool show);
    void reloadInterface();
    void updateInterface();

public:
    Ui::PlotSettings *ui;

private:
    enum {
        ColumnWaveformID,
        ColumnWaveformName,
        ColumnWaveformColor,
        ColumnWaveformCount
    };

    void setupConnections();
    void addWaveformToTable(Waveform *wf);

    RTPlotDock *m_currentPlotDock;
};

#endif // PLOTSETTINGS_H
