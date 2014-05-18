#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

#include "gui_globals.h"

#include "qkcore.h"
#include "qkconnect.h"

#include "cproperty.h"
#include "cpropertybrowser.h"
#include "eventwidget.h"
#include "rtplot.h"

#include "plotsettings.h"
#include "ui_plotsettings.h"

#include "loggersettingswidget.h"
#include "ui_loggersettingswidget.h"

#include "debugsettingswidget.h"
#include "ui_debugsettingswidget.h"

#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QTime>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QLayout>
#include <QPalette>
#include <QPixmap>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QkExplorerWidget::QkExplorerWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkExplorerWidget)
{
    m_conn = 0;
    m_debugPrintTime = false;
    m_debugPrintSource = false;

    m_modeFlags = 0;

    m_selNode = 0;
    m_selBoardType = sbtUnknown;

    QFontDatabase::addApplicationFont("://fonts/Ubuntu-R.ttf");
    QFontDatabase::addApplicationFont("://fonts/OpenSans-Regular.ttf");
    QFontDatabase::addApplicationFont("://fonts/PTSans.ttf");
    QFontDatabase::addApplicationFont("://fonts/DejaVuSans.ttf");

    m_outputWindow = new QMainWindow(this);
    m_outputWindow->setWindowTitle(tr("Messages"));
    m_outputText = new QTextEdit(m_outputWindow);
    m_outputText->setFont(GUI_MONOFONT);
    m_outputText->setReadOnly(true);
    //m_outputText->setWordWrapMode(QTextOption::NoWrap);
    m_outputWindow->setCentralWidget(m_outputText);
    m_outputWindow->setMinimumWidth(500);
    m_outputWindow->hide();

    setupLayout();
    setupConnections();
    setup();
    updateInterface();
}

QkExplorerWidget::~QkExplorerWidget()
{
    delete ui;
}

void QkExplorerWidget::setModeFlags(int flags)
{
    m_modeFlags = flags;

    updateInterface();
}

void QkExplorerWidget::setup()
{
    slotReloadSerialPorts();
    ui->plottingWidget->setLayout(new QVBoxLayout);
    QLayout *layout = ui->plottingWidget->layout();
    layout->setMargin(0);
    layout->setSpacing(0);
    slotViewer_addPlot();
}

void QkExplorerWidget::setupLayout()
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->statusBar->hide();
    QFont debugFont(GUI_MONOFONT);
    debugFont.setPointSize(9);
    ui->debugText->setFont(debugFont);
    ui->explorerTabs->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);

    QHeaderView *header;
    header = ui->eventTable->verticalHeader();
    header->show();


    setWindowTitle("qkexplorer");
    updateInterface();
}

