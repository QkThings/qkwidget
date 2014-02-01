#ifndef PLOTSETTINGS_H
#define PLOTSETTINGS_H

#include <QWidget>
#include "rtplot.h"

namespace Ui {
class PlotSettings;
}


class PlotSettings : public QWidget
{
    Q_OBJECT

public:
    enum {
        ColumnWaveformID,
        ColumnWaveformName,
        ColumnWaveformColor,
        ColumnWaveformCount
    };
    explicit PlotSettings(QWidget *parent = 0);
    ~PlotSettings();

    void setCurrentPlotDock(RTPlotDock *plotDock);

public slots:
    void setGlobal(bool enabled);
    void slotSetTitle(const QString &title);
    void slotSetTimeWindow(int timeWindow);
    void slotSetAmplitude();
    void slotSetAutoscale(bool enabled);
    void slotSetStopAtEnd(bool enabled);
    void slotShowHideTitle(bool show);
    void slotShowHideLegend(bool show);
    void slotShowHideAxis();
    void reloadInterface();
    void updateInterface();

public:
    Ui::PlotSettings *ui;

private:

    void setupConnections();
    void addWaveformToTable(Waveform *wf);

    RTPlotDock *m_currentPlotDock;
};

#endif // PLOTSETTINGS_H
