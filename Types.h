#ifndef __DBOBJ_TYPE_HELPERS_H__
#define __DBOBJ_TYPE_HELPERS_H__

#include <cstdint>
#include <tuple>
#include <type_traits>
#include "DBObj/ObjInfo.h"
#include "DBObj/Features.h"


namespace DBObj
{

template<class Conn,std::size_t Features>
class Connection;

namespace TypeManip
{

template<class Type,std::size_t Features,class Condition=void>
struct TypeInfo{};

template<class Type,std::size_t Features>
struct TypeInfo<Type,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>
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
