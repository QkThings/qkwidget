#include "rtplot.h"

#include <QDebug>

int Waveform::m_nextID = 0;
Waveform::Waveform(const QString &name, QObject *parent) :
    QObject(parent)
{
    m_id = m_nextID++;
    m_name = name;
}

int Waveform::id()
{
    return m_id;
}

void Waveform::setName(const QString &name)
{
    m_name = name;
    graph->setName(name);
}

QString Waveform::name()
{
    return m_name;
}

void Waveform::setColor(const QColor &color)
{
    if(graph != 0)
    {
        QPen pen = graph->pen();
        pen.setColor(color);
        graph->setPen(pen);
    }
}

QColor Waveform::color()
{
    return graph->pen().color();
}



RTPlot::RTPlot(QWidget *parent) :
    QCustomPlot(parent)
{
    m_timeWindow_sec = 10.0;
    m_clock.restart();
    m_replotAfterAdd = true;

    m_nextColorIdx = 0;
    m_defaultColors.append(Qt::blue);
    m_defaultColors.append(Qt::red);
    m_defaultColors.append(Qt::black);

    _setup();
}

void RTPlot::_setup()
{
    xAxis->setRange(0.0, m_timeWindow_sec);
    yAxis->setRange(-1.0, 1.0);
    axisRect()->setupFullAxesBox();
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));
}

Waveform* RTPlot::addWaveform(const QString &name, const QColor &color)
{
    Waveform *wf = new Waveform(name, this);
    wf->graph = addGraph();
    if(wf->graph != 0)
    {
        wf->graph->setName(name);

        QPen pen;
        /*if(color.isValid())
            pen.setColor(color);
        else
            pen.setColor(_pickWaveformColor());*/
        pen.setColor(_pickWaveformColor());
        //pen.setColor(Qt::black);
        pen.setWidth(2);
        wf->graph->setPen(pen);
        wf->setColor(pen.color());
    }
    else
    {
        qDebug() << "Unable to create graph";
    }
    m_waveforms.insert(wf->id(), wf);
    return wf;
}

Waveform* RTPlot::waveform(int id)
{
    return m_waveforms.value(id);
}

QList<Waveform*> RTPlot::waveforms()
{
    return m_waveforms.values();
}

void RTPlot::setTimeWindow(int sec)
{
    m_timeWindow_sec = sec;
    xAxis->setRange(0.0, m_timeWindow_sec);
    replot();
}

int RTPlot::timeWindow()
{
    return m_timeWindow_sec;
}

void RTPlot::start()
{
    m_clock.restart();
}

void RTPlot::stop()
{
    _clearAllWaveforms();
}

void RTPlot::addData(double data, Waveform *wf)
{
    double time = _elapsedSeconds();
    //qDebug() << "addData" << time << data;
    wf->graph->addData(time, data);
    wf->graph->rescaleValueAxis(true, true);

    if(m_replotAfterAdd)
        replot();
}

void RTPlot::addData(double data, int id)
{
    Waveform *wf = waveform(id);
    if(wf != 0)
        addData(data, wf);
}

void RTPlot::_clearAllWaveforms()
{
    int i;
    for(i=0; i < graphCount(); i++)
        graph(i)->clearData();
}

double RTPlot::_elapsedSeconds()
{
    double elapsed_sec = (double)m_clock.elapsed()/1000.0;
    if(elapsed_sec > m_timeWindow_sec)
    {
        m_clock.restart();
        elapsed_sec = 0.0;
        _clearAllWaveforms();
    }
    return elapsed_sec;
}

QColor RTPlot::_pickWaveformColor()
{
    int colorIdx = m_nextColorIdx++;
    if(m_nextColorIdx >= m_defaultColors.count())
        m_nextColorIdx = 0;

    return m_defaultColors.at(colorIdx);
}

void RTPlot::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "plot" << m_id << " focused";
}

int RTPlotDock::nextID = 0;

RTPlotDock::RTPlotDock(RTPlot *plot, QWidget *parent) :
    QDockWidget(parent)
{
    m_plot = plot;
    m_id = nextID++;
    setWidget(m_plot);
    //setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setFeatures(0);
    setMinimumHeight(200);
    setWindowTitle("Plot" + QString::number(m_id));
}

void RTPlotDock::setWindowTitle(const QString &title)
{
    QDockWidget::setWindowTitle(title);
    emit titleChanged(m_id, title);
}

RTPlot* RTPlotDock::plot()
{
    return m_plot;
}

int RTPlotDock::id()
{
    return m_id;
}

void RTPlotDock::mousePressEvent(QMouseEvent *e)
{
    QDockWidget::mousePressEvent(e);
    emit dockSelected(m_id);
}
