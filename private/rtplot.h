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

#include "qcustomplot.h"
#include <QDockWidget>

#include "qcustomplot.h"
#include <QObject>
#include <QElapsedTimer>

class Waveform : public QObject
{
    Q_OBJECT
public:
    explicit Waveform(const QString &name = QString(), QObject *parent = 0);
    int id();

    void setName(const QString &name);
    QString name();
    void setColor(const QColor &color);
    QColor color();

    QCPGraph *graph;
private:
    int m_id;
    QString m_name;
    static int m_nextID;
};

class RTPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit RTPlot(QWidget *parent = 0);
    Waveform* addWaveform(const QString &name = QString(), const QColor &color = QColor());
    void removeWaveform(int id);
    void removeWaveforms();
    Waveform* waveform(int id);
    QList<Waveform*> waveforms();
    int timeWindow();
    double amplitudeMin() { return yAxis->range().lower; }
    double amplitudeMax() { return yAxis->range().upper; }
    bool autoscale() { return m_autoscale; }
    bool stopAtEnd() { return m_stopAtEnd; }

signals:

public slots:
    void setTitle(const QString &title);
    void setTimeWindow(int sec);
    void setAmplitude(double min, double max);
    void setAutoscale(bool enabled) { setAmplitude(0.0,0.0); m_autoscale = enabled; }
    void setStopAtEnd(bool enabled);
    void start();
    void stop();
    void addData(int waveformID, double data, quint64 timestamp = 0);
    void addData(Waveform *wf, double data, quint64 timestamp = 0);
    void showTitle(bool show);
    void showAxis(bool x, bool y);

private slots:
    void slotMousePress();
    void slotMouseWheel();
    void titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title);
    void axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
    void selectionChanged();

private:
    void _setup();
    void _clearAllWaveforms();
    QColor _pickWaveformColor();

    int m_id;
    double m_timeWindow_sec;
    int m_nextColorIdx;
    QVector<QColor> m_defaultColors;
    QElapsedTimer m_clock;
    QTimer m_timer;
    QString m_xAxisLabel, m_yAxisLabel;
    QMap<int, Waveform*> m_waveforms;
    QCPPlotTitle *m_title;
    //bool m_replotAfterAdd;
    bool m_autoscale;
    bool m_stopAtEnd;
    bool m_elaspedTimerReset;
    quint64 m_startTimestamp;
};

class WaveformMapper
{
public:
     RTPlot *plot;
     int waveformID;
};


class RTPlotDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit RTPlotDock(RTPlot *plot, QWidget *parent= 0);
    void setWindowTitle(const QString &title);
    RTPlot *plot();
    int id();
    void mousePressEvent(QMouseEvent *e);

    static void resetId() { nextID = 0; }

signals:
    void dockSelected(int);
    void titleChanged(int,QString);

private:
    RTPlot *m_plot;
    int m_id;
    static int nextID;

};

#endif // RTPLOT_H
