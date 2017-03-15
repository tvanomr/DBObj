#ifndef __DBOBJ_DB_ERROR_H__
#define __DBOBJ_DB_ERROR_H__

#include <stdexcept>

namespace DB
{

class Error: public std::runtime_error
{
   std::string _FuncName;
   static std::string MakeError(const std::string& FuncName, const std::string& ErrorMessage)
   {
      return (FuncName.size()?("Error in "+FuncName+": "):(std::string()))+ErrorMessage;
   }
public:
   Error(const std::string& FuncName,const std::string& ErrorMessage)
      :std::runtime_error(MakeError(FuncName,ErrorMessage)),_FuncName(FuncName){}
   virtual const char* where() const
   {
      return _FuncName.c_str();
   }
   virtual ~Error() throw(){}
};

}

#endif
