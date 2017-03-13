#ifndef __GENERAL_TEMPLATES_INDICES_H__
#define __GENERAL_TEMPLATES_INDICES_H__

#include <cstdint>
#include <type_traits>

namespace GenTempl
{

template<class T>
struct Length{};

template<class T,std::size_t index=0>
struct Get{};


//return sequence tail, skip first nskip numbers
template<class T,std::size_t nskip=0,class Cond=void>
struct Tail{};

//add values as vectors or add one value to all values in vector T1
template<class T1,class T2>
struct Add{};

//substract a value or vector, beware of underflows
template<class T1,class T2>
struct Sub{};


//invert sequence order in a vector
template<class T,std::size_t... TempSeq>
struct Revert{};

template<std::size_t... values>
struct Values
{

};

template<std::size_t... values>
struct Max{};

template<std::size_t maxv,std::size_t... values>
struct LimitedMax{};

template<>
struct Max<>
{
   static constexpr std::size_t value=0;
};

template<std::size_t _value,std::size_t... values>
struct Max<_value,values...>
{
   static constexpr std::size_t value=(_value>Max<values...>::value)?_value:Max<values...>::value;
};

template<std::size_t maxv>
struct LimitedMax<maxv>: public Max<>{};

template<std::size_t maxv,std::size_t _value,std::size_t... values>
struct LimitedMax<maxv,_value,values...>
{
   static constexpr std::size_t value=(_value>LimitedMax<maxv,values...>::value)?((_value<maxv)?_value:LimitedMax<maxv,values...>::value):LimitedMax<maxv,values...>::value;
};

template<std::size_t... values>
struct Length<Values<values...>>
{
   static constexpr std::size_t value=sizeof...(values);
};

template<std::size_t _value,std::size_t... values>
struct Get<Values<_value,values...>,0>
{
   static constexpr std::size_t value=_value;
};

template<std::size_t _value,std::size_t... values,std::size_t index>
struct Get<Values<_value,values...>,index>
   :Get<Values<values...>,index-1>{};

template<std::size_t index,std::size_t... values>
struct Tail<Values<values...>,index,typename std::enable_if<index==0,void>::type>
{
   typedef Values<values...> type;
};

template<std::size_t _value,std::size_t... values,std::size_t nskip>
struct Tail<Values<_value,values...>,nskip,typename std::enable_if<nskip!=0,void>::type>: public Tail<Values<values...>,nskip-1>{};

template<class V1,class V2,std::size_t... inds>
struct AddVectorImpl{};

template<std::size_t... inds>
struct AddVectorImpl<Values<>,Values<>,inds...>
{
   typedef Values<inds...> type;
};

template<std::size_t ind1,std::size_t... inds1,std::size_t ind2,std::size_t... inds2,std::size_t... inds>
struct AddVectorImpl<Values<ind1,inds1...>,Values<ind2,inds2...>,inds...>:
      public AddVectorImpl<Values<inds1...>,Values<inds2...>,ind1+ind2,inds...>{};

template<std::size_t ind1,std::size_t ind12,std::size_t... inds1,std::size_t ind2,std::size_t ind22,std::size_t... inds2>
struct Add<Values<ind1,ind12,inds1...>,Values<ind2,ind22,inds2...>>
      : public AddVectorImpl<Values<ind1,ind12,inds1...>,Values<ind2,ind22,inds2...>>{};

template<class V1,std::size_t ind2,std::size_t... inds>
struct AddScalarImpl{};

template<std::size_t ind1,std::size_t ind2,std::size_t... inds>
struct AddScalarImpl<Values<ind1>,ind2,inds...>
{
   typedef Values<ind1+ind2,inds...> type;
};

template<std::size_t ind1,std::size_t... inds1,std::size_t ind2,std::size_t... inds>
struct AddScalarImpl<Values<ind1,inds1...>,ind2,inds...>:
      public AddScalarImpl<Values<inds1...>,ind2,ind1+ind2,inds...>{};

template<std::size_t ind1,std::size_t... inds1,std::size_t ind2>
struct Add<Values<ind1,inds1...>,Values<ind2>>
      : public AddScalarImpl<Values<ind1,inds1...>,ind2>{};

template<std::size_t ind>
struct Add<Values<>,Values<ind>>
{
   typedef Values<> type;
};

template<>
struct Add<Values<>,Values<>>
{
   typedef Values<> type;
};

template<class V1,class V2,std::size_t... inds>
struct SubVectorImpl{};

template<std::size_t... inds>
struct SubVectorImpl<Values<>,Values<>,inds...>
{
   typedef Values<inds...> type;
};

template<std::size_t ind1,std::size_t... inds1,std::size_t ind2,std::size_t... inds2,std::size_t... inds>
struct SubVectorImpl<Values<ind1,inds1...>,Values<ind2,inds2...>,inds...>:
      public SubVectorImpl<Values<inds1...>,Values<inds2...>,ind1-ind2,inds...>{};

template<std::size_t ind1,std::size_t ind12,std::size_t... inds1,std::size_t ind2,std::size_t ind22,std::size_t... inds2>
struct Sub<Values<ind1,ind12,inds1...>,Values<ind2,ind22,inds2...>>
      : public SubVectorImpl<Values<ind1,ind12,inds1...>,Values<ind2,ind22,inds2...>>{};

template<class V1,std::size_t ind2,std::size_t... inds>
struct SubScalarImpl{};

template<std::size_t ind1,std::size_t ind2,std::size_t... inds>
struct SubScalarImpl<Values<ind1>,ind2,inds...>
{
   typedef Values<ind1-ind2,inds...> type;
};

template<std::size_t ind1,std::size_t... inds1,std::size_t ind2,std::size_t... inds>
struct SubScalarImpl<Values<ind1,inds1...>,ind2,inds...>:
      public SubScalarImpl<Values<inds1...>,ind2,ind1-ind2,inds...>{};

template<std::size_t ind1,std::size_t... inds1,std::size_t ind2>
struct Sub<Values<ind1,inds1...>,Values<ind2>>
      : public SubScalarImpl<Values<ind1,inds1...>,ind2>{};

template<std::size_t ind>
struct Sub<Values<>,Values<ind>>
{
   typedef Values<> type;
};

template<>
struct Sub<Values<>,Values<>>
{
   typedef Values<> type;
};

template<std::size_t... TempSeq>
struct Revert<Values<>,TempSeq...>
{
   typedef Values<TempSeq...> type;
};

template<std::size_t value,std::size_t... values,std::size_t... TempSeq>
struct Revert<Values<value,values...>,TempSeq...>
      : public Revert<Values<values...>,value,TempSeq...>{};

template<class T,std::size_t index>
struct SortUniqueImpl{};

template<std::size_t value,std::size_t... values>
struct SortUnique: public SortUniqueImpl<Values<value,values...>,sizeof...(values)>{};

template<std::size_t value>
struct SortUnique<value>
{
   typedef Values<value> type;
   static constexpr std::size_t max=value;
};

template<class T>
struct SortUniqueValues{};

template<std::size_t value,std::size_t... values>
struct SortUniqueValues<Values<value,values...>>
      : public SortUnique<value,values...>{};

template<>
struct SortUniqueValues<Values<>>
{
   typedef Values<> type;
};


template<std::size_t value,class Vals>
struct HaveValue
{

};

template<std::size_t _value>
struct HaveValue<_value,Values<>>
{
   static constexpr bool value=false;
};

template<std::size_t _value,std::size_t... values>
struct HaveValue<_value,Values<_value,values...>>
{
   static constexpr bool value=true;
};

template<std::size_t _value,std::size_t value1,std::size_t... values>
struct HaveValue<_value,Values<value1,values...>>
      :public HaveValue<_value,Values<values...>>
{

};

template<class T,std::size_t... values>
struct HaveValues{};

template<std::size_t... valseq>
struct HaveValues<Values<valseq...>>
{
   static constexpr bool value=false;
};

template<std::size_t _value,std::size_t... values,std::size_t... valseq>
struct HaveValues<Values<valseq...>,_value,values...>
{
   static constexpr bool value=HaveValue<_value,Values<valseq...>>::value ||
   HaveValues<Values<valseq...>,values...>::value;
};

template<std::size_t... values>
struct IsUniqueSeq
{
   static constexpr bool value=true;
};

template<std::size_t _value,std::size_t... values>
struct IsUniqueSeq<_value,values...>
{
   static constexpr bool value=!HaveValue<_value,Values<values...>>::value &&
   IsUniqueSeq<values...>::value;
};

template<std::size_t min,std::size_t range,std::size_t... values>
struct IsWithinRange
{
   static constexpr bool value=true;
};

template<std::size_t min,std::size_t range,std::size_t _value,std::size_t... values>
struct IsWithinRange<min,range,_value,values...>
{
   static constexpr bool value=_value>=min && _value<(min+range) && IsWithinRange<min,range,values...>::value;
};

template<std::size_t min,std::size_t range,class T,class Condition=void>
struct HasWithinRange{};

template<std::size_t min,std::size_t range>
struct HasWithinRange<min,range,Values<>,void>
{
   static constexpr bool value=false;
};

template<std::size_t min,std::size_t range,std::size_t _value,std::size_t... values>
struct HasWithinRange<min,range,Values<_value,values...>,
      typename std::enable_if<(_value>=min && _value<(min+range)),void>::type>
{
   static constexpr bool value=true;
   static constexpr std::size_t vinrange=_value;
};

template<std::size_t min,std::size_t range,std::size_t _value,std::size_t... values>
struct HasWithinRange<min,range,Values<_value,values...>,
      typename std::enable_if<!(_value>=min && _value<(min+range)),void>::type>
   : public HasWithinRange<min,range,Values<values...>>{};

template<class T,std::size_t value>
struct AppendValue{};

template<std::size_t value,std::size_t... values>
struct AppendValue<Values<values...>,value>
{
   typedef Values<values..., value> type;
};

template<class T,std::size_t value>
struct RevAppendValue{};

template<std::size_t value,std::size_t... values>
struct RevAppendValue<Values<values...>,value>
{
   typedef Values<value,values...> type;
};

template<std::size_t value,std::size_t... values>
struct SortUniqueImpl<Values<value,values...>,0>
{
   typedef Values<Max<value,values...>::value> type;
};

template<std::size_t value,std::size_t... values,std::size_t index>
struct SortUniqueImpl<Values<value,values...>,index>
{
   typedef typename AppendValue<typename SortUniqueImpl<Values<value,values...>,index-1>::type,
   LimitedMax<Get<typename SortUniqueImpl<Values<value,values...>,index-1>::type,
   Length<typename SortUniqueImpl<Values<value,values...>,index-1>::type>::value-1>::value,value,values...>::value>::type type;
};

template<std::size_t... inds>
struct Indices : public Values<inds...>
{

};

template<std::size_t... inds>
struct Length<Indices<inds...>>
{
   static constexpr std::size_t value=sizeof...(inds);
};

template<std::size_t max,std::size_t... seq>
struct GenIndices: public GenIndices<max-1,max,seq...>
{

};

template<std::size_t... seq>
struct GenIndices<0,seq...>
{
	typedef Indices<0,seq...> type;
};

template<class T,std::size_t ninds=1>
struct HaveIndices
{

};

template<std::size_t... inds,std::size_t ninds>
struct HaveIndices<Indices<inds...>,ninds>
{
   static constexpr bool value=sizeof...(inds)>=ninds;
};

template<class T>
struct FirstInd
{

};

template<std::size_t ind,std::size_t... inds>
struct FirstInd<Indices<ind,inds...>>
{
	static constexpr std::size_t value=ind;
};

template<class T>
struct LastIndices
{

};

template<std::size_t ind,std::size_t... inds>
struct LastIndices<Indices<ind,inds...>>
{
	typedef Indices<inds...> type;
};

template<class T>
struct NumIndices
{

};

template<std::size_t... inds>
struct NumIndices<Indices<inds...>>
{
   static constexpr std::size_t value=sizeof...(inds);
};

template<class T,std::size_t start,std::size_t num>
struct AddIndices
{

};

template<std::size_t... inds,std::size_t start,std::size_t num>
struct AddIndices<Indices<inds...>,start,num>:
      public AddIndices<Indices<inds...,start>,start+1,num-1>
{

};

template<std::size_t... inds,std::size_t start>
struct AddIndices<Indices<inds...>,start,0>
{
   typedef Indices<inds...> type;
};

template<std::size_t v,class T1,class T2,class Condition=void>
struct FindIndByValue{};

template<std::size_t v,std::size_t ind,std::size_t... inds,std::size_t _value,std::size_t... values>
struct FindIndByValue<v,Indices<ind,inds...>,Values<_value,values...>,
      typename std::enable_if<_value==v,void>::type>
{
   static constexpr std::size_t value=ind;
};

template<std::size_t v,std::size_t ind,std::size_t... inds,std::size_t _value,std::size_t... values>
struct FindIndByValue<v,Indices<ind,inds...>,Values<_value,values...>,
      typename std::enable_if<_value!=v,void>::type>
      : public FindIndByValue<v,Indices<inds...>,Values<values...>>{};


template<class Inds,class Vals,std::size_t... values>
using FindIndsByValues=Indices<FindIndByValue<values,Inds,Vals>::value...>;

}

#endif
