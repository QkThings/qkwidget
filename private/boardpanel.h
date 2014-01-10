#ifndef BOARDPANEL_H
#define BOARDPANEL_H

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
    
    void setBoard(QkBoard *board, QkBoard::Type type, QkConnection *conn);
    void reload();
    void refresh();
    void refreshData();

private slots:
    void _slotSamplingModeChanged();
    void _slotUpdate();
    void _slotSave();
    void _updateInterface();

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
        //QList<CProperty*> actionsList;
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

    void _setupConnections();
};

#endif // BOARDPANEL_H
