#include "plotsettings.h"
#include "ui_plotsettings.h"

#include "rtplot.h"
#include "ptablewidget.h"

#include <QHeaderView>
#include <QPixmap>

PlotSettings::PlotSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotSettings)
{
    ui->setupUi(this);

    ui->tableWaveforms->setColumnCount(ColumnWaveformCount);

    QHeaderView *header = ui->tableWaveforms->horizontalHeader();

    header->setSectionResizeMode(ColumnWaveformName, QHeaderView::Stretch);
    header->setSectionResizeMode(ColumnWaveformColor, QHeaderView::Fixed);

    ui->tableWaveforms->setColumnHidden(ColumnWaveformID, true);
    ui->tableWaveforms->setColumnWidth(ColumnWaveformColor, 64);

    setupConnections();
}

PlotSettings::~PlotSettings()
{
    delete ui;
}

void PlotSettings::setupConnections()
{
    connect(ui->lineTitle, SIGNAL(textChanged(QString)), this, SLOT(slotSetTitle(QString)));
    connect(ui->spinTimeWindow, SIGNAL(valueChanged(int)), this, SLOT(slotSetTimeWindow(int)));
    connect(ui->checkShowTitle, SIGNAL(clicked(bool)), this, SLOT(slotShowHideTitle(bool)));
    connect(ui->checkShowLegend, SIGNAL(clicked(bool)), this, SLOT(slotShowHideLegend(bool)));

    connect(ui->tableWaveforms, SIGNAL(currentCellChanged(int,int,int,int)),
            this, SLOT(updateInterface()));
    connect(ui->comboData, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateInterface()));
}

void PlotSettings::setCurrentPlotDock(RTPlotDock *plotDock)
{
    m_currentPlotDock = plotDock;
    reloadInterface();
}

void PlotSettings::addWaveformToTable(Waveform *wf)
{
    int row = ui->tableWaveforms->addRow();
    ui->tableWaveforms->setItem(row, ColumnWaveformID, new QTableWidgetItem(QString::number(wf->id())));
    ui->tableWaveforms->setItem(row, ColumnWaveformName, new QTableWidgetItem(wf->name()));

    QTableWidgetItem *color = new QTableWidgetItem;
    color->setBackground(QBrush(wf->color()));
    ui->tableWaveforms->setItem(row, ColumnWaveformColor, color);
}

void PlotSettings::setGlobal(bool enabled)
{
    bool global = enabled;
    ui->groupWaveforms->setHidden(global);
    ui->labelTitle->setHidden(global);
    ui->lineTitle->setHidden(global);
}

void PlotSettings::slotSetTitle(const QString &title)
{
    m_currentPlotDock->setWindowTitle(title);
}

void PlotSettings::slotSetTimeWindow(int timeWindow)
{
    m_currentPlotDock->plot()->setTimeWindow(timeWindow);
}

void PlotSettings::slotShowHideTitle(bool show)
{

}

void PlotSettings::slotShowHideLegend(bool show)
{
    m_currentPlotDock->plot()->legend->setVisible(show);
}

void PlotSettings::reloadInterface()
{
    RTPlotDock *plotDock = m_currentPlotDock;
    RTPlot *plot = plotDock->plot();

    ui->tableWaveforms->removeAll();
    foreach(Waveform *wf, plot->waveforms())
        addWaveformToTable(wf);

    ui->lineTitle->setText(plotDock->windowTitle());
    ui->spinTimeWindow->setValue(plot->timeWindow());

    updateInterface();
}

void PlotSettings::updateInterface()
{
    if(ui->comboData->count() > 0)
        ui->buttonAddWaveform->setEnabled(true);
    else
        ui->buttonAddWaveform->setEnabled(false);

    if(ui->tableWaveforms->selectedItems().isEmpty())
        ui->buttonRemoveWaveform->setEnabled(false);
    else
        ui->buttonRemoveWaveform->setEnabled(true);
}
