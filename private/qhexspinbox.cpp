#include "qhexspinbox.h"

#include <QDebug>

QHexSpinBox::QHexSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
    setRange(0, 0xFFFFFFFF);
    validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,8}"), this);
}

QValidator::State QHexSpinBox::validate(QString &text, int &pos) const
{
    return validator->validate(text, pos);
}

QString QHexSpinBox::textFromValue(int value) const
{
    unsigned int uval = (unsigned int) value;
    qDebug() << __FUNCTION__ << (unsigned int)uval << this;
    QString hexStr;
    //hexStr.append(QString::number(value, 16).rightJustified(8, '0', true));
    hexStr.append(QString::number((uval >> 16) & 0xFFFF, 16).rightJustified(4, '0'));
    //hexStr.append(" ");
    hexStr.append(QString::number((uval) & 0xFFFF, 16).rightJustified(4, '0'));
    return hexStr.toUpper();
    //return QString::number(value, 16).toUpper().rightJustified(4,'0');
}

int QHexSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toUInt(&ok, 16);
}

