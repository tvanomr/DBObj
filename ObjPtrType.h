#ifndef __DBOBJ_OBJ_PTR_TYPE_H__
#define __DBOBJ_OBJ_PTR_TYPE_H__

#include "DBObj/Object.h"
#include "DBObj/Types.h"
#include "DBObj/Connection.h"

namespace DBObj
{
namespace TypeManip
{

template<class Obj>
struct TypeInfo<ObjPtr<Obj>,0,void>
{
   typedef std::tuple<std::size_t> IntType;
   template<class Conn,class Values,std::size_t index>
   static void MoveValue(ObjPtr<Obj>& value,Values& values,Connection<Conn,0>* pConn)
   {
      value=pConn->template GetObjPtr<Obj>(std::get<index>(values));
   }
   template<class Conn>
   static void Arg(const ObjPtr<Obj>& value,typename Connection<Conn,0>::DBQuery& query)
   {
      query.arg(value.GetID());
   }
};

template<class Obj>
struct TypeInfo<Obj*,0,typename std::enable_if<std::is_base_of<Object,Obj>::value,void>::type>
{
   typedef std::tuple<std::size_t> IntType;
   template<class Conn,class Values,std::size_t index>
   static void MoveValue(Obj*& value,Values& values,Connection<Conn,0>* pConn)
   {
      value=pConn->template GetObject<Obj>(std::get<index>(values));
   }
   template<class Conn>
   static void Arg(Obj* value,typename Connection<Conn,0>::DBQuery& query)
   {
      query.arg(value->GetID());
   }
};


}
}

#endif
