#include "qhexspinbox.h"

QHexSpinBox::QHexSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
    setRange(0, 65535);
    validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,4}"), this);
}

QValidator::State QHexSpinBox::validate(QString &text, int &pos) const
{
    return validator->validate(text, pos);
}

QString QHexSpinBox::textFromValue(int value) const
{
    return QString::number(value, 16).toUpper().rightJustified(4,'0');
}

int QHexSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toInt(&ok, 16);
}

