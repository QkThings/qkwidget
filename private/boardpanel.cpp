#include "boardpanel.h"
#include "ui_boardpanel.h"

#include "qkcore.h"
#include "qkconnect.h"

#include <QHeaderView>
#include <QScrollBar>

BoardPanel::BoardPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoardPanel)
{
    ui->setupUi(this);

    QHeaderView *header;

    header = ui->browser->header();
    header->setSectionResizeMode(BrowserColumnElement, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(BrowserColumnValue, QHeaderView::Stretch);
    header->setSectionResizeMode(BrowserColumnPopup, QHeaderView::Fixed);

    ui->browser->setColumnWidth(BrowserColumnPopup, 16);
    ui->browser->setColumnHidden(BrowserColumnPopup, true);
    ui->browser->setIndentation(10);

    QFont f("Roboto-Regular", 12);
    f.setWeight(QFont::Light);
    ui->label_name->setFont(f);

    QPalette p = ui->label_name->palette();
    p.setColor(QPalette::WindowText, QColor("#111111"));
    ui->label_name->setPalette(p);

    reset();

    setupConnections();
    updateInterface();
}

BoardPanel::~BoardPanel()
{
    delete ui;
}

void BoardPanel::setupConnections()
{
    connect(ui->button_update, SIGNAL(clicked()), this, SLOT(_slotUpdate()));
    connect(ui->button_save, SIGNAL(clicked()), this, SLOT(_slotSave()));
}

void BoardPanel::reset()
{
    m_board = 0;
    m_conn = 0;
}

void BoardPanel::setBoard(QkBoard *board, QkBoard::Type type, QkConnection *conn)
{
    switch(type)
    {
    case QkBoard::btDevice: ui->pixmap->setPixmap(QPixmap(":/img/qkdevice_24.png")); break;
    case QkBoard::btComm: ui->pixmap->setPixmap(QPixmap(":/img/qkcomm_24.png")); break;
    default: ;
    }

    m_board = board;
    m_boardType = type;
    m_conn = conn;

    if(board == 0)
    {
        ui->label_name->setText(tr("n/a"));
        ui->label_firmware->setText(tr("n/a"));
        //ui->label_qkversion->setText(tr("n/a"));
    }
    else
    {
        ui->label_name->setText(board->name());
        ui->label_firmware->setText(QString().sprintf("%04X", board->firmwareVersion()));
        //ui->label_qkversion->setText(board->qkInfo().versionString());

        reload();
        refresh();
    }
    updateInterface();
}

void BoardPanel::reload()
{
    CPropertyBrowser *browser = ui->browser;
    QkDevice *device = 0;

    browser->clear();

    if(m_board == 0) // not available
    {
        return;
    }

    int prevScrollValue = browser->verticalScrollBar()->value();

    m_qkProp.top = new CProperty("Qk", CProperty::Label);
    browser->addProperty(m_qkProp.top);
    m_qkProp.top->item()->setExpanded(true);

    m_qkProp.baudRate = new CProperty("Baud rate (bps)", CProperty::Label, m_qkProp.top);
    browser->addProperty(m_qkProp.baudRate, m_qkProp.top);

    m_boardProp.top = new CProperty("Board", CProperty::Label);
    browser->addProperty(m_boardProp.top);
    m_boardProp.top->item()->setExpanded(true);

    m_boardProp.name = new CProperty("Name", CProperty::Text, m_boardProp.top);
    browser->addProperty(m_boardProp.name, m_boardProp.top);

    m_boardProp.configs = new CProperty("Configuration", CProperty::Label);
    browser->addProperty(m_boardProp.configs);
    m_boardProp.configs->item()->setExpanded(true);

    int config_id = 0;
    m_boardProp.configsList.clear();
    foreach(QkBoard::Config config, m_board->configs())
    {
        CProperty::Type propType;
        switch(config.type())
        {
        case QkBoard::Config::ctIntDec:
            propType = CProperty::Int;
            break;
        case QkBoard::Config::ctIntHex:
            propType = CProperty::Hex;
            break;
        case QkBoard::Config::ctFloat:
            propType = CProperty::Double;
            break;
        case QkBoard::Config::ctBool:
            propType = CProperty::Bool;
            break;
        case QkBoard::Config::ctDateTime:
            propType = CProperty::DateTime;
            break;
        case QkBoard::Config::ctTime:
            propType = CProperty::Time;
            break;
        default:
            propType = CProperty::Label;
        }

        CProperty *configProp = new CProperty(config.label(), propType, m_boardProp.configs);
        configProp->setUserData(QVariant(config_id++));
        configProp->setValue(config.value());
        browser->addProperty(configProp, m_boardProp.configs);
        m_boardProp.configsList.append(configProp);
    }


    if(m_boardType == QkBoard::btDevice)
    {
        device = (QkDevice*) m_board;

        m_sampProp.top = new CProperty("Sampling", CProperty::Label);
        browser->addProperty(m_sampProp.top);
        m_sampProp.top->item()->setExpanded(true);

        m_sampProp.frequency = new CProperty("Frequency (Hz)", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.frequency, m_sampProp.top);

        m_sampProp.mode = new CProperty("Mode", CProperty::Enum, m_sampProp.top);
        QStringList samplingModes;
        samplingModes << "Single" << "Continuous" << "Triggered";
        m_sampProp.mode->setEnumList(samplingModes);
        browser->addProperty(m_sampProp.mode, m_sampProp.top);

        m_sampProp.N = new CProperty("N", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.N, m_sampProp.top);

        m_sampProp.triggerClock = new CProperty("Trigger clock", CProperty::Enum, m_sampProp.top);
        QStringList triggerClocks;
        triggerClocks << "1sec" << "10sec" << "1min" << "10min" << "1hour";

        m_sampProp.triggerClock->setEnumList(triggerClocks);
        browser->addProperty(m_sampProp.triggerClock, m_sampProp.top);

        m_sampProp.triggerScaler = new CProperty("Trigger scaler", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.triggerScaler, m_sampProp.top);

        m_deviceProp.data = new CProperty("Data", CProperty::Label);
        browser->addProperty(m_deviceProp.data);
        m_deviceProp.data->item()->setExpanded(true);

        int data_id = 0;
        m_deviceProp.dataList.clear();
        foreach(QkDevice::Data data, device->data())
        {
            CProperty *dataProp = new CProperty(data.label(), CProperty::Label, m_deviceProp.data);
            dataProp->setUserData(QVariant(data_id++));
            dataProp->setValue(data.value());
            browser->addProperty(dataProp, m_deviceProp.data);
            m_deviceProp.dataList.append(dataProp);
        }

        m_deviceProp.actions = new CProperty("Actions", CProperty::Label);
        browser->addProperty(m_deviceProp.actions);
        m_deviceProp.actions->item()->setExpanded(true);

        int action_id = 0;
        m_deviceProp.actionsList.clear();
        foreach(QkDevice::Action action, device->actions())
        {
            CProperty::Type propType;
            switch(action.type())
            {
            case QkDevice::Action::atBool:
                propType = CProperty::Bool;
                break;
            case QkDevice::Action::atInt:
                propType = CProperty::Int;
                break;
            default:
                propType = CProperty::Label;
            }

            CProperty *actionProp = new CProperty(action.label(), propType, m_deviceProp.actions);
            actionProp->setUserData(QVariant(action_id++));
            actionProp->setValue(action.value());
            browser->addProperty(actionProp, m_deviceProp.actions);
            m_deviceProp.actionsList.append(actionProp);

            connect(actionProp, SIGNAL(valueChanged(CProperty*)), this, SLOT(_slotActionValueChanged(CProperty*)));

        }

        m_deviceProp.events = new CProperty("Events", CProperty::Label);
        browser->addProperty(m_deviceProp.events);
        m_deviceProp.events->item()->setExpanded(true);

        int event_id = 0;
        m_deviceProp.eventsList.clear();
        foreach(QkDevice::Event event, device->events())
        {
            CProperty *eventProp = new CProperty(event.label(), CProperty::Label, m_deviceProp.events);
            eventProp->setUserData(QVariant(event_id++));
            browser->addProperty(eventProp, m_deviceProp.events);
            m_deviceProp.eventsList.append(eventProp);
        }

        connect(m_sampProp.mode, SIGNAL(valueChanged(CProperty*)), this, SLOT(_slotSamplingModeChanged()));
        _slotSamplingModeChanged();
    }

    browser->verticalScrollBar()->setValue(prevScrollValue);
}

void BoardPanel::refresh()
{
    if(m_board == 0)
        return;

    int i;
    CPropertyBrowser *browser = 0;
    QkDevice *device = 0;
    browser = ui->browser;

    m_qkProp.baudRate->setValue(m_board->qkInfo().baudRate);
    m_boardProp.name->setValue(m_board->name());


    QVector<QkBoard::Config> configs = m_board->configs();
    for(i = 0; i < configs.count(); i++)
    {
        m_boardProp.configsList[i]->setValue(configs[i].value());
    }

    if(m_boardType == QkBoard::btDevice)
    {
        device = (QkDevice*) m_board;
        m_sampProp.frequency->setValue(device->samplingInfo().frequency);
        m_sampProp.mode->setValue(device->samplingInfo().mode);
        m_sampProp.triggerClock->setValue(device->samplingInfo().triggerClock);
        m_sampProp.triggerScaler->setValue(device->samplingInfo().triggerScaler);
        m_sampProp.N->setValue(device->samplingInfo().N);

        refreshData();
    }
}

void BoardPanel::refreshData()
{
    if(m_board == 0 || m_boardType != QkBoard::btDevice)
    {
        qWarning() << __FUNCTION__ << "m_board == 0 || m_boardType != QkBoard::btDevice";
        return;
    }

    QkDevice *device = (QkDevice*) m_board;

    int i;
    QString valueStr;
    QVector<QkDevice::Data> data = device->data();

    for(i = 0; i < data.count(); i++)
    {
        if(device->dataType() == QkDevice::Data::dtInt)
            valueStr = QString().sprintf("%-10d", (int)data[i].value());
        else
            valueStr = QString().sprintf("% .6f", data[i].value());
        m_deviceProp.dataList[i]->setValue(valueStr);
    }
}

void BoardPanel::_slotActionValueChanged(CProperty *prop)
{
    if(m_board == 0) return;

    int action_id = prop->userData().toInt();

    QkDevice *device = (QkDevice*) m_board;

    device->actuate(action_id, prop->value());
}

void BoardPanel::_slotSamplingModeChanged()
{
    bool enTriggeredMode, enNumberOfSamples;
    QkDevice::SamplingMode curSampMode = (QkDevice::SamplingMode)m_sampProp.mode->value().toInt();

    if(curSampMode == QkDevice::smTriggered)
        enTriggeredMode = true;
    else
        enTriggeredMode = false;
    if(curSampMode == QkDevice::smContinuous)
        enNumberOfSamples = false;
    else
        enNumberOfSamples = true;

    m_sampProp.N->setEnabled(enNumberOfSamples);
    m_sampProp.triggerClock->setEnabled(enTriggeredMode);
    m_sampProp.triggerScaler->setEnabled(enTriggeredMode);
}

void BoardPanel::_slotUpdate()
{
    int i;

    if(m_boardType == QkBoard::btDevice)
    {
        QkDevice *device = (QkDevice*) m_board;

        device->_setName(m_boardProp.name->value().toString());

        for(i = 0; i < m_boardProp.configsList.count(); i++)
        {
            CProperty *configProp = m_boardProp.configsList.at(i);
            device->setConfigValue(i, configProp->value());
        }

        QkDevice::SamplingInfo sampInfo;
        sampInfo.frequency = m_sampProp.frequency->value().toInt();
        sampInfo.mode = (QkDevice::SamplingMode) m_sampProp.mode->value().toInt();
        sampInfo.triggerClock = (QkDevice::TriggerClock) m_sampProp.triggerClock->value().toInt();
        sampInfo.triggerScaler = m_sampProp.triggerScaler->value().toInt();
        sampInfo.N = m_sampProp.N->value().toInt();
        device->_setSamplingInfo(sampInfo);

        device->update();
        m_conn->qk()->getNode(device->address());
    }
}

void BoardPanel::_slotSave()
{

}

void BoardPanel::updateInterface()
{
    bool buttonsEnabled = (m_board != 0);
    ui->button_update->setEnabled(buttonsEnabled);
    ui->button_save->setEnabled(false);
}
