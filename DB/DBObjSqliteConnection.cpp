#include "DBObj/DB/SqliteConnection.h"
#include "DBObj/DB/Error.h"
#include <limits>

namespace
{

template<class T>
struct InputDouble : public DB::SqliteConnection::DBQuery::InputArgumentBase
{
   double data;
   InputDouble(T value):data(value){}
   void arg(sqlite3_stmt* pQuery,int index) override
   {
      sqlite3_bind_double(pQuery,index,data);
   }
};

template<class T>
struct InputInt : public DB::SqliteConnection::DBQuery::InputArgumentBase
{
   int data;
   InputInt(T value):data(value){}
   void arg(sqlite3_stmt* pQuery,int index) override
   {
      sqlite3_bind_int(pQuery,index,data);
   }
};

template<class T>
struct InputUint : public DB::SqliteConnection::DBQuery::InputArgumentBase
{
   int data;
   InputUint(T value):data((value>std::numeric_limits<int>::max())?std::numeric_limits<int>::max():value){}
   void arg(sqlite3_stmt *pQuery, int index) override
   {
      sqlite3_bind_int(pQuery,index,data);
   }
};

struct InputInt64 : public DB::SqliteConnection::DBQuery::InputArgumentBase
{
   std::int64_t data;
   InputInt64(std::int64_t value): data(value){}
   void arg(sqlite3_stmt *pQuery, int index) override
   {
      sqlite3_bind_int64(pQuery,index,data);
   }
};

struct InputUInt64 : public DB::SqliteConnection::DBQuery::InputArgumentBase
{
   std::int64_t data;
   InputUInt64(std::uint64_t value):
      data((value>std::numeric_limits<std::int64_t>::max())?std::numeric_limits<std::int64_t>::max():value){}
   void arg(sqlite3_stmt *pQuery, int index) override
   {
      sqlite3_bind_int64(pQuery,index,data);
   }
};

struct InputString : public DB::SqliteConnection::DBQuery::InputArgumentBase
{
   std::string data;
   InputString(const std::string& value){data=value;}
   void arg(sqlite3_stmt *pQuery, int index) override
   {
      sqlite3_bind_text64(pQuery,index,data.c_str(),data.length(),SQLITE_STATIC,SQLITE_UTF8);

//      std::cout<<"Bound string \""<<data<<"\" to index "<<index<<std::endl;
   }
};

template<class T>
struct OutputDouble : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   T* pData;
   OutputDouble(T& value):pData(&value){}
   void get(sqlite3_stmt *pQuery, int index) override
   {
      *pData=sqlite3_column_double(pQuery,index);
   }
};

template<class T>
struct OutputInt : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   T* pData;
   OutputInt(T& value):pData(&value){}
   void get(sqlite3_stmt *pQuery, int index) override
   {
      *pData=sqlite3_column_int(pQuery,index);

//      std::cout<<"Read integer: "<<sqlite3_column_int(pQuery,index)<<std::endl;
   }
};

template<class T>
struct OutputUInt : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   T* pData;
   OutputUInt(T& value): pData(&value){}
   void get(sqlite3_stmt *pQuery, int index) override
   {
      int v=sqlite3_column_int(pQuery,index);
      *pData=(v>0)?v:(-v);
   }
};

struct OutputInt64 : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   std::int64_t* pData;
   OutputInt64(std::int64_t& value): pData(&value){}
   void get(sqlite3_stmt *pQuery, int index) override
   {
      *pData=sqlite3_column_int64(pQuery,index);
   }
};

struct OutputUInt64 : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   std::uint64_t* pData;
   OutputUInt64(std::uint64_t& value): pData(&value){}
   void get(sqlite3_stmt *pQuery, int index) override
   {
      std::int64_t v=sqlite3_column_int64(pQuery,index);
      *pData=(v>0)?v:(-v);
   }
};

struct OutputString : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   std::string* pData;
   OutputString(std::string& value):pData(&value){}
   void get(sqlite3_stmt *pQuery, int index) override
   {
      *pData=reinterpret_cast<const char*>(sqlite3_column_text(pQuery,index));
   }
};

struct OutputNone : public DB::SqliteConnection::DBQuery::OutputArgumentBase
{
   void get(sqlite3_stmt *, int) override{}
};

}