void QkExplorerWidget::setupConnections()
{
    connect(ui->buttonReloadSerialPorts, SIGNAL(clicked()),
            this, SLOT(slotReloadSerialPorts()));
    connect(ui->buttonConnect, SIGNAL(clicked()),
            this, SLOT(slotConnect()));

    connect(ui->comboPort, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSetSerialPortName()));

    connect(ui->search_button, SIGNAL(clicked()),
            this, SLOT(slotSearch()));
    connect(ui->start_button, SIGNAL(clicked()),
            this, SLOT(slotStart()));
    connect(ui->stop_button, SIGNAL(clicked()),
            this, SLOT(slotStop()));

    connect(ui->explorerTabs, SIGNAL(currentChanged(int)),
            this, SLOT(updateInterface()));

    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(slotExplorerListRowChanged(int)));
    /*connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(slotBoardPanels_reload()));*/
    connect(ui->comboBoardType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotBoardTypeChanged()));

    connect(ui->logger_buttonSettings, SIGNAL(clicked(bool)), ui->loggerSettings, SLOT(setVisible(bool)));
    connect(ui->loggerSettings->ui->buttonClear, SIGNAL(clicked()),
            ui->eventTable, SLOT(removeAll()));
    connect(ui->loggerSettings->ui->buttonEnable, SIGNAL(clicked(bool)),
            this, SLOT(slotLogger_setEnabled(bool)));
    connect(ui->loggerSettings->ui->buttonEnable, SIGNAL(clicked()),
            this, SLOT(updateInterface()));

    connect(ui->debug_buttonSettings, SIGNAL(clicked(bool)), ui->debugSettings, SLOT(setVisible(bool)));
    connect(ui->debugSettings->ui->buttonClear, SIGNAL(clicked()),
            ui->debugText, SLOT(clear()));
    connect(ui->debugSettings->ui->buttonEnable, SIGNAL(clicked(bool)),
            this, SLOT(slotDebug_setEnabled(bool)));
    connect(ui->debugSettings->ui->buttonEnable, SIGNAL(clicked()),
            this, SLOT(updateInterface()));
    connect(ui->debugSettings->ui->checkTimestamp, SIGNAL(clicked()),
            this, SLOT(slotDebug_updateOptions()));
    connect(ui->debugSettings->ui->checkSource, SIGNAL(clicked()),
            this, SLOT(slotDebug_updateOptions()));

    connect(ui->viewer_checkGlobal, SIGNAL(clicked(bool)), ui->plotSettings, SLOT(setGlobal(bool)));
    connect(ui->viewer_buttonSettings, SIGNAL(clicked(bool)), ui->plotSettings, SLOT(setVisible(bool)));
    connect(ui->viewer_buttonSettings, SIGNAL(clicked(bool)), ui->viewer_checkGlobal, SLOT(setVisible(bool)));
    connect(ui->viewer_buttonAddPlot, SIGNAL(clicked()),this, SLOT(slotViewer_addPlot()));
    connect(ui->viewer_buttonRemovePlot, SIGNAL(clicked()),this, SLOT(slotViewer_removePlot()));
    connect(ui->viewer_comboPlot, SIGNAL(currentIndexChanged(int)),this, SLOT(slotViewer_currentPlotChanged(int)));

    PlotSettings *plotSettings = ui->plotSettings;
    connect(plotSettings->ui->comboNode, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(slotViewer_nodeChanged(QString)));

    connect(plotSettings->ui->buttonAddWaveform, SIGNAL(clicked()),
            this, SLOT(slotViewer_addWaveform()));
    connect(plotSettings->ui->buttonRemoveWaveform, SIGNAL(clicked()),
            this, SLOT(slotViewer_removeWaveform()));
}

void QkExplorerWidget::setCurrentConnection(QkConnection *conn)
{
    if(conn == 0)
    {
        qDebug() << __FUNCTION__ << "conn == 0";
        return;
    }

    QkCore *qk;
    if(m_conn != 0)
    {
        qk = m_conn->qk();
        disconnect(qk, SIGNAL(error(int,int)), this, SLOT(showError(int,int)));
        disconnect(qk, SIGNAL(infoChanged(int,QkBoard::Type,int)), this, SLOT(slotNodeUpdated(int)));
        disconnect(qk, SIGNAL(deviceFound(int)), this, SLOT(slotNodeFound(int)));
        disconnect(qk, SIGNAL(deviceUpdated(int)), this, SLOT(slotNodeUpdated(int)));
        disconnect(qk, SIGNAL(dataReceived(int)), this, SLOT(slotDataReceived(int)));
        //disconnect(qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(slotLogger_append(int,QkDevice::Event)));
        disconnect(qk, SIGNAL(eventReceived(int)), this, SLOT(slotLogger_append(int)));
        disconnect(qk, SIGNAL(debugString(int,QString)), this, SLOT(slotDebug_log(int,QString)));

        disconnect(m_conn, SIGNAL(connected()), this, SLOT(updateInterface()));
        disconnect(m_conn, SIGNAL(disconnected()), this, SLOT(updateInterface()));
    }

    m_conn = conn;
    qk = m_conn->qk();
    connect(qk, SIGNAL(error(int,int)), this, SLOT(showError(int,int)));
    connect(qk, SIGNAL(infoChanged(int,QkBoard::Type,int)), this, SLOT(slotNodeUpdated(int)));
    connect(qk, SIGNAL(commFound(int)), this, SLOT(slotNodeFound(int)));
    connect(qk, SIGNAL(commUpdated(int)), this, SLOT(slotNodeUpdated(int)));
    connect(qk, SIGNAL(deviceFound(int)), this, SLOT(slotNodeFound(int)));
    connect(qk, SIGNAL(deviceUpdated(int)), this, SLOT(slotNodeUpdated(int)));
    connect(qk, SIGNAL(dataReceived(int)), this, SLOT(slotDataReceived(int)));
    //connect(qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(slotLogger_append(int,QkDevice::Event)));
    connect(qk, SIGNAL(eventReceived(int)), this, SLOT(slotLogger_append(int)));
    connect(qk, SIGNAL(debugString(int,QString)), this, SLOT(slotDebug_log(int,QString)));

    connect(m_conn, SIGNAL(connected()), this, SLOT(updateInterface()));
    connect(m_conn, SIGNAL(disconnected()), this, SLOT(updateInterface()));

    updateInterface();
}

