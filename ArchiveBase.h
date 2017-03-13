#ifndef __DBOBJ_ARCHIVE_BASE_H__
#define __DBOBJ_ARCHIVE_BASE_H__

#include "DBObj/Connection.h"
#include "DBObj/ObjInfoUtil.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features>
class ArchiveBase{};

template<class Obj,class Conn>
class ArchiveBase<Obj,Conn,0>
{
protected:
   typedef typename TypeManip::GetValuesIndices<Obj,0>::type ValuesIndices;
   typename TypeManip::GetIntValuesT<Obj,0>::type values;
   Connection<Conn,0>* pConn;
   typename Connection<Conn,0>::DBQuery* pCurrentQ;
   typename Connection<Conn,0>::DBQuery InsertQ;
   std::string GetSelectClause()
   {
      return "select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
            " from "+std::string(ObjInfo<Obj>::TableName);
   }
public:
   void InitQueries(Connection<Conn,0>* pConnection)
   {
      pConn=pConnection;
      InsertQ=pConn->Query("insert into "+std::string(ObjInfo<Obj>::TableName)+"("+
                           TypeManip::CreateColumnList<ValuesIndices,Obj,0>()+") values ("+
                           TypeManip::CreatePlaceholderList<ValuesIndices,Obj,0>(1)+")",
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
      TypeManipSQL::ArgAll<ValuesIndices,Conn,0>(&obj,InsertQ);
      InsertQ.exec();
   }

   void DeleteAll()
   {
      pConn->DirectExec("delete from "+std::string(ObjInfo<Obj>::TableName));
   }

   void CheckTable()
   {
      DB::ConnectionBase::PLAINCOLUMNS cols;
      TypeManipSQL::GetColumnInfo<Obj,0,ValuesIndices>(cols);
      pConn->CheckTable(ObjInfo<Obj>::TableName,cols);
   }
};

}

#endif
