#ifndef __DBOBJ_OBJINFO_SQL_UTIL_H__
#define __DBOBJ_OBJINFO_SQL_UTIL_H__

#include "DBObj/ObjInfoUtil.h"
#include "DB/ConnectionBase.h"
#include "Templates/TupleManip.h"

namespace DBObj
{

template<class Conn,std::size_t Features>
class Connection;

namespace TypeManipSQL
{

template<std::size_t index,class Conn,std::size_t Features,class Values>
typename std::enable_if<(index==std::tuple_size<Values>::value),void>::type BindOargs(typename Connection<Conn,Features>::DBQuery&,Values&)
{
}

template<std::size_t index,class Conn,std::size_t Features,class Values>
typename std::enable_if<(index<std::tuple_size<Values>::value),void>::type BindOargs(typename Connection<Conn,Features>::DBQuery& query,Values& values)
{
   query.oarg(std::get<index>(values));
   BindOargs<index+1,Conn,Features>(query,values);
}

template<class Inds,std::size_t index,class Obj,class Values,class Conn,std::size_t Features>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,void>::type MoveValues(Obj*,Values&,Connection<Conn,Features>*)
{

}

template<class Inds,std::size_t index,class Obj,class Values,class Conn,std::size_t Features>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,void>::type MoveValues(Obj* pObj,Values& values,Connection<Conn,Features>* pConn)
{
   TypeManip::TypeInfo<TypeManip::IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::template MoveValue<Conn,Values,index>(std::get<GenTempl::FirstInd<Inds>::value>(ObjInfo<Obj>::info).Get(pObj),values,pConn);
   MoveValues<typename GenTempl::LastIndices<Inds>::type,index+std::tuple_size<typename TypeManip::TypeInfo<TypeManip::IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::IntType>::value>(pObj,values,pConn);
}

template<class Inds,class Conn,std::size_t Features,class Obj>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,void>::type ArgAll(Obj*,typename Connection<Conn,Features>::DBQuery&)
{

}

template<class Inds,class Conn,std::size_t Features,class Obj>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,void>::type ArgAll(Obj* pObj,typename Connection<Conn,Features>::DBQuery& query)
{
   TypeManip::TypeInfo<TypeManip::IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::template Arg<Conn>(std::get<GenTempl::FirstInd<Inds>::value>(ObjInfo<Obj>::info).Get(pObj),query);
   ArgAll<typename GenTempl::LastIndices<Inds>::type,Conn,Features>(pObj,query);
}

template<class Obj,std::size_t Features,class Types,class Type,std::size_t i,std::size_t ind>
typename std::enable_if<std::tuple_size<Types>::value==0,void>::type GetOneColumnInfo(const std::string &, DB::ConnectionBase::PLAINCOLUMNS &)
{

}

template<class Obj,std::size_t Features,class Types,class Type,std::size_t i,std::size_t ind>
typename std::enable_if<std::tuple_size<Types>::value!=0 && TypeManip::HaveSuffixes<Type,Features>::value,void>::type GetOneColumnInfo(const std::string& ColName,DB::ConnectionBase::PLAINCOLUMNS& cols)
{
   cols.push_back({ColName+TypeManip::TypeInfo<Type,0>::Suffixes[i],DB::GetColType<typename GenTempl::FirstType<Types>::type>::value});
   GetOneColumnInfo<Obj,Features,typename GenTempl::LastTypes<Types>::type,Type,i+1,ind>(ColName,cols);
}

template<class Obj,std::size_t Features,class Types,class Type,std::size_t i,std::size_t ind>
typename std::enable_if<std::tuple_size<Types>::value==1 && !TypeManip::HaveSuffixes<Type,Features>::value,void>::type GetOneColumnInfo(const std::string& ColName, DB::ConnectionBase::PLAINCOLUMNS& cols)
{
   if(std::get<ind>(ObjInfo<Obj>::info).ColumnType!=DB::TypeUnknown)
      cols.push_back({ColName,static_cast<DB::Types>(std::get<ind>(ObjInfo<Obj>::info).ColumnType)});
   else
      cols.push_back({ColName,DB::GetColType<typename GenTempl::FirstType<Types>::type>::value});
}

template<class Obj,std::size_t Features,class Inds>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,void>::type GetColumnInfo(DB::ConnectionBase::PLAINCOLUMNS&)
{

}

template<class Obj,std::size_t Features,class Inds>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,void>::type GetColumnInfo(DB::ConnectionBase::PLAINCOLUMNS& cols)
{
   GetOneColumnInfo<Obj,Features,typename TypeManip::TypeInfo<TypeManip::IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::IntType,TypeManip::IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,0,GenTempl::FirstInd<Inds>::value>(std::get<GenTempl::FirstInd<Inds>::value>(ObjInfo<Obj>::info).ColumnName,cols);
   GetColumnInfo<Obj,Features,typename GenTempl::LastIndices<Inds>::type>(cols);
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value==0),std::string>::type
GetRangeClauseImpl(std::size_t,std::size_t)
{
   return std::string();
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value>0),std::string>::type
GetRangeClauseImpl(std::size_t StartID,std::size_t ind)
{
   return std::string(" and ")+std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName+
         std::string(">=?")+std::to_string(StartID+ind*2)+std::string(" and ")+
         std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName+
         std::string("<=?")+std::to_string(StartID+ind*2+1)+
         GetRangeClauseImpl<Obj,typename GenTempl::Tail<Inds,1>::type>(StartID,ind+1);
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value>0),std::string>::type
GetRangeClause(std::size_t StartID=1,bool bNeedWhere=true)
{
   return (bNeedWhere?std::string(" where "):std::string(" "))+std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName+
         std::string(">=?")+std::to_string(StartID)+std::string(" and ")+
         std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName+
         std::string("<=?")+std::to_string(StartID+1)+
         GetRangeClauseImpl<Obj,typename GenTempl::Tail<Inds,1>::type>(StartID,1);
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value==0),std::string>::type
GetRangeClause(std::size_t StartID=1,bool bNeedWhere=true)
{
   return std::string();
}


template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value==0),std::string>::type
GetEqualClauseImpl(std::size_t,std::size_t)
{
   return std::string();
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value>0),std::string>::type
GetEqualClauseImpl(std::size_t StartID, std::size_t i)
{
   return std::string(" and ")+std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName+
         std::string("=")+std::to_string(StartID+i)+
         GetEqualClauseImpl<Obj,typename GenTempl::Tail<Inds,1>::type>(StartID,i+1);
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value==0),std::string>::type
GetEqualClause(std::size_t StartID=1,bool bNeedWhere=true)
{
   return std::string();
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value>0),std::string>::type
GetEqualClause(std::size_t StartID=1, bool bNeedWhere=true)
{
   return (bNeedWhere?std::string(" where "):std::string(" "))+std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName+
         std::string("=")+std::to_string(StartID)+
         GetEqualClauseImpl<Obj,typename GenTempl::Tail<Inds,1>::type>(StartID,1);
}

template<class Obj,std::size_t index,bool bFound,class Cond=void>
struct GetOrderByClauseImpl;

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,true,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value==(index+1)) &&
      GenTempl::HaveValue<ValueType::Ascend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return ","+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" asc";
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,true,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value==(index+1)) &&
      GenTempl::HaveValue<ValueType::Descend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return ","+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" desc";
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,false,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value==(index+1)) &&
      GenTempl::HaveValue<ValueType::Ascend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return " order by "+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" asc";
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,false,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value==(index+1)) &&
      GenTempl::HaveValue<ValueType::Descend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return " order by "+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" desc";
   }
};

