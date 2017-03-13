#ifndef __DBOBJ_ARCHIVE_2_H__
#define __DBOBJ_ARCHIVE_2_H__

#include "DBObj/Connection.h"
#include "DBObj/ObjInfoUtil.h"
#include "DBObj/Archive1.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class Archive2ID{};

template<class Obj,class Conn>
class Archive2ID<Obj,Conn,0,typename std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Parent>::value && TypeManip::HavePropIndex<Obj,ValueType::Key>::value && TypeManip::HavePropIndex<Obj,ValueType::Key2>::value,void>::type>
      : public Archive1ID<Obj,Conn,0>
{
protected:
   using typename Archive1ID<Obj,Conn,0>::ValuesIndices;
   using Archive1ID<Obj,Conn,0>::IDIndex;
   using Archive1ID<Obj,Conn,0>::KeyIndex;
   using Archive1ID<Obj,Conn,0>::values;
   using Archive1ID<Obj,Conn,0>::pConn;
   static constexpr std::size_t Key2Index=TypeManip::GetPropIndex<Obj,ValueType::Key2>::value;
   typename Connection<Conn,0>::DBQuery Load2KQ,DeleteObjQ;
public:
   using Archive1ID<Obj,Conn,0>::Load;
   using Archive1ID<Obj,Conn,0>::LoadStart;
   void InitQueries(Connection<Conn,0>* pConnection)
   {
      Archive1ID<Obj,Conn,0>::InitQueries(pConnection);
      Load2KQ=pConn->Query("select "+TypeManip::CreateColumnList<ValuesIndices,Obj,0>()+
                           " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                           std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?1 and ")+
                           std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string(">=?2 and ")+
                           std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string("<=?3 and ")+
                           std::get<Key2Index>(ObjInfo<Obj>::info).ColumnName+std::string(">=?4 and ")+
                           std::get<Key2Index>(ObjInfo<Obj>::info).ColumnName+std::string("<=?5"),
                           "Archive2ID::Load2k()");
      TypeManipSQL::BindOargs<0,Conn,0>(Load2KQ,values);
      DeleteObjQ=pConn->Query("delete from "+std::string(ObjInfo<Obj>::TableName)+
                              " where "+std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?1 and ")+
                              std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName+std::string("=?2 and ")+
                              std::get<Key2Index>(ObjInfo<Obj>::info).ColumnName+std::string("=?3"),
                              "Archive2ID::DeleteObj()");
   }

   template<class KeyType1,class KeyType2>
   void LoadStart(typename TypeManip::IthPropType<Obj,IDIndex> pParent,const KeyType1& From,const KeyType1& To,const KeyType2& From2,const KeyType2& To2)
   {
      Load2KQ.arg(pParent->GetID());
      TypeManip::TypeInfo<KeyType1,0>::template Arg<Conn>(From,Load2KQ);
      TypeManip::TypeInfo<KeyType1,0>::template Arg<Conn>(To,Load2KQ);
      TypeManip::TypeInfo<KeyType2,0>::template Arg<Conn>(From2,Load2KQ);
      TypeManip::TypeInfo<KeyType2,0>::template Arg<Conn>(To2,Load2KQ);
      Load2KQ.exec();
   }

   bool Load2Next(Obj& obj)
   {
      if(Load2KQ.next())
      {
         TypeManipSQL::MoveValues<ValuesIndices,0>(&obj,values,pConn);
         return true;
      }
      return false;
   }

   template<class KeyType1,class KeyType2>
   void Load(std::vector<Obj>& data,typename TypeManip::IthPropType<Obj,IDIndex> pParent,const KeyType1& From,const KeyType1& To,const KeyType2& From2,const KeyType2& To2)
   {
      data.clear();
      LoadStart(pParent,From,To,From2,To2);
      while(Load2KQ.next())
      {
         data.emplace_back();
         TypeManipSQL::MoveValues<ValuesIndices,0>(&data.back(),values,pConn);
      }
   }

   void DeleteObj(Obj& obj)
   {
      TypeManip::TypeInfo<TypeManip::IthPropType<Obj,IDIndex>,0>::template Arg<Conn>(std::get<IDIndex>(ObjInfo<Obj>::info).Get(&obj),DeleteObjQ);
      TypeManip::TypeInfo<TypeManip::IthPropType<Obj,KeyIndex>,0>::template Arg<Conn>(std::get<KeyIndex>(ObjInfo<Obj>::info).Get(&obj),DeleteObjQ);
      TypeManip::TypeInfo<TypeManip::IthPropType<Obj,Key2Index>,0>::template Arg<Conn>(std::get<Key2Index>(ObjInfo<Obj>::info).Get(&obj),DeleteObjQ);
      DeleteObjQ.exec();
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,0,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{std::string(std::get<IDIndex>(ObjInfo<Obj>::info).ColumnName),
                                                      std::string(std::get<KeyIndex>(ObjInfo<Obj>::info).ColumnName),
                                                      std::string(std::get<Key2Index>(ObjInfo<Obj>::info).ColumnName)});
   }
};

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class Archive2{};

