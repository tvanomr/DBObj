#ifndef __DBOBJ_SQL_UTIL_H__
#define __DBOBJ_SQL_UTIL_H__\

#include "DBObj/Connection.h"
#include "DBObj/Features.h"
#include <vector>
#include <memory>
#include <type_traits>

namespace DBObj
{

template<class Conn,std::size_t Features,std::size_t num,class Condition=void>
class QueryStorage{};

template<class Conn,std::size_t Features,std::size_t num>
class QueryStorage<Conn,Features,num,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
   typedef typename Connection<Conn,Features>::DBQuery DBQuery;
   typedef std::vector<void*> VQPtr;
   std::vector<std::unique_ptr<DBQuery>> Queries;
   std::vector<std::unique_ptr<VQPtr>> Vectors;
   VQPtr* pQueriesMap;
   VQPtr* CreateVec(std::size_t maxnum)
   {
      Vectors.emplace_back(new VQPtr(maxnum+1));
      VQPtr* ptr=Vectors.back().get();
      if(ptr)
      {
         (*ptr)[0]=nullptr;
         for(std::size_t i=1;i<=maxnum;i++)
            (*ptr)[i]=CreateVec(i-1);
      }
      return ptr;
   }
   template<class Inds>
   typename std::enable_if<GenTempl::Length<Inds>::value==0,void*&>::type
   FindQuery(VQPtr* pRoot)
   {
      return pRoot->at(0);
   }
   template<class Inds>
   typename std::enable_if<(GenTempl::Length<Inds>::value>0),void*&>::type
   FindQuery(VQPtr* pRoot)
   {
      return FindQuery<typename GenTempl::Tail<Inds,1>::type>(static_cast<VQPtr*>(pRoot->at(GenTempl::Get<Inds,0>::value)));
   }

   template<std::size_t value,std::size_t ind,std::size_t... inds,class Type,class... Types>
   typename std::enable_if<value==ind,void>::type
   InnerArg(DBQuery* pQuery,const Type& Bound1,const Type& Bound2,const Types&...)
   {
      TypeManipSQL::MassArg<Conn,Features>(*pQuery,Bound1,Bound2);
   }

   template<std::size_t value,std::size_t ind,std::size_t... inds,class Type,class... Types>
   typename std::enable_if<value!=ind,void>::type
   InnerArg(DBQuery* pQuery,const Type&,const Type&,const Types&... Bounds)
   {
      InnerArg<value,inds...>(pQuery,Bounds...);
   }

   template<class Inds,std::size_t iind,std::size_t... inds,class... Types>
   typename std::enable_if<(GenTempl::Length<Inds>::value<=iind),void>::type
   OuterArg(DBQuery*,const Types&...){}

   template<class Inds,std::size_t iind,std::size_t... inds,class... Types>
   typename std::enable_if<(GenTempl::Length<Inds>::value>iind),void>::type
   OuterArg(DBQuery* pQuery,const Types&... Boundaries)
   {
      InnerArg<GenTempl::Get<Inds,iind>::value,inds...>(pQuery,Boundaries...);
      OuterArg<Inds,iind+1,inds...>(pQuery,Boundaries...);
   }


public:
   QueryStorage():pQueriesMap(CreateVec(num)){}
   template<std::size_t... inds>
   bool HaveQuery()
   {
      return FindQuery<typename GenTempl::SortUnique<inds...>::type>(pQueriesMap)!=nullptr;
   }
   template<class Obj,std::size_t Offset,std::size_t... inds,class T>
   void CreateSelectQuery(Connection<Conn,Features>& ConnRef,T& values,const std::string& QueryStart,const std::string& QueryEnd,std::string FuncName,std::size_t StartID=1,bool bNeedWhere=true)
   {
      void*& pQuery=FindQuery<typename GenTempl::SortUnique<inds...>::type>(pQueriesMap);
      if(!pQuery)
      {
         Queries.emplace_back(new DBQuery());
         pQuery=Queries.back().get();
      }
      *static_cast<DBQuery*>(pQuery)=ConnRef.Query(QueryStart+DBObj::TypeManipSQL::GetRangeClause<Obj,typename TypeManip::GetPropIndicesByValues<Obj,typename GenTempl::Sub<typename GenTempl::Add<typename GenTempl::Revert<typename GenTempl::SortUnique<inds...>::type>::type,GenTempl::Values<Offset>>::type,GenTempl::Values<1>>::type>::type>(StartID,bNeedWhere)+QueryEnd,FuncName);
      TypeManipSQL::BindOargs<0,Conn,Features>(*static_cast<DBQuery*>(pQuery),values);
   }

   template<class Obj,std::size_t Offset,std::size_t... inds>
   void CreateDeleteQuery(Connection<Conn,Features>& ConnRef,const std::string& QueryStart,const std::string& QueryEnd,std::string FuncName,std::size_t StartID=1,bool bNeedWhere=true)
   {
      void*& pQuery=FindQuery<typename GenTempl::SortUnique<inds...>::type>(pQueriesMap);
      if(!pQuery)
      {
         Queries.emplace_back(new DBQuery());
         pQuery=Queries.back().get();
      }
      *static_cast<DBQuery*>(pQuery)=ConnRef.Query(QueryStart+DBObj::TypeManipSQL::GetRangeClause<Obj,typename TypeManip::GetPropIndicesByValues<Obj,typename GenTempl::Sub<typename GenTempl::Add<typename GenTempl::Revert<typename GenTempl::SortUnique<inds...>::type>::type,GenTempl::Values<Offset>>::type,GenTempl::Values<1>>::type>::type>(StartID,bNeedWhere)+QueryEnd,FuncName);
   }

   template<std::size_t... inds,class... Types>
   typename std::enable_if<sizeof...(inds)*2==sizeof...(Types),typename Connection<Conn,Features>::DBQuery*>::type
   ArgAndRun(const Types&... Boundaries)
   {
      DBQuery* pQuery=static_cast<DBQuery*>(FindQuery<typename GenTempl::SortUnique<inds...>::type>(pQueriesMap));
      if(pQuery)
      {
         OuterArg<typename GenTempl::Revert<typename GenTempl::SortUnique<inds...>::type>::type,0,inds...>(pQuery,Boundaries...);
         pQuery->exec();
      }
      return pQuery;
   }
};
}

#endif
