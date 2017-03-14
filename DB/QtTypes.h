#ifndef DB_QT_TYPES_H_
#define DB_QT_TYPES_H_

#include "DB/ConnectionBase/Types.h"
#include <QString>
#include <QDateTime>

namespace DB
{
template<>
struct GetColType<QString>: public ColTypeIsString{};
template<>
struct GetColType<QDateTime>:public ColTypeIsDatetime{};
}

#endif
