#include "util.h"
QVariant convert(const QString &value, const QVariant::Type &type)
{
    switch (type)
    {
        case QVariant::Double:
            return value.toDouble();
        case QVariant::Int:
            return value.toInt();
        default:
            return value;
    }
}
