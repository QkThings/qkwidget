#include "plotsettings.h"
#include "ui_plotsettings.h"

#include "rtplot.h"
#include "ptablewidget.h"

#include <QHeaderView>
#include <QPixmap>
#include <QFileDialog>

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
    connect(ui->checkStopAtEnd, SIGNAL(clicked(bool)), this, SLOT(slotSetStopAtEnd(bool)));
    connect(ui->buttonShowX, SIGNAL(clicked()), this, SLOT(slotShowHideAxis()));
    connect(ui->buttonShowY, SIGNAL(clicked()), this, SLOT(slotShowHideAxis()));
    connect(ui->checkAutoscale, SIGNAL(clicked(bool)), this, SLOT(slotSetAutoscale(bool)));
    connect(ui->spinAmplitudeMin, SIGNAL(valueChanged(double)), this, SLOT(slotSetAmplitude()));
    connect(ui->spinAmplitudeMax, SIGNAL(valueChanged(double)), this, SLOT(slotSetAmplitude()));
    connect(ui->tableWaveforms, SIGNAL(currentCellChanged(int,int,int,int)),
            this, SLOT(updateInterface()));
    connect(ui->comboData, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateInterface()));
    connect(ui->comboSize, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateInterface()));

    connect(ui->buttonBrowse, SIGNAL(clicked()), this, SLOT(slotBrowseOutputFolder()));
    connect(ui->buttonExport, SIGNAL(clicked()), this, SLOT(slotExport()));

    connect(ui->lineFilename, SIGNAL(textChanged(QString)), this, SLOT(updateInterface()));
    connect(ui->lineOutFolder, SIGNAL(textChanged(QString)), this, SLOT(updateInterface()));
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

void PlotSettings::slotSetAmplitude()
{
    double min = ui->spinAmplitudeMin->value();
    double max = ui->spinAmplitudeMax->value();
    m_currentPlotDock->plot()->setAmplitude(min,max);
}

void PlotSettings::slotSetTimeWindow(int timeWindow)
{
    m_currentPlotDock->plot()->setTimeWindow(timeWindow);
}

void PlotSettings::slotSetAutoscale(bool enabled)
{
    m_currentPlotDock->plot()->setAutoscale(enabled);
    reloadInterface();
}

void PlotSettings::slotSetStopAtEnd(bool enabled)
{
    m_currentPlotDock->plot()->setStopAtEnd(enabled);
}

void PlotSettings::slotShowHideTitle(bool show)
{
    m_currentPlotDock->plot()->showTitle(show);
}

void PlotSettings::slotShowHideLegend(bool show)
{
    m_currentPlotDock->plot()->legend->setVisible(show);
    m_currentPlotDock->plot()->replot();
}

void PlotSettings::slotShowHideAxis()
{
    bool showX = !ui->buttonShowX->isChecked();
    bool showY = !ui->buttonShowY->isChecked();
    m_currentPlotDock->plot()->showAxis(showX, showY);
}

void PlotSettings::slotExport()
{
    RTPlot *plot = m_currentPlotDock->plot();
    ExportFormat ef = (ExportFormat)ui->comboFormat->currentIndex();

    int width, height;

    switch(ui->comboSize->currentIndex())
    {
    case 0: width = 800; height = 600; break;
    case 1: width = 1200; height = 700; break;
    default:
        width = ui->spinW->value(); height = ui->spinH->value();
    }

    QString output = ui->lineOutFolder->text() + "/" + ui->lineFilename->text();

    switch(ef)
    {
    case efPDF:
        output.append(".pdf");
        plot->savePdf(output, true, width, height);
        break;
    case efPNG:
        output.append(".png");
        plot->savePng(output, width, height);
        break;
    case efCSV:
        output.append(".csv");
        exportToCsv(plot, output);
    }
}

void PlotSettings::slotBrowseOutputFolder()
{
    ui->lineOutFolder->setText(QFileDialog::getExistingDirectory(this, tr("Export to...")));
}

void PlotSettings::exportToCsv(RTPlot *plot, const QString &outputPath)
{
    QFile outFile(outputPath);

    if(plot->waveforms().count() == 0)
        return;

    if(!outFile.open(QFile::WriteOnly))
    {
        qDebug() << "Failed to open file";
        return;
    }

    QTextStream outStream(&outFile);

    int minCount=999999, maxCount=0;
    foreach(Waveform *wf, plot->waveforms())
    {
        int count = wf->graph->data()->count();
        maxCount = ((count > maxCount) ? count : maxCount);
        minCount = ((count < minCount) ? count : minCount);
    }

    qDebug() << "maxCount" << maxCount << "minCount" << minCount;

    QList<double> keys = plot->waveforms().at(0)->graph->data()->keys();


    while(keys.count() > minCount)
        keys.removeLast();

    for(int i=0; i < keys.count(); i++)
    {
        double key = keys[i];
        outStream << QString::number(key) << ",";
        for(int j=0; j < plot->waveforms().count(); j++)
        {
            Waveform *wf = plot->waveforms().at(j);
            outStream << QString::number(wf->graph->data()->value(key).value);
            if(j < (plot->waveforms().count() - 1))
                outStream << ",";
        }
        outStream << "\n";
    }

    outFile.close();
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
    ui->spinAmplitudeMin->setValue(plot->amplitudeMin());
    ui->spinAmplitudeMax->setValue(plot->amplitudeMax());
    ui->checkAutoscale->setChecked(plot->autoscale());

    updateInterface();
}

void PlotSettings::updateInterface()
{   
    if(ui->comboData->count() > 0)
        ui->buttonAddWaveform->setEnabled(true);
    else
        ui->buttonAddWaveform->setEnabled(false);

    if(ui->tableWaveforms->currentRow() >= 0)
        ui->buttonRemoveWaveform->setEnabled(true);
    else
        ui->buttonRemoveWaveform->setEnabled(false);

    bool amplitudeEnabled = !(m_currentPlotDock->plot()->autoscale());
    ui->spinAmplitudeMin->setEnabled(amplitudeEnabled);
    ui->spinAmplitudeMax->setEnabled(amplitudeEnabled);

    if(ui->lineFilename->text().isEmpty() || ui->lineOutFolder->text().isEmpty())
        ui->buttonExport->setEnabled(false);
    else
        ui->buttonExport->setEnabled(true);

    bool customSizeEnabled = (ui->comboSize->currentIndex() == (ui->comboSize->count()-1));
    ui->labelW->setVisible(customSizeEnabled);
    ui->labelH->setVisible(customSizeEnabled);
    ui->spinW->setVisible(customSizeEnabled);
    ui->spinH->setVisible(customSizeEnabled);
}
