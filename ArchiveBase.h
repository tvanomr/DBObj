#ifndef __DBOBJ_ARCHIVE_BASE_H__
#define __DBOBJ_ARCHIVE_BASE_H__

#include "DBObj/Features.h"
#include "DBObj/Connection.h"
#include "DBObj/ObjInfoUtil.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class ArchiveBase{};

template<class Obj,class Conn,std::size_t Features>
class ArchiveBase<Obj,Conn,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
protected:
   typedef typename TypeManip::GetValuesIndices<Obj,Features>::type ValuesIndices;
   typename TypeManip::GetIntValuesT<Obj,Features>::type values;
   Connection<Conn,Features>* pConn;
   typename Connection<Conn,Features>::DBQuery* pCurrentQ;
   typename Connection<Conn,Features>::DBQuery InsertQ;
   std::string GetSelectClause()
   {
      return "select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,Features>::type,Obj,Features>()+
            " from "+std::string(ObjInfo<Obj>::TableName);
   }
public:
   void InitQueries(Connection<Conn,Features>* pConnection)
   {
      pConn=pConnection;
      InsertQ=pConn->Query("insert into "+std::string(ObjInfo<Obj>::TableName)+"("+
                           TypeManip::CreateColumnList<ValuesIndices,Obj,Features>()+") values ("+
                           TypeManip::CreatePlaceholderList<ValuesIndices,Obj,Features>(1)+")",
                           "ArchiveBase::Insert()");
   }

   bool LoadNext(Obj& obj)
   {
      if(pCurrentQ && pCurrentQ->next())
      {
         TypeManipSQL::MoveValues<ValuesIndices,0>(&obj,values,pConn);
         return true;
      }
      return false;
   }

   void ResultToVector(std::vector<Obj>& vec)
   {
      vec.clear();
      if(pCurrentQ)
      {
         while(pCurrentQ->next())
         {
            vec.emplace_back();
            TypeManipSQL::MoveValues<ValuesIndices,0>(&vec.back(),values,pConn);
         }
      }
   }

   void Insert(Obj& obj)
   {
      TypeManipSQL::ArgAll<ValuesIndices,Conn,Features>(&obj,InsertQ);
      InsertQ.exec();
   }

   void DeleteAll()
   {
      pConn->DirectExec("delete from "+std::string(ObjInfo<Obj>::TableName));
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,Features,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols);
   }
};

}

#endif
