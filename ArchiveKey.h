#ifndef __DBOBJ_ARCHIVE_KEY_H__
#define __DBOBJ_ARCHIVE_KEY_H__

#include "DBObj/ArchiveBase.h"
#include "DBObj/SQLUtil.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class ArchiveKeyBase: public virtual ArchiveBase<Obj,Conn,Features>{};

template<class Obj,class Conn>
class ArchiveKeyBase<Obj,Conn,0,
      typename std::enable_if<TypeManip::HavePropIndices<Obj,ValueType::Key,ValueType::KeyRange>::value,void>::type>
      : public virtual ArchiveBase<Obj,Conn,0>
{
protected:
   typedef typename TypeManip::GetPropIndices<Obj,ValueType::Key,ValueType::KeyRange> KeyIndices;
   QueryStorage<Conn,0,GenTempl::Length<typename KeyIndices::values>::value> KeyQueries;
public:
   template<std::size_t... inds,class... Types>
   typename std::enable_if<GenTempl::IsUniqueSeq<inds...>::value &&
   GenTempl::HaveValues<typename KeyIndices::values,inds...>::value &&
   (sizeof...(Types)==2*sizeof...(inds)),void>::type
   LoadKeyStart(const Types&... ranges)
   {
      if(!KeyQueries.template HaveQuery<inds...>())
      {
         KeyQueries.template CreateSelectQuery<Obj,ValueType::Key,inds...>(*this->pConn,this->values,this->GetSelectClause(),TypeManipSQL::GetOrderByClause<Obj>(),"ArchiveKeyBase::LoadKey()");
      }
      this->pCurrentQ=KeyQueries.template ArgAndRun<inds...>(ranges...);
   }

   template<std::size_t... inds,class... Types>
   void LoadKey(std::vector<Obj>& data,const Types&... ranges)
   {
      LoadKeyStart<inds...>(ranges...);
      this->ResultToVector(data);
   }
};

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class ArchiveKey: public ArchiveKeyBase<Obj,Conn,Features>{};

template<class Obj,class Conn>
class ArchiveKey<Obj,Conn,0,
      typename std::enable_if<TypeManip::HavePropIndices<Obj,ValueType::Key,ValueType::KeyRange>::value,void>::type>
      : public ArchiveKeyBase<Obj,Conn,0>
{
protected:
   typename Connection<Conn,0>::DBQuery CheckExistQ;
   typename Connection<Conn,0>::DBQuery UpdateQ;
   std::int64_t count;
   using typename ArchiveKeyBase<Obj,Conn,0>::KeyIndices;
   using typename ArchiveBase<Obj,Conn,0>::ValuesIndices;
   QueryStorage<Conn,0,GenTempl::Length<typename KeyIndices::values>::value> KeyDeleteQueries;
public:
   void InitQueries(Connection<Conn,0>* pConnection)
   {
      ArchiveKeyBase<Obj,Conn,0>::InitQueries(pConnection);
      CheckExistQ=this->pConn->Query(std::string("select count(")+std::get<GenTempl::Get<typename KeyIndices::indices,0>::value>(ObjInfo<Obj>::info).ColumnName+
                                     TypeManipSQL::GetEqualClause<Obj,typename KeyIndices::indices>(),"ArchiveKey::Insert()");
      UpdateQ=this->pConn->Query("update "+std::string(ObjInfo<Obj>::TableName)+" set "+
                                 TypeManip::CreateUpdateList<ValuesIndices,Obj,0>(1)+
                                 TypeManipSQL::GetEqualClause<Obj,typename KeyIndices::indices>(),"ArchiveKey::Insert()");
      CheckExistQ.oarg(count);
   }
   void Insert(Obj& obj)
   {
      TypeManipSQL::ArgAll<KeyIndices,Conn,0>(&obj,CheckExistQ);
      count=0;
      CheckExistQ.exec();
      CheckExistQ.next();
      if(count>0)
      {
         TypeManipSQL::ArgAll<ValuesIndices,Conn,0>(&obj,UpdateQ);
         TypeManipSQL::ArgAll<KeyIndices,Conn,0>(&obj,UpdateQ);
         UpdateQ.exec();
      }
      else
         ArchiveBase<Obj,Conn,0>::Insert(obj);
   }

   template<std::size_t... inds,class... Types>
   typename std::enable_if<GenTempl::IsUniqueSeq<inds...>::value &&
   GenTempl::HaveValues<typename KeyIndices::values,inds...>::value &&
   (sizeof...(Types)==2*sizeof...(inds)),void>::type
   DeleteKey(const Types&... ranges)
   {
      if(!KeyDeleteQueries.template HaveQuery<inds...>())
      {
         KeyDeleteQueries.template CreateDeleteQuery<Obj,ValueType::Key,inds...>(*this->pConn,"delete from "+std::string(ObjInfo<Obj>::TableName),std::string(),"ArchiveKey::DeleteKey()");
      }
      KeyDeleteQueries.template ArgAndRun<inds...>(ranges...);
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,0,ValuesIndices>(cols);
      this->pConn->CheckTable(ObjInfo<Obj>::TableName,cols,TypeManipSQL::GetColumnList<Obj,typename TypeManip::GetPropIndicesByValues<Obj,typename GenTempl::SortUniqueValues<typename KeyIndices::values>::type>::type>());
   }
};


}

#endif
