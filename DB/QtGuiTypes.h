#ifndef DB_QT_GUI_TYPES_H_
#define DB_QT_GUI_TYPES_H_

#include "DBObj/DB/Types.h"
#include <QtGui/QImage>

namespace DB
{
template<>
struct GetColType<QImage>: public ColTypeIsBlob{};
}

#endif
