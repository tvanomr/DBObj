#ifndef __DBOBJ_ARCHIVE_1_H__
#define __DBOBJ_ARCHIVE_1_H__


#include "DBObj/Connection.h"
#include "DBObj/ObjInfoUtil.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class ArchiveCoord{};

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class Archive1{};

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class Archive1ID{};

template<class Obj,class Conn>
class ArchiveCoord<Obj,Conn,0>
{
protected:


   typename Connection<Conn,0>::DBQuery SelectCoordQ,*CurrentQ=nullptr;
   virtual std::string OrderClause()=0;
public:
   template<std::size_t ind,std::size_t... inds,class Type,class... Types>
   void LoadCoordStart(const Type& From,const Type& To, const Types&... v)
   {

   }

   template<std::size_t... inds,class... Types>
   std::enable_if<TypeManip::HavePropIndices<Obj,ValueType::Coord1,ValueType::CoordRange>::value,void>::type
      LoadCoordStart(const Type& From,const Type& To,const std::string& Order=std::string(),bool bAsc=true)
   {
      SelectCoordQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
                                " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord1>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string(">=?1 and ")+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord1>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string("<=?2")+OrderClause(), "ArchiveCoord::LoadCoord1()");
      TypeManipSQL::BindOargs<0,Conn,0>(LoadQ,values);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(From,SelectCoordQ);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(To,SelectCoordQ);
      SelectCoordQ.exec();
      CurrentQ=&SelectCoordQ;
   }

   template<class T>
   std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Coord2>::value,void>::type
      LoadCoord2Start(const Type& From,const Type& To,const std::string& Order=std::string(),bool bAsc=true)
   {
      SelectCoordQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
                                " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string(">=?1 and ")+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string("<=?2")+OrderClause(), "ArchiveCoord::LoadCoord1()");
      TypeManipSQL::BindOargs<0,Conn,0>(LoadQ,values);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(From,SelectCoordQ);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(To,SelectCoordQ);
      SelectCoordQ.exec();
      CurrentQ=&SelectCoordQ;
   }

   template<class T>
   std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Coord3>::value,void>::type
      LoadCoord3Start(const Type& From,const Type& To,const std::string& Order=std::string(),bool bAsc=true)
   {
      SelectCoordQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
                                " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string(">=?1 and ")+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string("<=?2")+OrderClause(), "ArchiveCoord::LoadCoord1()");
      TypeManipSQL::BindOargs<0,Conn,0>(LoadQ,values);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(From,SelectCoordQ);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(To,SelectCoordQ);
      SelectCoordQ.exec();
      CurrentQ=&SelectCoordQ;
   }

};

template<class Obj,class Conn>
class Archive1<Obj,Conn,0,typename std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Key>::value,void>::type>
{
protected:
   typedef typename TypeManip::GetValuesIndices<Obj,0>::type ValuesIndices;
   typename TypeManip::GetIntValuesT<Obj,0>::type values;
   typename Connection<Conn,0>::DBQuery LoadQ,InsertQ,DeleteQ;
   Connection<Conn,0>* pConn;
   static constexpr std::size_t index=TypeManip::GetPropIndex<Obj,ValueType::Key>::value;
public:
   void InitQueries(Connection<Conn,0>* pConnection)
   {
      pConn=pConnection;
      LoadQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
                         " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                         std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string(">=?1 and ")+
                         std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string("<=?2"),
                         "Archive1::Load()");
      TypeManipSQL::BindOargs<0,Conn,0>(LoadQ,values);
      InsertQ=pConn->Query("insert into "+std::string(ObjInfo<Obj>::TableName)+"("+
                           TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
                           ") values ("+TypeManip::CreatePlaceholderList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>(1)+")",
                           "Archive1::Insert()");
      DeleteQ=pConn->Query("delete from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                           std::get<index>(ObjInfo<Obj>::info).ColumnName+">=?1 and "+
                           std::get<index>(ObjInfo<Obj>::info).ColumnName+"<=?2",
                           "Archive1::Delete()");
   }

   template<class Type>
   void LoadStart(const Type& From,const Type& To)
   {
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(From,LoadQ);
      TypeManip::TypeInfo<Type,0>::template Arg<Conn>(To,LoadQ);
      LoadQ.exec();
   }

   bool LoadNext(Obj& obj)
   {
      if(LoadQ.next())
      {
         TypeManipSQL::MoveValues<ValuesIndices,0>(&obj,values,pConn);
         return true;
      }
      return false;
   }

   template<class Type>
   void Load(std::vector<Obj>& data,const Type& From,const Type& To)
   {
      data.clear();
      LoadStart(From,To);
      while(LoadQ.next())
      {
         data.emplace_back();
         TypeManipSQL::MoveValues<ValuesIndices,0>(&data.back(),values,pConn);
      }
   }

   template<class Type>
   void Delete(const Type& From,const Type& To)
   {
      DeleteQ.arg(From,To);
      DeleteQ.exec();
   }

   void Insert(Obj& obj)
   {
      TypeManipSQL::ArgAll<ValuesIndices,Conn,0>(&obj,InsertQ);
      InsertQ.exec();
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,0,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)});
   }

   void DeleteAll()
   {
      pConn->DirectExec("delete from "+std::string(ObjInfo<Obj>::TableName));
   }
};

