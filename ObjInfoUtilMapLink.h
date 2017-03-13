#ifndef __DBOBJ_OBJINFO_UTIL_MAP_LINK_H__
#define __DBOBJ_OBJINFO_UTIL_MAP_LINK_H__

#include "Templates/Indices.h"

namespace DBObj
{

template<class Key,class Child,std::size_t index>
class ChildrenMap;


namespace TypeManip
{

template<class T,class Key,class Child,std::size_t ind,std::size_t i=0>
struct GetChildrenMapIndex
{

};

template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Key,class Child>
struct GetChildrenMapIndex<std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,Key,Child,ind,i>
      :public GetChildrenMapIndex<std::tuple<PIs...>,Key,Child,ind,i+1>
{

};

template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Key,class Child>
struct GetChildrenMapIndex<std::tuple<PropInfo<GetF,ChildrenMap<Key,Child,ind>,GenTempl::Values<ValueTypes...>>,PIs...>,Key,Child,ind,i>
{
   static constexpr std::size_t value=i;
};


template<class T,class Key,class Parent,class Child,std::size_t ind,std::size_t i=0>
struct GetMapLinkIndex
{

};

template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Key,class Parent,class Child>
struct GetMapLinkIndex<std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,Key,Parent,Child,ind,i>
      :public GetMapLinkIndex<std::tuple<PIs...>,Key,Parent,Child,ind,i+1>
{

};

template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind,std::size_t i,class Key,class Parent,class Child>
struct GetMapLinkIndex<std::tuple<PropInfo<GetF,MapLink<Key,Parent,Child,ind>,GenTempl::Values<ValueTypes...>>,PIs...>,Key,Parent,Child,ind,i>
{
   static constexpr std::size_t value=i;
};

}

}

#endif
