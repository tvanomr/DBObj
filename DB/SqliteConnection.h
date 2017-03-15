#ifndef __DBOBJ_SQLITE_CONNECTION_H__
#define __DBOBJ_SQLITE_CONNECTION_H__

#include "DBObj/DB/ConnectionBase.h"
#include "DBObj/DB/Error.h"
#include <memory>
#include <sqlite3.h>
#include <set>
#include <map>

namespace DB
{

class SqliteConnection : public ConnectionBase
{
public:
   class SqliteError : public DB::Error
   {
      static std::map<int,std::string> Messages;
      int _code;
   public:
      SqliteError(const std::string& FuncName,int SqliteCode,const std::string& Query=std::string())
         : DB::Error(FuncName,"sqlite: "+Messages[SqliteCode]+(Query.size()?(" ("+Query+")"):std::string())),_code(SqliteCode){}
      int code(){return _code;}
   };

   class DBQuery
   {
   public:
      DBQuery();
      DBQuery(DBQuery&& query);
      ~DBQuery();
      void operator = (DBQuery&& query);
      DBQuery& arg(const double& value);
      DBQuery& arg(const float& value);
      DBQuery& arg(const bool& value);
      DBQuery& arg(const std::int8_t& value);
      DBQuery& arg(const std::int16_t& value);
      DBQuery& arg(const int& value);
      DBQuery& arg(const std::int64_t& value);
      DBQuery& arg(const std::uint8_t& value);
      DBQuery& arg(const std::uint16_t& value);
      DBQuery& arg(const std::uint32_t& value);
      DBQuery& arg(const std::uint64_t& value);
      DBQuery& arg(const std::string& value);

      DBQuery& oarg(double& value);
      DBQuery& oarg(float& value);
      DBQuery& oarg(bool& value);
      DBQuery& oarg(std::int8_t& value);
      DBQuery& oarg(std::int16_t& value);
      DBQuery& oarg(int& value);
      DBQuery& oarg(std::int64_t& value);
      DBQuery& oarg(std::uint8_t& value);
      DBQuery& oarg(std::uint16_t& value);
      DBQuery& oarg(std::uint32_t& value);
      DBQuery& oarg(std::uint64_t& value);
      DBQuery& oarg(std::string& value);
      DBQuery& oarg(void);  //skip the column

      template<class T, class T2, class... Ts>
      DBQuery& arg(const T& value,const T2& value2,const Ts&... values);
      template<class T, class T2, class... Ts>
      DBQuery& oarg(T& value,T2& value2,Ts&... values);

      void exec();
      bool next();

      struct InputArgumentBase
      {
         virtual void arg(sqlite3_stmt* pQuery,int index)=0;
      };
      struct OutputArgumentBase
      {
         virtual void get(sqlite3_stmt* pQuery,int index)=0;
      };
   private:
      std::vector<std::unique_ptr<InputArgumentBase>> Input;
      std::vector<std::unique_ptr<OutputArgumentBase>> Output;
      std::string QueryStr;
      sqlite3_stmt* pQuery=nullptr;
      SqliteConnection* pConn=nullptr;
      std::string FuncName;
      bool bPrepared=false;
      bool bExecuted=false;
      void ThrowError(const std::string& Message);
      void RealPrepare();
      void ClearStmt();
      void ResetStmt();
      DBQuery(SqliteConnection* pConnection,const std::string& Query,const std::string& func);
      friend class SqliteConnection;
   };

   SqliteConnection();
   ~SqliteConnection();
   void OpenDB(const std::string& FileName);
   DBQuery Query(const std::string& Request,const std::string& FuncName);
   void DirectExec(const std::string& Request,const std::string& FuncName);
   bool CheckTable(const std::string& TableName,const PLAINCOLUMNS& types,
                   const std::vector<std::string>& PrimaryInds=std::vector<std::string>());
   void Connect();
   void Disconnect();
   bool Connected();
   void SetFileName(const std::string& FileName);
private:
   sqlite3* pDB=nullptr;
   std::set<DBQuery*> Queries;
   std::string _FileName;
   DBQuery TableExistQ;
   int ntables=0;
   DBQuery GetColumnsInfoQ;
   int cid;
   std::string ColName;
   std::string ColType;
   friend class DBQuery;
   static void CheckError(int code,const std::string& FuncName,const std::string& Query=std::string())
   {
      if(code!=SQLITE_OK)
         throw SqliteError(FuncName,code,Query);
   }
};

template<class T,class T2,class... Ts>
SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const T& value,const T2& value2,const Ts&... values)
{
   arg(value);
   return arg(value2,values...);
}

template<class T,class T2,class... Ts>
SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(T& value,T2& value2,Ts&... values)
{
   oarg(value);
   return oarg(value2,values...);
}

}

#endif