void QkExplorerWidget::slotExplorerListRowChanged(int row)
{
    if(row < 0)
    {
        m_selNode = 0;
        return;
    }
    QString itemText = ui->explorerList->item(row)->text();
    if(itemText.contains(tr("Node")))
    {
        m_selBoardType = sbtCommDevice;
        bool ok;
        QString addrStr = itemText.split(' ').at(1);
        int addr = addrStr.toInt(&ok, 16);
        m_selNode = m_conn->qk()->node(addr);

        ui->comboBoardType->clear();
        if(m_selNode->comm() != 0)
            ui->comboBoardType->addItem("qkcomm");
        if(m_selNode->device() != 0)
            ui->comboBoardType->addItem("qkdevice");
    }
    else
    {
        m_selBoardType = sbtGatewayNetwork;
        m_selNode = 0;
    }
    updateInterface();
}


void QkExplorerWidget::slotDataReceived(int address)
{
    if(m_selNode != 0 && m_selNode->address() == address)
    {
        ui->boardPanel->refreshData();
    }

    QkNode *node = m_conn->qk()->node(address);
    if(node == 0 || node->device() == 0)
    {
        qWarning() << __FUNCTION__ << "node == 0 || node->device() == 0";
        return;
    }

    foreach(AddressDataPair *addrDataPair, m_waveformMapper.keys())
    {
        if(addrDataPair->address == address)
        {
            Waveform *wf = m_waveformMapper.value(addrDataPair);
            RTPlot *plot = m_plotMapper.value(wf);
            QVector<QkDevice::Data> dataArray = m_conn->qk()->node(address)->device()->data();
            QkDevice::Data data = dataArray[addrDataPair->dataIdx];
            plot->addData(wf, data.value(), data.timestamp());
        }
    }
}

void QkExplorerWidget::slotNodeFound(int address)
{
    QString addrStr = QString().sprintf("%04X", address);

    if(explorerList_findNode(address) < 0)
    {
        ui->explorerList->addItem(tr("Node ") + addrStr);
        if(ui->explorerList->count() == 1)
            ui->explorerList->setCurrentRow(0);
    }

    if(ui->explorerList->count() > 0 && ui->stackedWidget->currentIndex() == 0)
        ui->stackedWidget->setCurrentIndex(1);

    PlotSettings *plotSettings = ui->plotSettings;
    plotSettings->ui->comboNode->addItem(addrStr);
}


void QkExplorerWidget::slotNodeUpdated(int address)
{
    qDebug() << __FUNCTION__;

    if(m_selBoardType == sbtCommDevice)
    {
        if(m_selNode != 0 && m_selNode->address() == address)
            slotBoardPanels_reload();

        slotViewer_nodeChanged(address);
    }
}

