#ifndef __DBOBJ_OBJINFO_UTIL_H__
#define __DBOBJ_OBJINFO_UTIL_H__

#include "DBObj/ObjInfo.h"
#include "Templates/Indices.h"
#include "Templates/HaveAny.h"
#include "DBObj/Types.h"
#include <string>
#include <tuple>

namespace DBObj
{

namespace TypeManip
{

template<class T1,class T2>
struct MergeTuples{};

template<class... T1T,class... T2T>
struct MergeTuples<std::tuple<T1T...>,std::tuple<T2T...>>
{
   typedef std::tuple<T1T...,T2T...> type;
};

template<class Obj>
struct ObjInfoTT
{
   typedef typename std::remove_cv<decltype(ObjInfo<Obj>::info)>::type type;
};

template<class T>
struct PropInfoTypes{};

template<class _GetF,class _Type,std::size_t... _ValueTypes>
struct PropInfoTypes<PropInfo<_GetF,_Type,GenTempl::Values<_ValueTypes...>>>
{
   typedef _Type Type;
   typedef GenTempl::Values<_ValueTypes...> Values;
};

template<class _GetF,class _Type,std::size_t... _ValueTypes>
struct PropInfoTypes<const PropInfo<_GetF,_Type,GenTempl::Values<_ValueTypes...>>>
{
   typedef _Type Type;
   typedef GenTempl::Values<_ValueTypes...> Values;
};

template<class Obj,std::size_t ind>
using IthPropPIT=PropInfoTypes<typename std::tuple_element<ind,typename ObjInfoTT<Obj>::type>::type>;

template<class Obj,std::size_t ind>
using IthPropType=typename PropInfoTypes<typename std::tuple_element<ind,typename ObjInfoTT<Obj>::type>::type>::Type;

template<class Type,std::size_t Features>
struct HaveIntType : public GenTempl::HaveAny
{
   static no check(...);
   template<class _Type>
   static yes check(_Type*,Helper<typename _Type::IntType>* =nullptr);
   static constexpr bool value=(sizeof(yes)==sizeof(check(static_cast<TypeInfo<Type,Features>*>(nullptr))));
};

template<class Type,std::size_t Features>
struct HaveSuffixes : public GenTempl::HaveAny
{
   static no check(...);
   template<class _Type>
   static yes check(_Type*,Helper<decltype(_Type::Suffixes)>* =nullptr);
   static constexpr bool value=(sizeof(yes)==sizeof(check(static_cast<TypeInfo<Type,Features>*>(nullptr))));
};

template<class Type,std::size_t Features,class Conn,class Parent>
struct HaveSpecial : public GenTempl::HaveAny
{
   static no check(...);
   template<class _Type>
   static yes check(_Type*,Helper<typename _Type::template Special<Conn,Parent>>* =nullptr);
   static constexpr bool value=(sizeof(yes)==sizeof(check(static_cast<TypeInfo<Type,Features>*>(nullptr))));
};

template<class Type,std::size_t Features,class Conn,class Parent>
struct HaveSpecialEditor : public GenTempl::HaveAny
{
   static no check(...);
   template<class _Type>
   static yes check(_Type*,Helper<typename _Type::template SpecialEditor<Conn,Parent>>* =nullptr);
   static constexpr bool value=(sizeof(yes)==sizeof(check(static_cast<TypeInfo<Type,Features>*>(nullptr))));
};

//generate tuple type to hold intermediate values when reading from DB,implementation

template<class T,std::size_t Features,class Collection,class Condition=void>
struct GetIntValuesTImpl{};

template<class PI,class... PIs,std::size_t Features,class... Types>
struct GetIntValuesTImpl<std::tuple<PI,PIs...>,Features,std::tuple<Types...> ,
      typename std::enable_if<HaveIntType<typename PropInfoTypes<PI>::Type,Features>::value,void>::type>
   :public GetIntValuesTImpl<std::tuple<PIs...>,Features,
      typename MergeTuples<std::tuple<Types...>,
      typename TypeInfo<typename PropInfoTypes<PI>::Type,Features>::IntType>::type>{};

template<class PI,class... PIs,std::size_t Features,class... Types>
struct GetIntValuesTImpl<std::tuple<PI,PIs...>,Features,std::tuple<Types...>,
      typename std::enable_if<!HaveIntType<typename PropInfoTypes<PI>::Type,Features>::value,void>::type>
      :public GetIntValuesTImpl<std::tuple<PIs...>,Features,std::tuple<Types...>>{};

template<std::size_t Features,class... Types>
struct GetIntValuesTImpl<std::tuple<>,Features,std::tuple<Types...>,void>
{
   typedef std::tuple<Types...> type;
};

//generate GenTempl::Indices type to hold indices of properties to be loaded from database in a usual way, implementation

template<class T,std::size_t Features,class Collection,std::size_t index=0,class Condition=void>
struct GetValuesIndicesImpl{};

template<class PI,class... PIs,std::size_t Features,std::size_t index,std::size_t... inds>
struct GetValuesIndicesImpl<std::tuple<PI,PIs...>,Features,GenTempl::Indices<inds...>,index,
      typename std::enable_if<HaveIntType<typename PropInfoTypes<PI>::Type,Features>::value,void>::type>
      :public GetValuesIndicesImpl<std::tuple<PIs...>,Features,GenTempl::Indices<inds...,index>,index+1>{};

template<class PI,class... PIs,std::size_t Features,std::size_t index,std::size_t... inds>
struct GetValuesIndicesImpl<std::tuple<PI,PIs...>,Features,GenTempl::Indices<inds...>,index,
      typename std::enable_if<!HaveIntType<typename PropInfoTypes<PI>::Type,Features>::value,void>::type>
      :public GetValuesIndicesImpl<std::tuple<PIs...>,Features,GenTempl::Indices<inds...>,index+1>{};

template<std::size_t Features,std::size_t index,std::size_t... inds>
struct GetValuesIndicesImpl<std::tuple<>,Features,GenTempl::Indices<inds...>,index,void>
{
   typedef GenTempl::Indices<inds...> type;
};

//generate tuple to hold special loaders

template<class T,std::size_t Features,class Conn,class Parent,class Collection,class Condition=void>
struct GetSpecialTImpl{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,class... Specials>
struct GetSpecialTImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,std::tuple<Specials...>,
      typename std::enable_if<HaveSpecial<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      :public GetSpecialTImpl<std::tuple<PIs...>,Features,Conn,Parent,std::tuple<Specials...,typename TypeInfo<typename PropInfoTypes<PI>::Type,Features>::template Special<Conn,Parent>>>{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,class... Specials>
struct GetSpecialTImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,std::tuple<Specials...>,
      typename std::enable_if<!HaveSpecial<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      :public GetSpecialTImpl<std::tuple<PIs...>,Features,Conn,Parent,std::tuple<Specials...>>{};

template<std::size_t Features,class Conn,class Parent,class... Specials>
struct GetSpecialTImpl<std::tuple<>,Features,Conn,Parent,std::tuple<Specials...>,void>
{
   typedef std::tuple<Specials...> type;
};

//generate indices for properties with special loaders

template<class T,std::size_t Features,class Conn,class Parent,class Collection,std::size_t index=0,class Condition=void>
struct GetSpecialIndicesImpl{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,std::size_t index,std::size_t... inds>
struct GetSpecialIndicesImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...>,index,
      typename std::enable_if<HaveSpecial<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      : public GetSpecialIndicesImpl<std::tuple<PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...,index>,index+1>{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,std::size_t index,std::size_t... inds>
struct GetSpecialIndicesImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...>,index,
      typename std::enable_if<!HaveSpecial<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      : public GetSpecialIndicesImpl<std::tuple<PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...>,index+1>{};

template<std::size_t Features,class Conn,class Parent,std::size_t index,std::size_t... inds>
struct GetSpecialIndicesImpl<std::tuple<>,Features,Conn,Parent,GenTempl::Indices<inds...>,index,void>
{
   typedef GenTempl::Indices<inds...> type;
};

//generate tuple for SpecialEditor classes inside TypeInfo

template<class T,std::size_t Features,class Conn,class Parent,class Collection,std::size_t index=0,class Condition=void>
struct GetSpecialEditorTImpl{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,class... SpEs,std::size_t index>
struct GetSpecialEditorTImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,std::tuple<SpEs...>,index,
         typename std::enable_if<HaveSpecialEditor<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      :public GetSpecialEditorTImpl<std::tuple<PIs...>,Features,Conn,Parent,std::tuple<SpEs...,
      typename TypeInfo<typename PropInfoTypes<PI>::Type,Features>::template SpecialEditor<Conn,Parent>>,index+1>{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,class... SpEs,std::size_t index>
struct GetSpecialEditorTImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,std::tuple<SpEs...>,index,
         typename std::enable_if<!HaveSpecialEditor<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      :public GetSpecialEditorTImpl<std::tuple<PIs...>,Features,Conn,Parent,std::tuple<SpEs...>,index+1>{};

template<std::size_t Features,class Conn,class Parent,class... SpEs,std::size_t index>
struct GetSpecialEditorTImpl<std::tuple<>,Features,Conn,Parent,std::tuple<SpEs...>,index,void>
{
   typedef std::tuple<SpEs...> type;
};

//generate indices for SpecialEditor classes inside TypeInfo

template<class T,std::size_t Features,class Conn,class Parent,class Collection,std::size_t index=0,class Condition=void>
struct GetSpecialEditorIndicesImpl{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,std::size_t index,std::size_t... inds>
struct GetSpecialEditorIndicesImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...>,index,
      typename std::enable_if<HaveSpecialEditor<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      : public GetSpecialEditorIndicesImpl<std::tuple<PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...,index>,index+1>{};

template<class PI,class... PIs,std::size_t Features,class Conn,class Parent,std::size_t index,std::size_t... inds>
struct GetSpecialEditorIndicesImpl<std::tuple<PI,PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...>,index,
      typename std::enable_if<!HaveSpecialEditor<typename PropInfoTypes<PI>::Type,Features,Conn,Parent>::value,void>::type>
      : public GetSpecialEditorIndicesImpl<std::tuple<PIs...>,Features,Conn,Parent,GenTempl::Indices<inds...>,index+1>{};

template<std::size_t Features,class Conn,class Parent,std::size_t index,std::size_t... inds>
struct GetSpecialEditorIndicesImpl<std::tuple<>,Features,Conn,Parent,GenTempl::Indices<inds...>,index,void>
{
   typedef GenTempl::Indices<inds...> type;
};

//find property index of a property with a certain property type id

template<class Obj,std::size_t ptid,std::size_t index=0,class Condition=void>
struct GetPropIndex{};

template<class Obj,std::size_t ptid,std::size_t _index>
struct GetPropIndex<Obj,ptid,_index,
         typename std::enable_if<GenTempl::HaveValue<ptid,typename IthPropPIT<Obj,_index>::Values>::value &&
      (_index<(std::tuple_size<decltype(ObjInfo<Obj>::info)>::value-1)),void>::type>
{
   static constexpr size_t value=_index;
};

template<class Obj,std::size_t ptid,std::size_t _index>
struct GetPropIndex<Obj,ptid,_index,
      typename std::enable_if<!GenTempl::HaveValue<ptid,typename IthPropPIT<Obj,_index>::Values>::value &&
      (_index<(std::tuple_size<decltype(ObjInfo<Obj>::info)>::value-1)),void>::type>
   :public GetPropIndex<Obj,ptid,_index+1>{};

template<class Obj,std::size_t ptid>
struct HavePropIndex : public GenTempl::HaveAny
{
   static no check(...);
   template<class _Type>
   static yes check(_Type*,Helper<decltype(_Type::value)>* =nullptr);
   static constexpr bool value=(sizeof(yes)==sizeof(check(static_cast<GetPropIndex<Obj,ptid>*>(nullptr))));
};

// GetPropIndices : returns indices and property type values for a given value range relative to the base+1

template<class Obj,std::size_t ptid,std::size_t ptrange,std::size_t index=0,class Condition=void>
struct GetPropIndices{};

template<class Obj,std::size_t ptid,std::size_t ptrange,std::size_t _index>
struct GetPropIndices<Obj,ptid,ptrange,_index,
      typename std::enable_if<GenTempl::HasWithinRange<ptid,ptrange,typename IthPropPIT<Obj,_index>::Values>::value && (_index<(std::tuple_size<decltype(ObjInfo<Obj>::info)>::value-1)),void>::type>
{
   typedef typename GenTempl::AppendValue<typename GetPropIndices<Obj,ptid,ptrange,_index+1>::indices,_index>::type indices;
   typedef typename GenTempl::AppendValue<typename GetPropIndices<Obj,ptid,ptrange,_index+1>::values,GenTempl::HasWithinRange<ptid,ptrange,typename IthPropPIT<Obj,_index>::Values>::vinrange-ptid+1>::type values;
};

template<class Obj,std::size_t ptid,std::size_t ptrange,std::size_t _index>
struct GetPropIndices<Obj,ptid,ptrange,_index,
      typename std::enable_if<!GenTempl::HasWithinRange<ptid,ptrange,typename IthPropPIT<Obj,_index>::Values>::value && (_index<(std::tuple_size<decltype(ObjInfo<Obj>::info)>::value-1)),void>::type>
   :public GetPropIndices<Obj,ptid,ptrange,_index+1>{};

template<class Obj,std::size_t ptid,std::size_t ptrange,std::size_t _index>
struct GetPropIndices<Obj,ptid,ptrange,_index,
      typename std::enable_if<GenTempl::HasWithinRange<ptid,ptrange,typename IthPropPIT<Obj,_index>::Values>::value && (_index==(std::tuple_size<decltype(ObjInfo<Obj>::info)>::value-1)),void>::type>
{
   typedef GenTempl::Values<_index> indices;
   typedef GenTempl::Values<GenTempl::HasWithinRange<ptid,ptrange,typename IthPropPIT<Obj,_index>::Values>::vinrange> values;
};

template<class Obj,std::size_t ptid,std::size_t ptrange,std::size_t _index>
struct GetPropIndices<Obj,ptid,ptrange,_index,
      typename std::enable_if<!GenTempl::HasWithinRange<ptid,ptrange,typename IthPropPIT<Obj,_index>::Values>::value && (_index==(std::tuple_size<decltype(ObjInfo<Obj>::info)>::value-1)),void>::type>
{
   typedef GenTempl::Values<> indices;
   typedef GenTempl::Values<> values;
};

template<class Obj,std::size_t ptid,std::size_t ptrange>
struct HavePropIndices
{
   static constexpr bool value=GenTempl::Length<typename GetPropIndices<Obj,ptid,ptrange>::indices>::value>0;
};


template<class Obj,class Values>
struct GetPropIndicesByValues{};

template<class Obj,std::size_t... values>
struct GetPropIndicesByValues<Obj,GenTempl::Values<values...>>
{
   typedef GenTempl::Values<GetPropIndex<Obj,values>::value...> type;
};

//interface classes

template<class Obj,std::size_t Features>
struct GetIntValuesT
      :public GetIntValuesTImpl<typename ObjInfoTT<Obj>::type,Features,std::tuple<> >{};

template<class Obj,std::size_t Features>
struct GetValuesIndices
      :public GetValuesIndicesImpl<typename ObjInfoTT<Obj>::type,Features,GenTempl::Indices<> >{};

template<class Obj,std::size_t Features,class Conn>
struct GetSpecialT
      : public GetSpecialTImpl<typename ObjInfoTT<Obj>::type,Features,Conn,Obj,std::tuple<> >{};

template<class Obj,std::size_t Features,class Conn>
struct GetSpecialIndices
      : public GetSpecialIndicesImpl<typename ObjInfoTT<Obj>::type,Features,Conn,Obj,GenTempl::Indices<> >{};

template<class Obj,std::size_t Features,class Conn>
struct GetSpecialEditorIndices
      : public GetSpecialEditorIndicesImpl<typename ObjInfoTT<Obj>::type,Features,Conn,Obj,GenTempl::Indices<> >{};

template<class Obj,std::size_t Features,class Conn>
struct GetSpecialEditorT
      : public GetSpecialEditorTImpl<typename ObjInfoTT<Obj>::type,Features,Conn,Obj,std::tuple<> >{};

template<std::size_t ind,class Obj,std::size_t Features>
typename std::enable_if<HaveSuffixes<IthPropType<Obj,ind>,Features>::value,std::string>::type CreateColumnListForType()
{
   std::string ret=std::string(std::get<ind>(ObjInfo<Obj>::info).ColumnName)+TypeInfo<IthPropType<Obj,ind>,Features>::Suffixes[0];
   for(std::size_t i=1;i<std::tuple_size<typename TypeInfo<IthPropType<Obj,ind>,Features>::IntType>::value;i++)
      ret+=std::string(",")+std::get<ind>(ObjInfo<Obj>::info).ColumnName+TypeInfo<IthPropType<Obj,ind>,Features>::Suffixes[i];
   return ret;
}

template<std::size_t ind,class Obj,std::size_t Features>
typename std::enable_if<!HaveSuffixes<IthPropType<Obj,ind>,Features>::value,std::string>::type CreateColumnListForType()
{
   return std::string(std::get<ind>(ObjInfo<Obj>::info).ColumnName);
}

template<class Inds,class Obj,std::size_t Features>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,std::string>::type CreateColumnList()
{
   return std::string();
}

template<class Inds,class Obj,std::size_t Features>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,std::string>::type CreateColumnList()
{
   std::string ret=CreateColumnList<typename GenTempl::LastIndices<Inds>::type,Obj,Features>();
   return ret.size()?(CreateColumnListForType<GenTempl::FirstInd<Inds>::value,Obj,Features>()+","+ret):
                     CreateColumnListForType<GenTempl::FirstInd<Inds>::value,Obj,Features>();
}

template<class Inds,class Obj,std::size_t Features,class Conn,std::size_t index,class Values>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,void>::type MoveValues(Obj* pObj,Values& Val,Connection<Conn,Features>* pConn)
{
   TypeInfo<IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::template MoveValue<Conn,Values,index>(std::get<GenTempl::FirstInd<Inds>::value>(ObjInfo<Obj>::info).Get(pObj),Val,pConn);
   MoveValues<typename GenTempl::LastIndices<Inds>::type,Features,Conn,index+std::tuple_size<typename TypeInfo<IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::IntType>::value,Values>(pObj,Val,pConn);
}

template<class Inds,class Obj,std::size_t Features,class Conn,std::size_t index,class Values>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,void>::type MoveValues(Obj*,Values&,Connection<Conn,Features>*)
{

}

//editor functionality

template<class Inds,class Obj,std::size_t Features>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,std::string>::type CreatePlaceholderList(std::size_t)
{
   return std::string();
}

template<class Inds,class Obj,std::size_t Features>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,std::string>::type CreatePlaceholderList(std::size_t i)
{
   std::string ret=CreatePlaceholderList<typename GenTempl::LastIndices<Inds>::type,Obj,Features>(i+std::tuple_size<typename TypeInfo<IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::IntType>::value);
   std::string ph;
   ph="?"+std::to_string(i);
   for(std::size_t j=i+1;j<i+std::tuple_size<typename TypeInfo<IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::IntType>::value;j++)
      ph+=",?"+std::to_string(j);
   return ret.size()?(ph+","+ret):ph;
}

template<class Obj,std::size_t index,std::size_t Features>
typename std::enable_if<HaveSuffixes<IthPropType<Obj,index>,Features>::value,std::string>::type CreateUpdateListElem(std::size_t i)
{
   std::string ret=std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+TypeInfo<IthPropType<Obj,index>,Features>::Suffixes[0]+std::string("=?")+std::to_string(i);
   for(std::size_t j=1;j<std::tuple_size<typename TypeInfo<IthPropType<Obj,index>,Features>::IntType>::value;j++)
      ret+=std::string(",")+std::get<index>(ObjInfo<Obj>::info).ColumnName+TypeInfo<IthPropType<Obj,index>,Features>::Suffixes[j]+std::string("=?")+std::to_string(i+j);
   return ret;
}

template<class Obj,std::size_t index,std::size_t Features>
typename std::enable_if<!HaveSuffixes<IthPropType<Obj,index>,Features>::value,std::string>::type CreateUpdateListElem(std::size_t i)
{
   return std::string(std::get<index>(ObjInfo<Obj>::info).ColumnName)+"=?"+std::to_string(i);
}

template<class Inds,class Obj,std::size_t Features>
typename std::enable_if<!GenTempl::HaveIndices<Inds>::value,std::string>::type CreateUpdateList(std::size_t)
{
   return std::string();
}

template<class Inds,class Obj,std::size_t Features>
typename std::enable_if<GenTempl::HaveIndices<Inds>::value,std::string>::type CreateUpdateList(std::size_t i)
{
   std::string ret=CreateUpdateList<typename GenTempl::LastIndices<Inds>::type,Obj,Features>(i+std::tuple_size<typename TypeInfo<IthPropType<Obj,GenTempl::FirstInd<Inds>::value>,Features>::IntType>::value);
   std::string us=CreateUpdateListElem<Obj,GenTempl::FirstInd<Inds>::value,Features>(i);
   return ret.size()?(us+","+ret):us;
}

}
}

#endif
