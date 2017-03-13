#ifndef __DBOBJ_TYPE_HELPERS_H__
#define __DBOBJ_TYPE_HELPERS_H__

#include <cstdint>
#include "DBObj/ObjInfo.h"
#include <tuple>

namespace DBObj
{

template<class Conn,std::size_t Features>
class Connection;

namespace TypeManip
{

template<class Type,std::size_t Features,class Condition=void>
struct TypeInfo
{
   typedef std::tuple<Type> IntType;
   template<class Conn,class Values,std::size_t index>
   static void MoveValue(Type& value,Values& values,Connection<Conn,Features>*)
   {
      value=std::move(std::get<index>(values));
   }
   template<class Conn>
   static void Arg(const Type& value,typename Connection<Conn,Features>::DBQuery& query)
   {
      query.arg(value);
   }
};



}
}

#endif