void QkExplorerWidget::slotBoardTypeChanged()
{
    if(m_selNode == 0)
        return;
    if(m_selBoardType == sbtCommDevice)
    {
        if(ui->comboBoardType->currentText().toLower().contains("comm"))
            ui->boardPanel->setBoard(m_selNode->comm(), QkBoard::btComm, m_conn);
        else if(ui->comboBoardType->currentText().toLower().contains("device"))
            ui->boardPanel->setBoard(m_selNode->device(), QkBoard::btDevice, m_conn);
    }
    ui->boardPanel->reload();
    ui->boardPanel->refresh();
}

void QkExplorerWidget::slotBoardPanels_reload()
{    
    if(ui->explorerList->currentRow() < 0)
        return;

    if(m_selBoardType == sbtCommDevice)
    {
        if(m_selNode == 0)
            return;

        if(m_selNode->comm() != 0)
        {
            ui->boardPanel->setBoard(m_selNode->comm(), QkBoard::btComm, m_conn);
            ui->boardPanel->reload();
            ui->boardPanel->refresh();
        }

        if(m_selNode->device() != 0)
        {
            ui->boardPanel->setBoard(m_selNode->device(), QkBoard::btDevice, m_conn);
            ui->boardPanel->reload();
            ui->boardPanel->refresh();
        }
    }
}


int QkExplorerWidget::explorerList_findNode(int address)
{
    int row;

    for(row = 0; row < ui->explorerList->count(); row++)
    {
        QString itemText = ui->explorerList->item(row)->text();
        if(itemText.contains(tr("Node")))
        {
            bool ok;
            QString addrStr = itemText.split(' ').at(1);
            int addr = addrStr.toInt(&ok, 16);
            if(addr == address)
                return row;
        }
    }
    return -1;
}

void QkExplorerWidget::slotReloadSerialPorts()
{
    QStringList list;
    foreach(QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        QString portName = info.portName();
        if(portName.contains("ACM") || portName.contains("USB"))
            list.append(portName);
    }
    ui->comboPort->clear();
    ui->comboPort->addItems(list);
}

void QkExplorerWidget::slotSetSerialPortName()
{
    if(ui->comboPort->currentIndex() >= 0)
    {
        QString portName = ui->comboPort->currentText();
        if(portName != "" && m_conn != 0)
        {
            QkSerialConnection *serialConn = (QkSerialConnection*)m_conn;
            serialConn->setPortName(portName);
        }
    }
}

void QkExplorerWidget::slotConnect()
{
//    qDebug() << "slotConnect()";
//    if(m_conn->isConnected())
//        m_conn->close();
//    else
//        m_conn->open();
//    updateInterface();

    qDebug() << "slotConnect()";

    if(m_conn->isConnected())
    {
        ui->statusBar->showMessage(tr("Disconnecting..."));
        m_conn->close();
        ui->statusBar->showMessage(tr("Disconnected"), 1000);
    }
    else
    {
        if(m_conn->descriptor().type == QkConnection::ctSerial)
        {
            QkSerialConnection *serialConn = (QkSerialConnection*)m_conn;
            serialConn->setBaudRate(38400);
            //serialConn->setBaudRate(115200);
//            int baudRate = ui->comboBaud->currentText().toInt();
//            serialConn->setBaudRate(baudRate);
            serialConn->setPortName(ui->comboPort->currentText());
        }
        qDebug() << "connecting...";
        ui->statusBar->showMessage(tr("Connecting"));
        if(m_conn->open())
        {
            qDebug() << "connected";
            ui->statusBar->showMessage(tr("Connected"), 1000);
        }
        else
            ui->statusBar->clearMessage();
    }
    updateInterface();
}

void QkExplorerWidget::slotSearch()
{
    ui->stackedWidget->setCurrentIndex(0);

    ui->explorerList->clear();

    PlotSettings *plotSettings = ui->plotSettings;
    plotSettings->ui->comboNode->clear();

    m_conn->qk()->search();
}

