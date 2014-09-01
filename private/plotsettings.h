/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    void slotExport();
    void slotBrowseOutputFolder();
    void reloadInterface();
    void updateInterface();

public:
    Ui::PlotSettings *ui;

private:
    enum ExportFormat
    {
        efPNG,
        efPDF,
        efCSV
    };

    void setupConnections();
    void addWaveformToTable(Waveform *wf);
    void exportToCsv(RTPlot *plot, const QString &outputPath);

    RTPlotDock *m_currentPlotDock;
};

#endif // PLOTSETTINGS_H
