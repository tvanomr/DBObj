#ifndef __DBOBJ_STD_TYPES_H__
#define __DBOBJ_STD_TYPES_H__

#include "DBObj/Types.h"
#include "DBObj/Connection.h"
#include <set>
#include <type_traits>

namespace DBObj
{

namespace TypeManip
{

template<class Type,std::size_t Features>
struct TypeInfo<std::set<Type>,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
   template<class Conn,class Parent>
   struct Special
   {
      typename Connection<Conn,Features>::DBQuery LoadQ;
      Type value;
      template<std::size_t ind>
      void Init(Connection<Conn,Features>* pConn)
      {
         LoadQ=pConn->Query(std::string("select f_value from ")+ObjInfo<Parent>::TableName+
                            std::string("_")+std::get<ind>(ObjInfo<Parent>::info).ColumnName+
                            std::string(" where f_guid=?1"),"TypeInfo<std::set>::Special::InitProp()");
         LoadQ.oarg(value);
      }
      void InitProp(std::size_t id,std::set<Type>& prop,Connection<Conn,Features>*)
      {
         LoadQ.arg(id);
         LoadQ.exec();
         prop.clear();
         while(LoadQ.next())
            prop.insert(value);
      }
   };
   template<class Conn,class Parent>
   struct SpecialEditor
   {
      typename Connection<Conn,Features>::DBQuery InsertQ;
      typename Connection<Conn,Features>::DBQuery DeleteQ;
      template<std::size_t ind>
      void Init(Connection<Conn,Features>* pConn)
      {
         InsertQ=pConn->Query(std::string("insert into ")+ObjInfo<Parent>::TableName+
                              std::string("_")+std::get<ind>(ObjInfo<Parent>::info).ColumnName+
                              std::string(" (f_guid,f_value) values (?1,?2)"),"TypeInfo<std::set>::SpecialEditor::SaveProp()");
         DeleteQ=pConn->Query(std::string("delete from ")+ObjInfo<Parent>::TableName+
                              std::string("_")+std::get<ind>(ObjInfo<Parent>::info).ColumnName+
                              std::string(" where f_guid=?1"),
                              "TypeInfo<std::set>::SpecialEditor::SaveProp()");
      }

      void Save(std::size_t id,const std::set<Type>& values,Connection<Conn,Features>*)
      {
         DeleteQ.arg(id);
         DeleteQ.exec();
         for(auto& value: values)
         {
            InsertQ.arg(id,value);
            InsertQ.exec();
         }
      }

      void Delete(std::size_t id)
      {
         DeleteQ.arg(id);
         DeleteQ.exec();
      }

      template<std::size_t ind>
      void CheckTable(Connection<Conn,Features>* pConn)
      {
         pConn->CheckTable(ObjInfo<Parent>::TableName+std::string("_")+std::get<ind>(ObjInfo<Parent>::info).ColumnName,
         {{"f_guid",DB::TypeInteger},{"f_value",DB::GetColType<Type>::value}},{"f_guid","f_value"});
      }
   };
};

}

}

#endif