void QkExplorerWidget::slotStart()
{
    m_conn->qk()->start();
    updateInterface();
    foreach(RTPlotDock *plotDock, m_plotDockMapper.values())
        plotDock->plot()->start();
}

void QkExplorerWidget::slotStop()
{
    m_conn->qk()->stop();
    updateInterface();
    foreach(RTPlotDock *plotDock, m_plotDockMapper.values())
        plotDock->plot()->stop();
}

void QkExplorerWidget::slotClear()
{
    ui->debugText->clear();
}

void QkExplorerWidget::slotDebug_log(int address, QString debugStr)
{
    if(!ui->debugSettings->ui->buttonEnable->isChecked())
        return;

    QString str;
    if(m_debugPrintTime)
        str.append(QTime::currentTime().toString("hh:mm:ss") + " ");
    if(m_debugPrintSource)
        str.append("[" + QString().sprintf("%04X", address) + "] ");
    str.append(debugStr);
    ui->debugText->append(str);
}

void QkExplorerWidget::slotDebug_updateOptions()
{
    m_debugPrintTime = ui->debugSettings->ui->checkTimestamp->isChecked();
    m_debugPrintSource = ui->debugSettings->ui->checkSource->isChecked();
}

void QkExplorerWidget::slotDebug_setEnabled(bool enabled)
{
    if(enabled)
        ui->label_debugEnabled->setPixmap(QPixmap(":/icons/on_16.png"));
    else
        ui->label_debugEnabled->setPixmap(QPixmap(":/icons/off_16.png"));
}

void QkExplorerWidget::slotLogger_append(int address, QkDevice::Event event)
{
    if(!ui->loggerSettings->ui->buttonEnable->isChecked())
        return;

    int r = ui->eventTable->addRow();

    QTableWidgetItem *item = new QTableWidgetItem();
    EventWidget *eventWidget = new EventWidget();
    eventWidget->setEvent(&event, address);
    ui->eventTable->setItem(r, 0, item);
    ui->eventTable->setCellWidget(r, 0, eventWidget);

    ui->eventTable->scrollToBottom();
}

void QkExplorerWidget::slotLogger_append(int address)
{
    if(!ui->loggerSettings->ui->buttonEnable->isChecked())
        return;

    QkCore *qk = m_conn->qk();
    QQueue<QkDevice::Event> *events = qk->node(address)->device()->eventsFired();
    while(events->count() > 0)
        slotLogger_append(address, events->dequeue());
}

void QkExplorerWidget::slotLogger_setEnabled(bool enabled)
{
    if(enabled)
        ui->label_loggerEnabled->setPixmap(QPixmap(":/icons/on_16.png"));
    else
        ui->label_loggerEnabled->setPixmap(QPixmap(":/icons/off_16.png"));
}

void QkExplorerWidget::slotViewer_addPlot()
{
    QLayout *layout = ui->plottingWidget->layout();
    RTPlotDock *plotDock = createPlotDock();
    layout->addWidget(plotDock);
    ui->viewer_comboPlot->addItem(plotDock->windowTitle(), plotDock->id());
    ui->viewer_comboPlot->setCurrentIndex(ui->viewer_comboPlot->count()-1);
}

void QkExplorerWidget::slotViewer_removePlot()
{
    int currentPlotIdx = ui->viewer_comboPlot->currentIndex();
    int plotDockID = ui->viewer_comboPlot->itemData(currentPlotIdx).toInt();
    RTPlotDock *plotDock = m_plotDockMapper.value(plotDockID);
    RTPlot *plot = plotDock->plot();

    foreach(Waveform *wf, plot->waveforms())
    {
        m_waveformMapper.remove(m_waveformMapper.key(wf));
        m_plotMapper.remove(wf);
    }
    plot->removeWaveforms();
    m_plotDockMapper.remove(plotDock->id());
    delete plotDock;

    ui->viewer_comboPlot->removeItem(currentPlotIdx);
}