namespace DB
{

std::map<int,std::string> SqliteConnection::SqliteError::Messages{
   std::make_pair(SQLITE_OK,std::string("Successful result")),
         std::make_pair(SQLITE_ERROR,std::string("SQL error or missing database")),
         std::make_pair(SQLITE_INTERNAL,std::string("Internal logic error in SQLite")),
         std::make_pair(SQLITE_PERM,std::string("Access permission denied")),
         std::make_pair(SQLITE_ABORT,std::string("Callback routine requested an abort")),
         std::make_pair(SQLITE_BUSY,std::string("The database file is locked")),
         std::make_pair(SQLITE_LOCKED,std::string("A table in the database is locked")),
         std::make_pair(SQLITE_NOMEM,std::string("A malloc() failed")),
         std::make_pair(SQLITE_READONLY,std::string("Attempt to write a readonly database")),
         std::make_pair(SQLITE_INTERRUPT,std::string("Operation terminated by sqlite3_interrupt()")),
         std::make_pair(SQLITE_IOERR,std::string("Some kind of disk I/O error occurred")),
         std::make_pair(SQLITE_CORRUPT,std::string("The database disk image is malformed")),
         std::make_pair(SQLITE_NOTFOUND,std::string("Unknown opcode in sqlite3_file_control()")),
         std::make_pair(SQLITE_FULL,std::string("Insertion failed because database is full")),
         std::make_pair(SQLITE_CANTOPEN,std::string("Unable to open the database file")),
         std::make_pair(SQLITE_PROTOCOL,std::string("Database lock protocol error")),
         std::make_pair(SQLITE_EMPTY,std::string("Database is empty")),
         std::make_pair(SQLITE_SCHEMA,std::string("The database schema changed")),
         std::make_pair(SQLITE_TOOBIG,std::string("String or BLOB exceeds size limit")),
         std::make_pair(SQLITE_CONSTRAINT,std::string("Abort due to constraint violation")),
         std::make_pair(SQLITE_MISMATCH,std::string("Data type mismatch")),
         std::make_pair(SQLITE_MISUSE,std::string("Library used incorrectly")),
         std::make_pair(SQLITE_NOLFS,std::string("Uses OS features not supported on host")),
         std::make_pair(SQLITE_AUTH,std::string("Authorization denied")),
         std::make_pair(SQLITE_FORMAT,std::string("Auxiliary database format error")),
         std::make_pair(SQLITE_RANGE,std::string("2nd parameter to sqlite3_bind out of range")),
         std::make_pair(SQLITE_NOTADB,std::string("File opened that is not a database file")),
         std::make_pair(SQLITE_NOTICE,std::string("Notifications from sqlite3_log()")),
         std::make_pair(SQLITE_WARNING,std::string("Warnings from sqlite3_log()")),
         std::make_pair(SQLITE_ROW,std::string("sqlite3_step() has another row ready")),
         std::make_pair(SQLITE_DONE,std::string("sqlite3_step() has finished executing")),
         std::make_pair(SQLITE_IOERR_READ,std::string("I/O error in the VFS layer while trying to read from a file on disk")),
         std::make_pair(SQLITE_IOERR_SHORT_READ,std::string("A read attempt in the VFS layer was unable to obtain as many bytes as was requested")),
         std::make_pair(SQLITE_IOERR_WRITE,std::string("I/O error in the VFS layer while trying to write into a file on disk")),
         std::make_pair(SQLITE_IOERR_FSYNC,std::string("I/O error in the VFS layer while trying to flush previously written content out of OS and/or disk-control buffers and into persistent storage")),
         std::make_pair(SQLITE_IOERR_DIR_FSYNC,std::string("I/O error in the VFS layer while trying to invoke fsync() on a directory")),
         std::make_pair(SQLITE_IOERR_TRUNCATE,std::string("I/O error in the VFS layer while trying to truncate a file to a smaller size")),
         std::make_pair(SQLITE_IOERR_FSTAT,std::string("I/O error in the VFS layer while trying to invoke fstat() (or the equivalent) on a file in order to determine information such as the file size or access permissions")),
         std::make_pair(SQLITE_IOERR_UNLOCK,std::string("I/O error within xUnlock method on the sqlite3_io_methods object")),
         std::make_pair(SQLITE_IOERR_RDLOCK,std::string("I/O error within xLock method on the sqlite3_io_methods object while trying to obtain a read lock")),
         std::make_pair(SQLITE_IOERR_DELETE,std::string("I/O error within xDelete method on the sqlite3_vfs object")),
         std::make_pair(SQLITE_IOERR_BLOCKED,std::string("Deprecated error code (SQLITE_IOERR_BLOCKED),should not happen")),
         std::make_pair(SQLITE_IOERR_NOMEM,std::string("An operation could not be completed due to the inability to allocate sufficient memory")),
         std::make_pair(SQLITE_IOERR_ACCESS,std::string("I/O error within the xAccess method on the sqlite3_vfs object")),
         std::make_pair(SQLITE_IOERR_CHECKRESERVEDLOCK,std::string("I/O error within the xCheckReservedLock method on the sqlite3_io_methods object")),
         std::make_pair(SQLITE_IOERR_LOCK,std::string("I/O error in the advisory file locking logic, usually a problem obtaining a PENDING lock")),
         std::make_pair(SQLITE_IOERR_CLOSE,std::string("I/O error within the xClose method on the sqlite3_io_methods object")),
         std::make_pair(SQLITE_IOERR_DIR_CLOSE,std::string("Deprecated error code (SQLITE_IOERR_DIR_CLOSE),should not happen")),
         std::make_pair(SQLITE_IOERR_SHMOPEN,std::string("I/O error within the xShmMap method on the sqlite3_io_methods object while trying to open a new shared memory segment")),
         std::make_pair(SQLITE_IOERR_SHMSIZE,std::string("I/O error within the xShmMap method on the sqlite3_io_methods object while trying to resize an existing shared memory segment")),
         std::make_pair(SQLITE_IOERR_SHMLOCK,std::string("Deprecated error code (SQLITE_IOERR_SHMLOCK),should not happen")),
         std::make_pair(SQLITE_IOERR_SHMMAP,std::string("I/O error within the xShmMap method on the sqlite3_io_methods object while trying to map a shared memory segment into the process address space")),
         std::make_pair(SQLITE_IOERR_SEEK,std::string("I/O error within the xRead or xWrite methods on the sqlite3_io_methods object while trying to seek a file descriptor to the beginning point of the file where the read or write is to occur")),
         std::make_pair(SQLITE_IOERR_DELETE_NOENT,std::string("xDelete method on the sqlite3_vfs object failed because the file being deleted does not exist")),
         std::make_pair(SQLITE_IOERR_MMAP,std::string("I/O error within the xFetch or xUnfetch methods on the sqlite3_io_methods object while trying to map or unmap part of the database file into the process address space")),
         std::make_pair(SQLITE_IOERR_GETTEMPPATH,std::string("VFS is unable to determine a suitable directory in which to place temporary files")),
         std::make_pair(SQLITE_IOERR_CONVPATH,std::string("cygwin_conv_path() system call failed (used only by Cygwin VFS)")),
         std::make_pair(SQLITE_IOERR_VNODE,std::string("SQLITE_IOERR_VNODE")),
         std::make_pair(SQLITE_IOERR_AUTH,std::string("SQLITE_IOERR_AUTH")),
         std::make_pair(SQLITE_LOCKED_SHAREDCACHE,std::string("The locking conflict has occurred due to contention with a different database connection that happens to hold a shared cache with the database connection to which the error was returned")),
         std::make_pair(SQLITE_BUSY_RECOVERY,std::string("An operation could not continue because another process is busy recovering a WAL mode database file following a crash")),
         std::make_pair(SQLITE_BUSY_SNAPSHOT,std::string("Database connection tried to promote a read transaction into a write transaction but finds that another database connection has already written to the database and thus invalidated prior reads")),
         std::make_pair(SQLITE_CANTOPEN_NOTEMPDIR,std::string("Deprecated error code (SQLITE_CANTOPEN_NOTEMPDIR),should not happen")),
         std::make_pair(SQLITE_CANTOPEN_ISDIR,std::string("A file open operation failed because the file is really a directory")),
         std::make_pair(SQLITE_CANTOPEN_FULLPATH,std::string("A file open operation failed because the operating system was unable to convert the filename into a full pathname")),
         std::make_pair(SQLITE_CANTOPEN_CONVPATH,std::string("cygwin_conv_path() system call failed while trying to open a file (used only by Cygwin VFS)")),
         std::make_pair(SQLITE_CORRUPT_VTAB,std::string("Content in the virtual table is corrupt")),
         std::make_pair(SQLITE_READONLY_RECOVERY,std::string("A WAL mode database cannot be opened because the database file needs to be recovered and recovery requires write access but only read access is available")),
         std::make_pair(SQLITE_READONLY_CANTLOCK,std::string("Unable to obtain a read lock on a WAL mode database because the shared-memory file associated with that database is read-only")),
         std::make_pair(SQLITE_READONLY_ROLLBACK,std::string("A database cannot be opened because it has a hot journal that needs to be rolled back but cannot because the database is readonly")),
         std::make_pair(SQLITE_READONLY_DBMOVED,std::string("A database cannot be modified because the database file has been moved since it was opened, and so any attempt to modify the database might result in database corruption if the processes crashes because the rollback journal would not be correctly named")),
         std::make_pair(SQLITE_ABORT_ROLLBACK,std::string("SQL statement aborted because the transaction that was active when the SQL statement first started was rolled back")),
         std::make_pair(SQLITE_CONSTRAINT_CHECK,std::string("CHECK constraint failed")),
         std::make_pair(SQLITE_CONSTRAINT_COMMITHOOK,std::string("A commit hook callback returned non-zero that thus caused the SQL statement to be rolled back")),
         std::make_pair(SQLITE_CONSTRAINT_FOREIGNKEY,std::string("Foreign key constraint failed")),
         std::make_pair(SQLITE_CONSTRAINT_FUNCTION,std::string("Not used in sqlite, can be used by external functions")),
         std::make_pair(SQLITE_CONSTRAINT_NOTNULL,std::string("NOT NULL constraint failed")),
         std::make_pair(SQLITE_CONSTRAINT_PRIMARYKEY,std::string("PRIMARY KEY constraint failed")),
         std::make_pair(SQLITE_CONSTRAINT_TRIGGER,std::string("RAISE function within a trigger fired, causing the SQL statement to abort")),
         std::make_pair(SQLITE_CONSTRAINT_UNIQUE,std::string("UNIQUE constraint failed")),
         std::make_pair(SQLITE_CONSTRAINT_VTAB,std::string("Not used in sqlite, can be used by virtual tables")),
         std::make_pair(SQLITE_CONSTRAINT_ROWID,std::string("rowid is not unique but should be")),
         std::make_pair(SQLITE_NOTICE_RECOVER_WAL,std::string("This code was passed to the callback of sqlite3_log() when a WAL mode database file is recovered")),
         std::make_pair(SQLITE_NOTICE_RECOVER_ROLLBACK,std::string("This code was passed to the callback of sqlite3_log() when a hot journal is rolled back")),
         std::make_pair(SQLITE_WARNING_AUTOINDEX,std::string("This code was passed to the callback of sqlite3_log() whenever automatic indexing is used")),
         std::make_pair(SQLITE_AUTH_USER,std::string("SQLITE_AUTH_USER")),
         std::make_pair(SQLITE_OK_LOAD_PERMANENTLY,std::string("Return code for loading extension"))
};

SqliteConnection::DBQuery::DBQuery(SqliteConnection *pConnection, const std::string &Query,const std::string& func)
   :QueryStr(Query),pConn(pConnection),FuncName(func)
{
   pConn->Queries.insert(this);
}

SqliteConnection::DBQuery::DBQuery()
{

}

SqliteConnection::DBQuery::DBQuery(DBQuery &&query)
{
   *this=std::move(query);
}

SqliteConnection::DBQuery::~DBQuery()
{
   try
   {
      ClearStmt();
      if(pConn)
         pConn->Queries.erase(this);
   }
   catch(...){}
}

void SqliteConnection::DBQuery::operator =(DBQuery&& query)
{
   if(pConn && query.pConn!=pConn)
   {
      if(pQuery)
         sqlite3_finalize(pQuery);
      pConn->Queries.erase(this);
   }
   if(query.pConn)
   {
      if(query.pConn!=pConn)
         query.pConn->Queries.insert(this);
      query.pConn->Queries.erase(&query);
   }
   pConn=query.pConn;
   Input=std::move(query.Input);
   Output=std::move(query.Output);
   QueryStr=std::move(query.QueryStr);
   pQuery=query.pQuery;
   FuncName=std::move(query.FuncName);
   bPrepared=query.bPrepared;
   bExecuted=query.bExecuted;
   query.pConn=nullptr;
   query.pQuery=nullptr;
   query.bPrepared=false;
}

void SqliteConnection::DBQuery::ThrowError(const std::string &Message)
{
   throw DB::Error(FuncName,Message);
}

void SqliteConnection::DBQuery::RealPrepare()
{
   if(!bPrepared)
   {
      if(!pConn)
         ThrowError("Query is not attached to any database connection, exec error");
      if(!pConn->Connected())
         ThrowError("Query is attached to a database connection but it is closed, exec error");
      if(pQuery)
         sqlite3_finalize(pQuery);
      SqliteConnection::CheckError(sqlite3_prepare_v2(pConn->pDB,QueryStr.c_str(),QueryStr.size(),&pQuery,nullptr),FuncName,QueryStr);
      if(!pQuery)
         ThrowError("Unable to create prepared query");
      bPrepared=true;
   }
   if(pQuery)
      for(std::size_t i=0;i<Input.size();i++)
         Input[i]->arg(pQuery,int(i+1));
}

void SqliteConnection::DBQuery::ClearStmt()
{
   if(pQuery)
   {
      sqlite3_finalize(pQuery);
      pQuery=nullptr;
      bPrepared=false;
   }
}

void SqliteConnection::DBQuery::ResetStmt()
{
   if(pQuery && bExecuted)
   {
      SqliteConnection::CheckError(sqlite3_reset(pQuery),FuncName,QueryStr);
      bExecuted=false;
      Input.clear();

//      std::cout<<"Query reset"<<std::endl;
   }
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const double &value)
{
   ResetStmt();
   Input.emplace_back(new InputDouble<double>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const float &value)
{
   ResetStmt();
   Input.emplace_back(new InputDouble<float>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const bool &value)
{
   ResetStmt();
   Input.emplace_back(new InputInt<bool>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::int8_t &value)
{
   ResetStmt();
   Input.emplace_back(new InputInt<std::int8_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::int16_t &value)
{
   ResetStmt();
   Input.emplace_back(new InputInt<std::int16_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const int &value)
{
   ResetStmt();
   Input.emplace_back(new InputInt<int>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::int64_t &value)
{
   ResetStmt();
   Input.emplace_back(new InputInt64(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::uint8_t &value)
{
   ResetStmt();
   Input.emplace_back(new InputUint<std::uint8_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::uint16_t &value)
{
   ResetStmt();
   Input.emplace_back(new InputUint<std::uint16_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::uint32_t &value)
{
   ResetStmt();
   Input.emplace_back(new InputUint<std::uint32_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::uint64_t& value)
{
   ResetStmt();
   Input.emplace_back(new InputUInt64(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::arg(const std::string &value)
{
   ResetStmt();
   Input.emplace_back(new InputString(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(double &value)
{
   Output.emplace_back(new OutputDouble<double>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(float &value)
{
   Output.emplace_back(new OutputDouble<float>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(bool& value)
{
   Output.emplace_back(new OutputInt<bool>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::int8_t& value)
{
   Output.emplace_back(new OutputInt<std::int8_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::int16_t& value)
{
   Output.emplace_back(new OutputInt<std::int16_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(int& value)
{
   Output.emplace_back(new OutputInt<int>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::int64_t &value)
{
   Output.emplace_back(new OutputInt64(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::uint8_t& value)
{
   Output.emplace_back(new OutputUInt<std::uint8_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::uint16_t& value)
{
   Output.emplace_back(new OutputUInt<std::uint16_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::uint32_t& value)
{
   Output.emplace_back(new OutputUInt<std::uint32_t>(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::uint64_t& value)
{
   Output.emplace_back(new OutputUInt64(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(std::string &value)
{
   Output.emplace_back(new OutputString(value));
   return *this;
}

SqliteConnection::DBQuery& SqliteConnection::DBQuery::oarg(void)
{
   Output.emplace_back(new OutputNone);
   return *this;
}

void SqliteConnection::DBQuery::exec()
{
   RealPrepare();
}

bool SqliteConnection::DBQuery::next()
{
   if(pQuery)
   {
      int res=sqlite3_step(pQuery);

//      std::cout<<"Step"<<std::endl;

      bExecuted=true;
      if(res == SQLITE_ROW)
      {
         for(std::size_t i=0;i<Output.size();i++)
            Output[i]->get(pQuery,i);
         return true;
      }
      else if(res==SQLITE_DONE)
      {
         sqlite3_reset(pQuery);
         Input.clear();
         return false;
      }
      else
         throw SqliteError(FuncName,res);
   }
   return false;
}

SqliteConnection::SqliteConnection()
   : TableExistQ(this,"select count(tbl_name) from SQLITE_MASTER where type='table' and upper(tbl_name)=upper(?1)","SqliteConnection::CheckTable()")
{
   TableExistQ.oarg(ntables);
}
SqliteConnection::~SqliteConnection()
{
   if(Connected())
      Disconnect();
   for(auto ptr: Queries)
   {
      ptr->ClearStmt();
      ptr->pConn=nullptr;
   }
}

void SqliteConnection::OpenDB(const std::string &FileName)
{
   if(Connected())
      Disconnect();
   _FileName=FileName;
   Connect();
}

SqliteConnection::DBQuery SqliteConnection::Query(const std::string &Request, const std::string &FuncName)
{
   return DBQuery(this,Request,FuncName);
}

void SqliteConnection::DirectExec(const std::string &Request, const std::string &FuncName)
{
   if(Connected())
      CheckError(sqlite3_exec(pDB,Request.c_str(),nullptr,nullptr,nullptr),FuncName,Request);
}

bool SqliteConnection::CheckTable(const std::string &TableName, const PLAINCOLUMNS &types, const std::vector<std::string> &PrimaryInds)
{
   static std::string FuncName("SqliteConnection::CheckTable()");
   static std::map<DB::Types,std::string> TypeMap{
      std::make_pair(TypeString,std::string("TEXT")),
            std::make_pair(TypeInteger,std::string("INTEGER")),
            std::make_pair(TypeSmallint,std::string("INTEGER")),
            std::make_pair(TypeDatetime,std::string("TEXT")),
            std::make_pair(TypeDouble,std::string("REAL")),
            std::make_pair(TypeClob,std::string("TEXT")),
            std::make_pair(TypeBlob,std::string("BLOB"))
   };
   bool ret=true;
   if(Connected())
   {
      TableExistQ.arg(TableName);
      TableExistQ.exec();
      if(TableExistQ.next())
      {
         if(ntables>0)
         {

//            std::cout<<"Found table "<<TableName<<std::endl;

            DBQuery ColInfoQ(this,"pragma table_info("+TableName+")",FuncName);
            std::map<std::string,std::pair<std::string,int>> cols;
            std::string ColName,ColType;
            int primary,nprims=0;
            ColInfoQ.oarg().oarg(ColName,ColType).oarg().oarg().oarg(primary);
            ColInfoQ.exec();
            while(ColInfoQ.next())
            {
               cols[ColName]=std::make_pair(ColType,primary);
               nprims+=primary;
            }
            //checking columns by name, adding if necessary
            for(auto type:types)
               if(cols.count(type.Name)==0)
               {
                  DirectExec("alter table "+TableName+
                                   " add "+type.Name+" "+
                                   TypeMap[type.type],FuncName);
                  ret=false;
               }
            //sqlite can't easily drop columns or change primary keys so that's it
         }
         else
         {
            std::string CreateTableQ="create table "+TableName;
            if(types.size())
            {
               CreateTableQ+="( "+types[0].Name+" "+TypeMap[types[0].type];
               for(std::size_t i=1;i<types.size();i++)
               {
                  CreateTableQ+=",";
                  CreateTableQ+=types[i].Name;
                  CreateTableQ+=" ";
                  CreateTableQ+=TypeMap[types[i].type];
               }
               if(PrimaryInds.size())
               {
                  CreateTableQ+=", primary key (";
                  CreateTableQ+=PrimaryInds[0];
                  for(std::size_t i=1;i<PrimaryInds.size();i++)
                  {
                     CreateTableQ+=",";
                     CreateTableQ+=PrimaryInds[i];
                  }
                  CreateTableQ+=(")");
               }
               CreateTableQ+=")";
            }
            DirectExec(CreateTableQ,FuncName);
            ret=false;
         }
      }
   }
   return ret;
}

void SqliteConnection::Connect()
{
   if(!Connected())
      CheckError(sqlite3_open_v2(_FileName.c_str(),&pDB,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,nullptr),
                 "SqliteConnection::Connect()");
}

void SqliteConnection::Disconnect()
{
   if(Connected())
   {
      CheckError(sqlite3_close_v2(pDB),"SqliteConnection::Disconnect()");
      for(auto ptr:Queries)
         ptr->ClearStmt();
      pDB=nullptr;
   }
}

bool SqliteConnection::Connected()
{
   return pDB!=nullptr;
}

void SqliteConnection::SetFileName(const std::string &FileName)
{
   if(Connected())
   {
      if(FileName!=_FileName)
      {
         Disconnect();
         _FileName=FileName;
         Connect();
      }
   }
   else
      _FileName=FileName;
}

}
