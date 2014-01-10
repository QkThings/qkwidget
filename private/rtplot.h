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
    void setTimeWindow(int sec);
    int timeWindow();

signals:

public slots:
    void start();
    void stop();
    void addData(double data, int id);
    void addData(double data, Waveform *wf);

private slots:


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
    QMap<int, Waveform*> m_waveforms;
    bool m_replotAfterAdd;
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
