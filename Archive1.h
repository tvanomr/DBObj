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

template<class Obj,class Conn,std::size_t Features>
class ArchiveCoord<Obj,Conn,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
protected:

   typename Connection<Conn,Features>::DBQuery SelectCoordQ,*CurrentQ=nullptr;
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
      SelectCoordQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>()+
                                " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord1>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string(">=?1 and ")+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord1>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string("<=?2")+OrderClause(), "ArchiveCoord::LoadCoord1()");
      TypeManipSQL::BindOargs<0,Conn,Features>(LoadQ,values);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(From,SelectCoordQ);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(To,SelectCoordQ);
      SelectCoordQ.exec();
      CurrentQ=&SelectCoordQ;
   }

   template<class T>
   std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Coord2>::value,void>::type
      LoadCoord2Start(const Type& From,const Type& To,const std::string& Order=std::string(),bool bAsc=true)
   {
      SelectCoordQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>()+
                                " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string(">=?1 and ")+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string("<=?2")+OrderClause(), "ArchiveCoord::LoadCoord1()");
      TypeManipSQL::BindOargs<0,Conn,Features>(LoadQ,values);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(From,SelectCoordQ);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(To,SelectCoordQ);
      SelectCoordQ.exec();
      CurrentQ=&SelectCoordQ;
   }

   template<class T>
   std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Coord3>::value,void>::type
      LoadCoord3Start(const Type& From,const Type& To,const std::string& Order=std::string(),bool bAsc=true)
   {
      SelectCoordQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>()+
                                " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string(">=?1 and ")+
                                std::get<TypeManip::GetPropIndex<Obj,ValueType::Coord2>::value>(ObjInfo<Obj>::info).ColumnName+
                                std::string("<=?2")+OrderClause(), "ArchiveCoord::LoadCoord1()");
      TypeManipSQL::BindOargs<0,Conn,Features>(LoadQ,values);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(From,SelectCoordQ);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(To,SelectCoordQ);
      SelectCoordQ.exec();
      CurrentQ=&SelectCoordQ;
   }

};

template<class Obj,class Conn,std::size_t Features>
class Archive1<Obj,Conn,Features,
      typename std::enable_if<(TypeManip::HavePropIndex<Obj,ValueType::Key>::value &&
                               HaveFeature(Features,DBObj::Features::SQL)),void>::type>
{
protected:
   typedef typename TypeManip::GetValuesIndices<Obj,Features>::type ValuesIndices;
   typename TypeManip::GetIntValuesT<Obj,Features>::type values;
   typename Connection<Conn,Features>::DBQuery LoadQ,InsertQ,DeleteQ;
   Connection<Conn,Features>* pConn;
   static constexpr std::size_t index=TypeManip::GetPropIndex<Obj,ValueType::Key>::value;
public:
   void InitQueries(Connection<Conn,Features>* pConnection)
   {
      pConn=pConnection;
      LoadQ=pConn->Query("select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>()+
                         " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                         std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string(">=?1 and ")+
                         std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string("<=?2"),
                         "Archive1::Load()");
      TypeManipSQL::BindOargs<0,Conn,Features>(LoadQ,values);
      InsertQ=pConn->Query("insert into "+std::string(ObjInfo<Obj>::TableName)+"("+
                           TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>()+
                           ") values ("+TypeManip::CreatePlaceholderList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>(1)+")",
                           "Archive1::Insert()");
      DeleteQ=pConn->Query("delete from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                           std::get<index>(ObjInfo<Obj>::info).ColumnName+">=?1 and "+
                           std::get<index>(ObjInfo<Obj>::info).ColumnName+"<=?2",
                           "Archive1::Delete()");
   }

   template<class Type>
   void LoadStart(const Type& From,const Type& To)
   {
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(From,LoadQ);
      TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(To,LoadQ);
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
      TypeManipSQL::ArgAll<ValuesIndices,Conn,Features>(&obj,InsertQ);
      InsertQ.exec();
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,Features,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)});
   }

   void DeleteAll()
   {
      pConn->DirectExec("delete from "+std::string(ObjInfo<Obj>::TableName));
   }
};

template<class Obj,class Conn,std::size_t Features>
class Archive1ID<Obj,Conn,Features,
      typename std::enable_if<(TypeManip::HavePropIndex<Obj,ValueType::Parent>::value &&
                               TypeManip::HavePropIndex<Obj,ValueType::Key>::value &&
                               HaveFeature(Features,DBObj::Features::SQL)),void>::type>
{
protected:
   typedef typename TypeManip::GetValuesIndices<Obj,Features>::type ValuesIndices;
   static constexpr std::size_t IDIndex=TypeManip::GetPropIndex<Obj,ValueType::Parent>::value;
   static constexpr std::size_t KeyIndex=TypeManip::GetPropIndex<Obj,ValueType::Key>::value;
   typename TypeManip::GetIntValuesT<Obj,Features>::type values;
   typename Connection<Conn,Features>::DBQuery LoadQ,InsertQ,DeleteQ,DeleteByIDQ;
   Connection<Conn,Features>* pConn;
public:
   void InitQueries(Connection<Conn,Features>* pConnection)
   {
      pConn=pConnection;
      LoadQ=pConn->Query("select "+TypeManip::CreateColumnList<ValuesIndices,Obj,Features>()+
                         " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                         std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?1 and ")+
                         std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string(">=?2 and ")+
                         std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string("<=?3"),
                         "Archive1ID::Load()");
      TypeManipSQL::BindOargs<0,Conn,Features>(LoadQ,values);
      InsertQ=pConn->Query("insert into "+std::string(ObjInfo<Obj>::TableName)+"("+
                           TypeManip::CreateColumnList<ValuesIndices,Obj,Features>()+") values ("+
                           TypeManip::CreatePlaceholderList<ValuesIndices,Obj,Features>(1)+")",
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
      TypeManip::TypeInfo<KeyType,Features>::template Arg<Conn>(From,LoadQ);
      TypeManip::TypeInfo<KeyType,Features>::template Arg<Conn>(To,LoadQ);
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
      TypeManip::TypeInfo<KeyType,Features>::template Arg<Conn>(From,LoadQ);
      TypeManip::TypeInfo<KeyType,Features>::template Arg<Conn>(To,LoadQ);
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
      TypeManip::TypeInfo<KeyType,Features>::template Arg<Conn>(From,DeleteQ);
      TypeManip::TypeInfo<KeyType,Features>::template Arg<Conn>(To,DeleteQ);
      DeleteQ.exec();
   }

   void DeleteByParent(typename TypeManip::IthPropType<Obj,IDIndex> pParent)
   {
      DeleteByIDQ.arg(pParent->GetID());
      DeleteByIDQ.exec();
   }

   void Insert(Obj& obj)
   {
      TypeManipSQL::ArgAll<ValuesIndices,Conn,Features>(&obj,InsertQ);
      InsertQ.exec();
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,Features,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{std::string(std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName),
                        std::string(std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName)});
   }
};

}

#endif
