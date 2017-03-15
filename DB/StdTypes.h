#ifndef DB_STD_TYPES_H_
#define DB_STD_TYPES_H_

#include "DBObj/DB/Types.h"
#include <string>
#include <chrono>

namespace DB
{
template<>
struct GetColType<std::string>:public ColTypeIsString{};
template<class Clock,class Duration>
struct GetColType<std::chrono::time_point<Clock,Duration>>:public ColTypeIsDatetime{};
}

#endif
