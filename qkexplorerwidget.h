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

#ifndef QKEXPLORERWIDGET_H
#define QKEXPLORERWIDGET_H

#include <QMainWindow>
#include <QDockWidget>
#include <QSignalMapper>

namespace Ui {
class QkExplorerWidget;
}

#include "qkcore.h"
#include "qkconnect.h"

class CProperty;
class CPropertyBrowser;
class RTPlotDock;
class RTPlot;
class Waveform;
class BoardPanel;

class QTextEdit;

class QkExplorerWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    enum ExplorerMode
    {
        ModeSingleNode = (1<<0),
        ModeSingleConnection = (1<<1)
    };
    typedef int ExplorerModes;

    enum ExplorerFeature
    {
        FeatureDockableWidgets = (1<<0)
    };
    typedef int ExplorerFeatures;

    explicit QkExplorerWidget(QWidget *parent = 0);
    ~QkExplorerWidget();

    void setModes(ExplorerModes modes);
    void setFeatures(ExplorerFeatures features);
    QList<QDockWidget *> docks();

public slots:
    void setConnection(QkConnection *conn);

private slots:
    void slotReloadSerialPorts();
    void slotSetSerialPortName();
    void slotConnectionStatus(int id, QkConnection::Status status);
    void slotCoreStatus(QkCore::Status status);
    void slotConnect();
    void slotSearch();
    void slotStart();
    void slotStop();
    void slotClear();
    void slotBoardTypeChanged();
    void slotBoardPanels_reload();
    //void slotExplorerList_reload();
    void slotNodeFound(int address);
    void slotExplorerListRowChanged(int row);
    void slotDataReceived(int address, QkDevice::DataArray dataArray);
    void slotNodeUpdated(int address);

    void slotDock(int id);

    void slotLogger_append(int address, QkDevice::Event event);
//    void slotLogger_append(int address);
    void slotLogger_setEnabled(bool enabled);

    void slotDebug_log(int address, QString debugStr);
    void slotDebug_updateOptions();
    void slotDebug_setEnabled(bool enabled);

    void slotViewer_addPlot();
    void slotViewer_removePlot();
    void slotViewer_removeAllPlots();
    void slotViewer_addWaveform();
    void slotViewer_removeWaveform();
    void slotViewer_nodeChanged(int address);
    void slotViewer_nodeChanged(QString addrStr);
    void slotViewer_dockSelected(int id);
    void slotViewer_currentPlotChanged(int idx);
    void slotViewer_plotTitleChanged(int id, QString title);
    RTPlotDock* createPlotDock();

    void showError(int code, int arg);
    void showError(const QString &message);
    void updateInterface();
    
private:
    enum StackedPanelIndex
    {
        spiHome = 0,
        spiExplorer = 1
    };
    enum SelectedBoardType
    {
        sbtGatewayNetwork,
        sbtCommDevice,
        sbtUnknown
    };
    enum TabIndex
    {
        TabDashboard,
        TabViewer,
        TabLogger,
        TabDebug,
        TabCOUNT
    };
    enum
    {
        LoggerColumnEventTimestamp,
        LoggerColumnEventSource,
        LoggerColumnEventLabel,
        LoggerColumnEventMessage,
        LoggerColumnEventArguments
    };
    enum
    {
        LoggerColumnNotificationTimestamp,
        LoggerColumnNotificationSource,
        LoggerColumnNotificationMessage,
        LoggerColumnNotificationArguments
    };

    class DockableWidget {
    public:
        DockableWidget(QWidget *parent) {
            dock = new QDockWidget(QString(), parent);
            dock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar |
                              QDockWidget::DockWidgetFloatable |
                              QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable);
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            dock->setFloating(true);
            dock->hide();
            docked = false;
        }
        ~DockableWidget()
        {
            delete dock;
        }
        QDockWidget *dock;
        QWidget *widget;
        bool docked;
    };

    class AddressDataPair {
    public:
        int address;
        int dataIdx;
    };

    void reset();
    void setupLayout();
    void setupConnections();
    void setDashboardMessage(const QString &title, const QString &message);
    int explorerList_findNode(int address);

    Ui::QkExplorerWidget *ui;
    QkConnection *m_conn;
    SelectedBoardType m_selBoardType;
    QkNode *m_selNode;

    QSignalMapper m_dockSignalMapper;

    QMap<int,RTPlotDock*> m_plotDockMapper;
    QMap<Waveform*,RTPlot*> m_plotMapper;
    QMap<AddressDataPair*, Waveform*> m_waveformMapper;

    RTPlotDock *m_currentPlotDock;

    ExplorerModes m_modes;
    ExplorerFeatures m_features;

    bool m_debugPrintTime;
    bool m_debugPrintSource;

    QQueue<QkDevice::Event> m_loggerEventQueue;

    BoardPanel *m_commBoardPanel;
    BoardPanel *m_deviceBoardPanel;

    QMainWindow *m_outputWindow;
    QTextEdit *m_outputText;

    QVector<DockableWidget*> m_dockableWidgets;
};

#endif // QKEXPLORERWIDGET_H