void QkExplorerWidget::slotViewer_addWaveform()
{   
    qDebug() << "slotViewer_addWaveform()";
    int currentPlotIdx = ui->viewer_comboPlot->currentIndex();
    int plotDockID = ui->viewer_comboPlot->itemData(currentPlotIdx).toInt();
    RTPlotDock *plotDock = m_plotDockMapper.value(plotDockID);
    RTPlot *plot = plotDock->plot();

    PlotSettings *plotSettings = ui->plotSettings;
    QString addrStr = plotSettings->ui->comboNode->currentText();
    bool ok;
    int addr = addrStr.toInt(&ok, 16);

    int dataIdx = plotSettings->ui->comboData->currentIndex();
    if(dataIdx >= 0)
    {
        AddressDataPair *addrDataPair = new AddressDataPair;
        addrDataPair->address = addr;
        addrDataPair->dataIdx = dataIdx;

        QkNode *node = m_conn->qk()->node(addr);
        QkDevice::Data data = node->device()->data().at(dataIdx);

        Waveform *wf = plot->addWaveform(data.label());
        m_waveformMapper.insert(addrDataPair, wf);
        m_plotMapper.insert(wf, plot);
    }

    plotSettings->setCurrentPlotDock(plotDock);

    int nextPlotIdx = (dataIdx + 1) % plotSettings->ui->comboData->count();
    plotSettings->ui->comboData->setCurrentIndex(nextPlotIdx);
}

void QkExplorerWidget::slotViewer_removeWaveform()
{
    int currentPlotIdx = ui->viewer_comboPlot->currentIndex();
    int plotDockID = ui->viewer_comboPlot->itemData(currentPlotIdx).toInt();
    RTPlotDock *plotDock = m_plotDockMapper.value(plotDockID);
    RTPlot *plot = plotDock->plot();

    PlotSettings *plotSettings = ui->plotSettings;
    pTableWidget *table = plotSettings->ui->tableWaveforms;
    int row = table->currentRow();
    if(row < 0) return;

    int id = table->item(row, PlotSettings::ColumnWaveformID)->text().toInt();

    Waveform *wf = plot->waveform(id);
    m_waveformMapper.remove(m_waveformMapper.key(wf));
    m_plotMapper.remove(wf);
    plot->removeWaveform(id);

    table->removeRow(row);
}

void QkExplorerWidget::slotViewer_nodeChanged(int address)
{
    PlotSettings *plotSettings = ui->plotSettings;

    QkNode *node = m_conn->qk()->node(address);
    plotSettings->ui->comboData->clear();
    if(node->device() != 0)
    {
        foreach(QkDevice::Data data, node->device()->data())
            plotSettings->ui->comboData->addItem(data.label());
    }
}

void QkExplorerWidget::slotViewer_nodeChanged(QString addrStr)
{
    bool ok;
    int addr = addrStr.toInt(&ok, 16);
    slotViewer_nodeChanged(addr);
}

void QkExplorerWidget::slotViewer_dockSelected(int  id)
{
    int i;
    for(i=0; i < ui->viewer_comboPlot->count(); i++)
    {
        int plotDockID = ui->viewer_comboPlot->itemData(i).toInt();
        if(plotDockID == id)
            ui->viewer_comboPlot->setCurrentIndex(i);
    }

}

void QkExplorerWidget::slotViewer_currentPlotChanged(int idx)
{
    m_currentPlotDock = 0;

    if(ui->viewer_comboPlot->count() > 0)
    {
        int plotDockID = ui->viewer_comboPlot->itemData(idx).toInt();
        m_currentPlotDock = m_plotDockMapper.value(plotDockID);
    }

    if(m_currentPlotDock != 0)
        ui->plotSettings->setCurrentPlotDock(m_currentPlotDock);

    updateInterface();
}

