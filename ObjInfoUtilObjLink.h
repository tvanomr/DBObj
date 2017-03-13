#ifndef __DBOBJ_OBJINFO_UTIL_OBJ_LINK_H__
#define __DBOBJ_OBJINFO_UTIL_OBJ_LINK_H__

#include "Templates/Indices.h"

namespace DBObj
{

template<class Child,std::size_t index>
class Children;

template<class Parent,class Child,std::size_t index>
class ObjLink;


namespace TypeManip
{

template<class T,class Child,std::size_t ind,std::size_t i=0>
struct GetChildrenIndex
{

};

template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Child>
struct GetChildrenIndex<std::tuple<PropInfo<GetF,Children<Child,ind>,GenTempl::Values<ValueTypes...>>,PIs...>,Child,ind,i>
{
   static constexpr std::size_t value=i;
};

template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Child>
struct GetChildrenIndex<std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,Child,ind,i>
      :public GetChildrenIndex<std::tuple<PIs...>,Child,ind,i+1>
{

};

template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Child>
struct GetChildrenIndex<const std::tuple<PropInfo<GetF,Children<Child,ind>,GenTempl::Values<ValueTypes...>>,PIs...>,Child,ind,i>
{
   static constexpr std::size_t value=i;
};

template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Child>
struct GetChildrenIndex<const std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,Child,ind,i>
      :public GetChildrenIndex<std::tuple<PIs...>,Child,ind,i+1>
{

};


template<class T,class Parent,class Child,std::size_t ind,std::size_t i=0>
struct GetObjLinkIndex
{

};

template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Parent,class Child>
struct GetObjLinkIndex<std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,Parent,Child,ind,i>
      :public GetObjLinkIndex<std::tuple<PIs...>,Parent,Child,ind,i+1>
{

};

template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Parent,class Child>
struct GetObjLinkIndex<std::tuple<PropInfo<GetF,ObjLink<Parent,Child,ind>,GenTempl::Values<ValueTypes...>>,PIs...>,Parent,Child,ind,i>
{
   static constexpr std::size_t value=i;
};

template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Parent,class Child>
struct GetObjLinkIndex<const std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,Parent,Child,ind,i>
      :public GetObjLinkIndex<std::tuple<PIs...>,Parent,Child,ind,i+1>
{

};

template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Parent,class Child>
struct GetObjLinkIndex<const std::tuple<PropInfo<GetF,ObjLink<Parent,Child,ind>,GenTempl::Values<ValueTypes...>>,PIs...>,Parent,Child,ind,i>
{
   static constexpr std::size_t value=i;
};

}

}

#endif