template<class Obj,class Conn>
class Archive1ID<Obj,Conn,0,typename std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Parent>::value && TypeManip::HavePropIndex<Obj,ValueType::Key>::value,void>::type>
{
protected:
   typedef typename TypeManip::GetValuesIndices<Obj,0>::type ValuesIndices;
   static constexpr std::size_t IDIndex=TypeManip::GetPropIndex<Obj,ValueType::Parent>::value;
   static constexpr std::size_t KeyIndex=TypeManip::GetPropIndex<Obj,ValueType::Key>::value;
   typename TypeManip::GetIntValuesT<Obj,0>::type values;
   typename Connection<Conn,0>::DBQuery LoadQ,InsertQ,DeleteQ,DeleteByIDQ;
   Connection<Conn,0>* pConn;
public:
   void InitQueries(Connection<Conn,0>* pConnection)
   {
      pConn=pConnection;
      LoadQ=pConn->Query("select "+TypeManip::CreateColumnList<ValuesIndices,Obj,0>()+
                         " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                         std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?1 and ")+
                         std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string(">=?2 and ")+
                         std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string("<=?3"),
                         "Archive1ID::Load()");
      TypeManipSQL::BindOargs<0,Conn,0>(LoadQ,values);
      InsertQ=pConn->Query("insert into "+std::string(ObjInfo<Obj>::TableName)+"("+
                           TypeManip::CreateColumnList<ValuesIndices,Obj,0>()+") values ("+
                           TypeManip::CreatePlaceholderList<ValuesIndices,Obj,0>(1)+")",
                           "Archive1ID::Insert()");
      DeleteQ=pConn->Query("delete from "+std::string(ObjInfo<Obj>::TableName)+
                           " where "+std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?1 and ")+
                           std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string(">=?2 and ")+
                           std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string("<=?3"),
                           "Archive1ID::Delete()");
      DeleteByIDQ=pConn->Query("delete from "+std::string(ObjInfo<Obj>::TableName)+
                               " where "+std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?1"),
                               "Archive1ID::DeleteByParent()");
   }

   template<class KeyType>
   void LoadStart(typename TypeManip::IthPropType<Obj,IDIndex> pParent,const KeyType& From,const KeyType& To)
   {
      LoadQ.arg(pParent->GetID());
      TypeManip::TypeInfo<KeyType,0>::template Arg<Conn>(From,LoadQ);
      TypeManip::TypeInfo<KeyType,0>::template Arg<Conn>(To,LoadQ);
      LoadQ.exec();
   }

   bool LoadNext(Obj& obj)
   {
      if(LoadQ.next())
      {
         TypeManipSQL::MoveValues<ValuesIndices,0>(&obj,values,pConn);
         return true;
      }
      return false;
   }

   template<class KeyType>
   void Load(std::vector<Obj>& data,typename TypeManip::IthPropType<Obj,IDIndex> pParent,const KeyType& From,const KeyType& To)
   {
      data.clear();
      LoadQ.arg(pParent->GetID());
      TypeManip::TypeInfo<KeyType,0>::template Arg<Conn>(From,LoadQ);
      TypeManip::TypeInfo<KeyType,0>::template Arg<Conn>(To,LoadQ);
      LoadQ.exec();
      while(LoadQ.next())
      {
         data.emplace_back();
         TypeManipSQL::MoveValues<ValuesIndices,0>(&data.back(),values,pConn);
      }
   }

   template<class KeyType>
   void Delete(typename TypeManip::IthPropType<Obj,IDIndex> pParent,const KeyType& From,const KeyType& To)
   {
      DeleteQ.arg(pParent->GetID());
      TypeManip::TypeInfo<KeyType,0>::template Arg<Conn>(From,DeleteQ);
      TypeManip::TypeInfo<KeyType,0>::template Arg<Conn>(To,DeleteQ);
      DeleteQ.exec();
   }

   void DeleteByParent(typename TypeManip::IthPropType<Obj,IDIndex> pParent)
   {
      DeleteByIDQ.arg(pParent->GetID());
      DeleteByIDQ.exec();
   }

   void Insert(Obj& obj)
   {
      TypeManipSQL::ArgAll<ValuesIndices,Conn,0>(&obj,InsertQ);
      InsertQ.exec();
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,0,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{std::string(std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName),
                        std::string(std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName)});
   }
};

}

#endif
