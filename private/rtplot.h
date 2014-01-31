#ifndef RTPLOT_H
#define RTPLOT_H

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
    Waveform* waveform(int id);
    QList<Waveform*> waveforms();
    int timeWindow();
    double amplitudeMin() { return yAxis->range().minRange; }
    double amplitudeMax() { return yAxis->range().minRange; }
    bool autoscale() { return m_autoScale; }

signals:

public slots:
    void setTimeWindow(int sec);
    void setAmplitude(double min, double max);
    void setAutoscale(bool enabled) { m_autoScale = enabled; }
    void start();
    void stop();
    void addData(double data, int id);
    void addData(double data, Waveform *wf);

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
    double _elapsedSeconds();
    QColor _pickWaveformColor();
    void mousePressEvent(QMouseEvent *event);

    int m_id;
    double m_timeWindow_sec;
    int m_nextColorIdx;
    QVector<QColor> m_defaultColors;
    QElapsedTimer m_clock;
    QTimer m_timer;
    QMap<int, Waveform*> m_waveforms;
    //bool m_replotAfterAdd;
    bool m_autoScale;
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

signals:
    void dockSelected(int);
    void titleChanged(int,QString);

private:
    RTPlot *m_plot;
    int m_id;
    static int nextID;

};

#endif // RTPLOT_H