template<class Obj,class Conn>
class Archive2<Obj,Conn,0,typename std::enable_if<TypeManip::HavePropIndex<Obj,ValueType::Key>::value && TypeManip::HavePropIndex<Obj,ValueType::Key2>::value,void>::type>
      : public Archive1<Obj,Conn,0>
{
protected:
   using typename Archive1<Obj,Conn,0>::ValuesIndices;
   using Archive1<Obj,Conn,0>::index;
   using Archive1<Obj,Conn,0>::values;
   using Archive1<Obj,Conn,0>::pConn;
   static constexpr std::size_t index2=TypeManip::GetPropIndex<Obj,ValueType::Key2>::value;
   typename Connection<Conn,0>::DBQuery Load2KQ,DeleteObjQ;
public:
   using Archive1<Obj,Conn,0>::Load;
   using Archive1<Obj,Conn,0>::LoadStart;
   void InitQueries(Connection<Conn,0>* pConnection)
   {
      Archive1<Obj,Conn,0>::InitQueries(pConnection);
      Load2KQ=pConn->Query("select "+TypeManip::CreateColumnList<ValuesIndices,Obj,0>()+
                           " from "+std::string(ObjInfo<Obj>::TableName)+" where "+
                           std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string(">=?1 and ")+
                           std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string("<=?2 and ")+
                           std::get<index2>(ObjInfo<Obj>::info).ColumnName+std::string(">=?3 and ")+
                           std::get<index2>(ObjInfo<Obj>::info).ColumnName+std::string("<=?4"),
                           "Archive2::Load2k()");
      TypeManipSQL::BindOargs<0,Conn,0>(Load2KQ,values);
      DeleteObjQ=pConn->Query("delete from "+std::string(ObjInfo<Obj>::TableName)+
                              " where "+std::get<index>(ObjInfo<Obj>::info).ColumnName+std::string("=?1 and ")+
                              std::get<index2>(ObjInfo<Obj>::info).ColumnName+std::string("=?2"),
                              "Archive2::DeleteObj()");
   }

   template<class KeyType1,class KeyType2>
   void LoadStart(const KeyType1& From,const KeyType1& To,const KeyType1& From2,const KeyType2& To2)
   {
      TypeManip::TypeInfo<KeyType1,0>::template Arg<Conn>(From,Load2KQ);
      TypeManip::TypeInfo<KeyType1,0>::template Arg<Conn>(To,Load2KQ);
      TypeManip::TypeInfo<KeyType2,0>::template Arg<Conn>(From2,Load2KQ);
      TypeManip::TypeInfo<KeyType2,0>::template Arg<Conn>(To2,Load2KQ);
      Load2KQ.exec();
   }

   bool Load2Next(Obj& obj)
   {
      if(Load2KQ.next())
      {
         TypeManipSQL::MoveValues<ValuesIndices,0>(&obj,values,pConn);
         return true;
      }
      return false;
   }

   template<class KeyType1,class KeyType2>
   void Load(std::vector<Obj>& data,const KeyType1& From,const KeyType1& To,const KeyType2& From2,const KeyType2& To2)
   {
      data.clear();
      LoadStart(From,To,From2,To2);
      while(Load2KQ.next())
      {
         data.emplace_back();
         TypeManipSQL::MoveValues<ValuesIndices,0>(&data.back(),values,pConn);
      }
   }

   void DeleteObj(Obj& obj)
   {
      TypeManip::TypeInfo<TypeManip::IthPropType<Obj,index>,0>::template Arg<Conn>(std::get<index>(ObjInfo<Obj>::info).Get(&obj),DeleteObjQ);
      TypeManip::TypeInfo<TypeManip::IthPropType<Obj,index2>,0>::template Arg<Conn>(std::get<index2>(ObjInfo<Obj>::info).Get(&obj),DeleteObjQ);
      DeleteObjQ.exec();
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,0,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName),
                                                      std::string(std::get<index2>(ObjInfo<Obj>::info).ColumnName)});
   }
};

}

#endif