void QkExplorerWidget::slotViewer_plotTitleChanged(int id, QString title)
{
    for(int i=0; i < ui->viewer_comboPlot->count(); i++)
        if(ui->viewer_comboPlot->itemData(i) == id)
            ui->viewer_comboPlot->setItemText(i, title);
}

RTPlotDock *QkExplorerWidget::createPlotDock()
{
    RTPlotDock *plotDock = new RTPlotDock(new RTPlot, this);
    connect(plotDock, SIGNAL(dockSelected(int)), this, SLOT(slotViewer_dockSelected(int)));
    connect(plotDock, SIGNAL(titleChanged(int,QString)), this, SLOT(slotViewer_plotTitleChanged(int,QString)));
    m_plotDockMapper.insert(plotDock->id(), plotDock);
    return plotDock;
}

void QkExplorerWidget::updateInterface()
{
    bool enableButtons = (m_conn != 0 && m_conn->isConnected() ? true : false);
    ui->search_button->setEnabled(enableButtons);
    ui->start_button->setEnabled(enableButtons);
    ui->stop_button->setEnabled(enableButtons);

    bool removePlotEnabled = (ui->viewer_comboPlot->count() > 1);
    ui->viewer_buttonRemovePlot->setEnabled(removePlotEnabled);

    bool modeSingleConnection = ((m_modeFlags & mfSingleConnection) ? true : false);
    ui->buttonReloadSerialPorts->setVisible(modeSingleConnection);
    ui->comboPort->setVisible(modeSingleConnection);
    ui->buttonConnect->setVisible(modeSingleConnection);

    bool modeSingleNode = ((m_modeFlags & mfSingleNode) ? true : false);
    ui->plotSettings->ui->comboNode->setVisible(!modeSingleNode);

    if(modeSingleNode)
    {
        ui->label->setHidden(modeSingleNode);
        ui->explorerList->setHidden(modeSingleNode);
    }
    else
    {
        bool nothingToShow;
        if(ui->explorerList->count() == 0)
            nothingToShow = true;
        else
            nothingToShow = false;
        ui->label->setHidden(nothingToShow);
        ui->explorerList->setHidden(nothingToShow);
    }

    bool connected = (m_conn != 0 && m_conn->isConnected() ? true : false);
    if(connected)
    {
        ui->buttonConnect->setText(tr("Connected"));
        QPalette buttonPalette(QColor("#00b460"));
        ui->buttonConnect->setPalette(buttonPalette);
        ui->comboPort->setDisabled(true);
        ui->buttonReloadSerialPorts->setDisabled(true);
    }
    else
    {
        ui->buttonConnect->setText(tr("Disconnected"));
        QPalette buttonPalette(QColor("#ef6565"));
        ui->buttonConnect->setPalette(buttonPalette);
        ui->comboPort->setDisabled(false);
        ui->buttonReloadSerialPorts->setDisabled(false);
    }

    if(m_conn != 0)
    {
        if(!m_conn->isConnected())
            ui->status_label->setVisible(false);
        else
        {
            if(m_conn->qk()->isRunning())
            {
                ui->status_label->setText(tr("Running"));
                QString style = "QLabel { background: #00b460; color: white;}";
                ui->status_label->setStyleSheet(style);
            }
            else
            {
                ui->status_label->setText(tr("Stopped"));
                QString style = "QLabel { background: #ef6565; color: white;}";
                ui->status_label->setStyleSheet(style);
            }
            ui->status_label->setVisible(true);
        }
    }

}

void QkExplorerWidget::showError(int code, int arg)
{
    showError(QkCore::errorMessage(code) + " " + QString().sprintf("(%d)",arg));
}

void QkExplorerWidget::showError(const QString &message)
{
//    QMessageBox::critical(this, tr("Error"), message);
    QString timestamp = QDateTime::currentDateTime().time().toString("hh:mm:ss");
    m_outputText->append(timestamp + tr(" [ERROR] ") + message);
    m_outputWindow->show();
    qDebug() << "ERROR:" << message;
}
