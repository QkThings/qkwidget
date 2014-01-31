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
    m_timer.setInterval(50);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(replot()));

    m_nextColorIdx = 0;
    m_defaultColors.append(QColor("#0c50e0"));
    m_defaultColors.append(QColor("#ff312a"));
    m_defaultColors.append(QColor("#4d4d4d"));
    m_defaultColors.append(QColor("#00b460"));
    m_defaultColors.append(QColor("#c88400"));
    m_defaultColors.append(QColor("#a100e0"));
    m_defaultColors.append(QColor("#e00065"));

    _setup();
}

void RTPlot::_setup()
{
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);

    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, new QCPPlotTitle(this, "Interaction Example"));

    xAxis->setLabel("Time");
    yAxis->setLabel("Amplitude");

    xAxis->setRange(0.0, m_timeWindow_sec);
    yAxis->setRange(-1.0, 1.0);
    axisRect()->setupFullAxesBox();

    legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(slotMousePress()));
    connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(slotMouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(this, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
}

void RTPlot::titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title)
{
  Q_UNUSED(event)
  // Set the plot title by double clicking on it
  bool ok;
  QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
  if (ok)
  {
    title->setText(newTitle);
    replot();
  }
}

void RTPlot::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      replot();
    }
  }
}

void RTPlot::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      replot();
    }
  }
}

void RTPlot::slotMousePress()
{
        qDebug() << "slotMousePress()";
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    axisRect()->setRangeDrag(xAxis->orientation());
  else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    axisRect()->setRangeDrag(yAxis->orientation());
  else
    axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void RTPlot::slotMouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

//  if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    axisRect()->setRangeZoom(xAxis->orientation());
//  else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    axisRect()->setRangeZoom(yAxis->orientation());
//  else
//    axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void RTPlot::selectionChanged()
{
    qDebug() << "selectionChanged()";
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (xAxis->selectedParts().testFlag(QCPAxis::spAxis) || xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (yAxis->selectedParts().testFlag(QCPAxis::spAxis) || yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<graphCount(); ++i)
  {
    QCPGraph *g = graph(i);
    QCPPlottableLegendItem *item = legend->itemWithPlottable(g);
    if (item->selected() || g->selected())
    {
      item->setSelected(true);
      g->setSelected(true);
    }
  }
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

void RTPlot::setAmplitude(double min, double max)
{
    yAxis->setRange(min, max);
    replot();
}

int RTPlot::timeWindow()
{
    return m_timeWindow_sec;
}

void RTPlot::start()
{
    m_clock.restart();
    m_timer.start();
}

void RTPlot::stop()
{
    m_timer.stop();
    _clearAllWaveforms();
}

void RTPlot::addData(double data, Waveform *wf)
{
    double time = _elapsedSeconds();
    //qDebug() << "addData" << time << data;
    wf->graph->addData(time, data);
    if(m_autoScale)
        wf->graph->rescaleValueAxis(true, false);
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
