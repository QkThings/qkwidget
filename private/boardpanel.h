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

namespace Ui {
class BoardPanel;
}

class CProperty;
class QkConnection;

#include "qkcore.h"

class BoardPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit BoardPanel(QWidget *parent = 0);
    ~BoardPanel();
    
    void reset();
    void setBoard(QkBoard *board, QkBoard::Type type, QkConnection *conn);
    void reload();
    void refresh();
    void refreshData();

private slots:
    void _slotActionValueChanged(CProperty *prop);
    void _slotSamplingModeChanged();
    void _slotUpdate();
    void _slotSave();
    void updateInterface();

private:
    enum
    {
        BrowserColumnElement,
        BrowserColumnValue,
        BrowserColumnPopup
    };

    class QkProperties
    {
    public:
        CProperty *top;
        CProperty *version;
        CProperty *baudRate;
    };
    class BoardProperties
    {
    public:
        CProperty *top;
        CProperty *name;
        CProperty *fwVersion;
        CProperty *configs;
        QList<CProperty*> configsList;
    };
    class SamplingProperties
    {
    public:
        CProperty *top;
        CProperty *frequency;
        CProperty *mode;
        CProperty *triggerClock;
        CProperty *triggerScaler;
        CProperty *N;
    };
    class DeviceProperties
    {
    public:
        CProperty *data;
        QList<CProperty*> dataList;
        CProperty *actions;
        QList<CProperty*> actionsList;
        CProperty *events;
        QList<CProperty*> eventsList;
    };

    Ui::BoardPanel *ui;
    QkBoard *m_board;
    QkBoard::Type m_boardType;
    QkConnection *m_conn;


    QkProperties m_qkProp;
    BoardProperties m_boardProp;
    SamplingProperties m_sampProp;
    DeviceProperties m_deviceProp;

    void setupConnections();
};

#endif // BOARDPANEL_H
