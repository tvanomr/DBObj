#ifndef __DBOBJ_OBJINFO_H__
#define __DBOBJ_OBJINFO_H__

#include "DB/ConnectionBase/Types.h"
#include "Templates/Indices.h"
#include <cstddef>
#include <type_traits>
#include <tuple>

namespace DBObj
{
struct ValueType
{
   enum : std::size_t
   {
      Ordinary=0,
      Parent=1,
      Ascend=2,
      Descend=3,
      Positive=5,
      Title=4, //this property is the object's title
      Key=10,
      KeyRange=10,
      Key2=11,
      Coord1=20,
      CoordRange=10,
      Coord2=21,
      Coord3=22
   };
   static constexpr size_t Masks[]={7,24};
   static constexpr std::size_t GetMask(std::size_t value,std::size_t index=0)
   {
      return (index<(sizeof(Masks)/sizeof(std::size_t)))?(((value&Masks[index])==value)?Masks[index]:GetMask(value,index+1)):0;
   }
   static constexpr bool HaveType(std::size_t type,std::size_t value,std::size_t index=0)
   {
      return (index<sizeof(Masks)/sizeof(std::size_t))?(((value&Masks[index])==type)?true:HaveType(type,value,index+1)):false;
   }
   static constexpr std::size_t NMasks()
   {
      return sizeof(Masks);
   }
};

template<class GetF,class Type,class ValInfo>
struct PropInfo
{
	typedef Type type;
	GetF Get;
	const char* ColumnName;
	std::size_t ColumnType;
#ifdef QMLDBOBJ_PROPS
   const char* PropName;
#endif
};

template<class Obj>
struct ObjInfo
{

};

template<std::size_t ObjTypeID>
struct GetObjTypeByID
{

};

namespace ObjInfoInternal
{

template<class Type,std::size_t ColType,class ValTypes>
struct TypeContainer{};

template<std::size_t i,class... Types>
struct IthType
{

};


#ifdef QMLDBOBJ_PROPS

template<class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,std::size_t NameSize,class... Types>
struct IthType<0,GetF,TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>,char [StrSize],char [NameSize],Types...>
{
   typedef PropInfo<GetF,Type,GenTempl::Values<ValTypes...>> type;
};

template<std::size_t i,class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,std::size_t NameSize,class... Types>
struct IthType<i,GetF,TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>,char [StrSize],char [NameSize],Types...> : public IthType<i-1,Types...>{};

template<std::size_t i,class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,std::size_t NameSize,class... Others>
constexpr typename std::enable_if<i!=0,typename IthType<i,GetF,TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>,char [StrSize],char [NameSize],Others...>::type>::type
   GetIth(const GetF&,const TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>> &,const char (&) [StrSize],const char (&) [NameSize],const Others&... values)
{
   return GetIth<i-1>(values...);
}

template<std::size_t i,class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,std::size_t NameSize,class... Others>
constexpr typename std::enable_if<i==0,PropInfo<GetF,Type,GenTempl::Values<ValTypes...>>>::type
   GetIth(const GetF& arg,const TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>&,const char (&str) [StrSize],const char (&name) [NameSize],const Others&...)
{
   return PropInfo<GetF,Type,GenTempl::Values<ValTypes...>>{arg,str,ColType,name};
}
#else
template<class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,class... Types>
struct IthType<0,GetF,TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>,char [StrSize],Types...>
{
   typedef PropInfo<GetF,Type,GenTempl::Values<ValTypes...>> type;
};

template<std::size_t i,class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,class... Types>
struct IthType<i,GetF,TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>,char [StrSize],Types...> : public IthType<i-1,Types...>{};

template<std::size_t i,class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,class... Others>
constexpr typename std::enable_if<i!=0,typename IthType<i,GetF,TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>,char [StrSize],Others...>::type>::type
   GetIth(const GetF&,const TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>> &,const char (&) [StrSize],const Others&... values)
{
   return GetIth<i-1>(values...);
}

template<std::size_t i,class GetF,class Type,std::size_t ColType,std::size_t... ValTypes,std::size_t StrSize,class... Others>
constexpr typename std::enable_if<i==0,PropInfo<GetF,Type,GenTempl::Values<ValTypes...>>>::type
   GetIth(const GetF& arg,const TypeContainer<Type,ColType,GenTempl::Values<ValTypes...>>&,const char (&str) [StrSize],const Others&...)
{
   return PropInfo<GetF,Type,GenTempl::Values<ValTypes...>>{arg,str,ColType};
}
#endif

template<std::size_t... inds,class... Params>
constexpr std::tuple<typename IthType<inds,Params...>::type...> CreatePropTupleInt(GenTempl::Indices<inds...>,const Params&... params)
{
	return std::make_tuple(GetIth<inds>(params...)...);
}

#ifdef QMLDBOBJ_PROPS
template<class... Params>
constexpr auto CreatePropTuple(const Params&... params)
   ->decltype(CreatePropTupleInt(typename GenTempl::GenIndices<sizeof...(Params)/4-1>::type(),params...))
{
   return CreatePropTupleInt(typename GenTempl::GenIndices<sizeof...(Params)/4-1>::type(),params...);
}
#else
template<class... Params>
constexpr auto CreatePropTuple(const Params&... params)
	->decltype(CreatePropTupleInt(typename GenTempl::GenIndices<sizeof...(Params)/3-1>::type(),params...))
{
	return CreatePropTupleInt(typename GenTempl::GenIndices<sizeof...(Params)/3-1>::type(),params...);
}
#endif

}

}