template<class Obj,std::size_t i,bool bFound>
struct GetOrderByClauseImpl<Obj,i,bFound,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value==(i+1)) &&
      !GenTempl::HaveValue<ValueType::Descend,typename TypeManip::IthPropPIT<Obj,i>::Values>::value &&
      !GenTempl::HaveValue<ValueType::Ascend,typename TypeManip::IthPropPIT<Obj,i>::Values>::value,void>::type>
{
   static std::string f()
   {
      return std::string();
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,true,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value>(index+1)) &&
      GenTempl::HaveValue<ValueType::Ascend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return ","+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" asc"+
            GetOrderByClauseImpl<Obj,index+1,true>::f();
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,true,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value>(index+1)) &&
      GenTempl::HaveValue<ValueType::Descend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return ","+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" desc"+
            GetOrderByClauseImpl<Obj,index+1,true>::f();
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,false,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value>(index+1)) &&
      GenTempl::HaveValue<ValueType::Ascend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return " order by "+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" asc"+
            GetOrderByClauseImpl<Obj,index+1,true>::f();
   }
};

template<class Obj,std::size_t index>
struct GetOrderByClauseImpl<Obj,index,false,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value>(index+1)) &&
      GenTempl::HaveValue<ValueType::Descend,typename TypeManip::IthPropPIT<Obj,index>::Values>::value,void>::type>
{
   static std::string f()
   {
      return " order by "+std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+" desc"+
            GetOrderByClauseImpl<Obj,index+1,true>::f();
   }
};