#define OBJ_INTERNAL_GET_11TH(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,...)   a11

#ifdef QMLDBOBJ_PROPS

#define OBJ_INTERNAL_PROP1(Prop)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,DB::TypeUnknown,GenTempl::Values<ValueType::Ordinary>>(),"none",#Prop

#define OBJ_INTERNAL_PROP2(Prop,Col)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,DB::TypeUnknown,GenTempl::Values<ValueType::Ordinary>>(),#Col,#Prop

#define OBJ_INTERNAL_PROP3(Prop,Col,PropName)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,DB::TypeUnknown,GenTempl::Values<ValueType::Ordinary>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP4(Prop,Col,PropName,ColType)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::Ordinary>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP5(Prop,Col,PropName,ColType,PropType)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP6(Prop,Col,PropName,ColType,PropType,PropType2)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP7(Prop,Col,PropName,ColType,PropType,PropType2,PropType3)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP8(Prop,Col,PropName,ColType,PropType,PropType2,PropType3,PropType4)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3,ValueType::PropType4>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP9(Prop,Col,PropName,ColType,PropType,PropType2,PropType3,PropType4,PropType5)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3,ValueType::PropType4,ValueType::PropType5>>(),#Col,#PropName

#define OBJ_INTERNAL_PROP10(Prop,Col,PropName,ColType,PropType,PropType2,PropType3,PropType4,PropType5,PropType6)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3,ValueType::PropType4,ValueType::PropType5,ValueType::PropType6>>(),#Col,#PropName


#else

#define OBJ_INTERNAL_PROP1(Prop)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,DB::TypeUnknown,GenTempl::Values<ValueType::Ordinary>>(),"none"

#define OBJ_INTERNAL_PROP2(Prop,Col)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,DB::TypeUnknown,GenTempl::Values<ValueType::Ordinary>>(),#Col

#define OBJ_INTERNAL_PROP3(Prop,Col,PropName)   OBJ_INTERNAL_PROP2(Prop,Col)

#define OBJ_INTERNAL_PROP4(Prop,Col,PropName,ColType)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::Ordinary>>(),#Col