template<class Obj,std::size_t i,bool bFound>
struct GetOrderByClauseImpl<Obj,i,bFound,typename std::enable_if<
      (std::tuple_size<decltype(ObjInfo<Obj>::info)>::value>(i+1)) &&
      !GenTempl::HaveValue<ValueType::Descend,typename TypeManip::IthPropPIT<Obj,i>::Values>::value &&
      !GenTempl::HaveValue<ValueType::Ascend,typename TypeManip::IthPropPIT<Obj,i>::Values>::value,void>::type>
{
   static std::string f()
   {
      return GetOrderByClauseImpl<Obj,i+1,bFound>::f();
   }
};

template<class Obj>
std::string GetOrderByClause()
{
   return GetOrderByClauseImpl<Obj,0,false>::f();
}

template<class Obj,class Inds>
typename std::enable_if<GenTempl::Length<Inds>::value==0,DB::ConnectionBase::PLAINCOLUMNS>::type GetColumnList()
{
   return DB::ConnectionBase::PLAINCOLUMNS();
}

template<class Obj,class Inds>
typename std::enable_if<(GenTempl::Length<Inds>::value>0),DB::ConnectionBase::PLAINCOLUMNS>::type GetColumnList()
{
   DB::ConnectionBase::PLAINCOLUMNS cols=GetColumnList<Obj,typename GenTempl::Tail<Inds,1>::type>();
   cols.emplace_back(std::get<GenTempl::Get<Inds,0>::value>(ObjInfo<Obj>::info).ColumnName);
   return cols;
}

template<class Conn,std::size_t Features,class Query,class Type,class... Types>
typename std::enable_if<(sizeof...(Types)==0),void>::type
MassArgImpl(Query& query,const Type& value,const Types&... values)
{
   TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(value,query);
}

template<class Conn,std::size_t Features,class Query,class Type,class... Types>
typename std::enable_if<(sizeof...(Types)>0),void>::type
MassArgImpl(Query& query,const Type& value,const Types&... values)
{
   TypeManip::TypeInfo<Type,Features>::template Arg<Conn>(value,query);
   MassArgImpl<Conn,Features>(query,values...);
}

template<class Conn,std::size_t Features,class Query,class... Types>
typename std::enable_if<(sizeof...(Types)>0),void>::type
MassArg(Query& query,const Types&... values)
{
   MassArgImpl<Conn,Features>(query,values...);
}

template<class Conn,std::size_t Features,class Query,class... Types>
typename std::enable_if<(sizeof...(Types)==0),void>::type
MassArg(Query&,const Types&...){}

}

}

#endif