#define OBJ_INTERNAL_PROP5(Prop,Col,PropName,ColType,PropType)\
      [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
      ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType>>(),#Col

#define OBJ_INTERNAL_PROP6(Prop,Col,PropName,ColType,PropType,PropType2) \
   [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2>>(),#Col

#define OBJ_INTERNAL_PROP7(Prop,Col,PropName,ColType,PropType,PropType2,PropType3) \
   [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3>>(),#Col

#define OBJ_INTERNAL_PROP8(Prop,Col,PropName,ColType,PropType,PropType2,PropType3,PropType4) \
   [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3,ValueType::PropType4>>(),#Col

#define OBJ_INTERNAL_PROP9(Prop,Col,PropName,ColType,PropType,PropType2,PropType3,PropType4,PropType5) \
   [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3,ValueType::PropType4,ValueType::PropType5>>(),#Col

#define OBJ_INTERNAL_PROP10(Prop,Col,PropName,ColType,PropType,PropType2,PropType3,PropType4,PropType5,PropType6) \
   [](type* pObj)->typename std::remove_cv<decltype(type::Prop)>::type& {return pObj->Prop;},\
   ObjInfoInternal::TypeContainer<typename std::remove_cv<decltype(type::Prop)>::type,ColType,GenTempl::Values<ValueType::PropType,ValueType::PropType2,ValueType::PropType3,ValueType::PropType4,ValueType::PropType5,ValueType::PropType6>>(),#Col
#endif

#define OBJ_PROP(Prop,...) ,OBJ_INTERNAL_GET_11TH(Prop,__VA_ARGS__,OBJ_INTERNAL_PROP10,OBJ_INTERNAL_PROP9,OBJ_INTERNAL_PROP8,OBJ_INTERNAL_PROP7,OBJ_INTERNAL_PROP6,OBJ_INTERNAL_PROP5,OBJ_INTERNAL_PROP4,OBJ_INTERNAL_PROP3,OBJ_INTERNAL_PROP2,OBJ_INTERNAL_PROP1)(Prop,__VA_ARGS__)

#ifdef QMLDBOBJ_PROPS
#define OBJ_START(Class,Table,ClName,Prop,...)\
   template<>\
   struct GetObjTypeByID<Class::ObjTypeID>\
   {\
      typedef Class type;\
   };\
   template<>\
   struct ObjInfo<Class>\
   {\
      typedef Class type;\
      static constexpr const char* TableName=#Table;\
      static constexpr const char* ClassName=#ClName;\
      static constexpr auto info=ObjInfoInternal::CreatePropTuple(\
      OBJ_INTERNAL_GET_11TH(Prop,__VA_ARGS__,OBJ_INTERNAL_PROP10,OBJ_INTERNAL_PROP9,OBJ_INTERNAL_PROP8,OBJ_INTERNAL_PROP7,OBJ_INTERNAL_PROP6,OBJ_INTERNAL_PROP5,OBJ_INTERNAL_PROP4,OBJ_INTERNAL_PROP3,OBJ_INTERNAL_PROP2,OBJ_INTERNAL_PROP1)(Prop,__VA_ARGS__)

#else
#define OBJ_START(Class,Table,ClassName,Prop,...)\
   template<>\
   struct GetObjTypeByID<Class::ObjTypeID>\
   {\
      typedef Class type;\
   };\
   template<>\
   struct ObjInfo<Class>\
   {\
      typedef Class type;\
      static constexpr const char* TableName=#Table;\
      static constexpr auto info=ObjInfoInternal::CreatePropTuple(\
      OBJ_INTERNAL_GET_11TH(Prop,__VA_ARGS__,OBJ_INTERNAL_PROP10,OBJ_INTERNAL_PROP9,OBJ_INTERNAL_PROP8,OBJ_INTERNAL_PROP7,OBJ_INTERNAL_PROP6,OBJ_INTERNAL_PROP5,OBJ_INTERNAL_PROP4,OBJ_INTERNAL_PROP3,OBJ_INTERNAL_PROP2,OBJ_INTERNAL_PROP1)(Prop,__VA_ARGS__)
#endif

#define OBJ_START_NOID(Class,Table,Prop,...)\
   template<>\
   struct ObjInfo<Class>\
   {\
      typedef Class type;\
      static constexpr const char* TableName=#Table;\
      static constexpr auto info=ObjInfoInternal::CreatePropTuple(\
      OBJ_INTERNAL_GET_11TH(Prop,__VA_ARGS__,OBJ_INTERNAL_PROP10,OBJ_INTERNAL_PROP9,OBJ_INTERNAL_PROP8,OBJ_INTERNAL_PROP7,OBJ_INTERNAL_PROP6,OBJ_INTERNAL_PROP5,OBJ_INTERNAL_PROP4,OBJ_INTERNAL_PROP3,OBJ_INTERNAL_PROP2,OBJ_INTERNAL_PROP1)(Prop,__VA_ARGS__)

#define OBJ_END(Class)\
		);\
	};\
	constexpr decltype(ObjInfo<Class>::info) ObjInfo<Class>::info;



#endif
