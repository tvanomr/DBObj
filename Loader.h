#ifndef __DBOBJ_LOADER_H__
#define __DBOBJ_LOADER_H__

#include "DBObj/ObjInfo.h"
#include "DBObj/Object.h"
#include <vector>
#include <string>
#include "Debug/XDebug.h"
#include "DBObj/ObjLink.h"
#include "DBObj/MapLink.h"
#include "DBObj/Types.h"
#include "DBObj/ObjInfoUtil.h"
#include "DBObj/ObjInfoSQLUtil.h"
//#include "DBObj/Connection.h"

namespace DBObj
{

template<class Obj,std::size_t Features>
class Connection;

template<class Conn,std::size_t Features,bool Present>
class FillLoadersFillers;

template <class Obj,class Conn,std::size_t Features>
class ObjLoader
{

};

template<class Conn,std::size_t Features>
class ObjLoaderBase
{
};



template<class Conn>
class ObjLoaderBase<Conn,0>
{
public:
	virtual bool Fill(Object*)=0;
	virtual Object* Create()=0;
	virtual ~ObjLoaderBase(){}
};

template <class Obj,class Conn>
class ObjLoader<Obj,Conn,0> : public ObjLoaderBase<Conn,0>
{
protected:

	friend class FillLoadersFillers<Conn,0,true>;

   typename TypeManip::GetIntValuesT<Obj,0>::type Values;
   typename TypeManip::GetSpecialT<Obj,0,Conn>::type Specials;
	Connection<Conn,0>* pConn;
   typename Connection<Conn,0>::DBQuery LoadQ;

   template<class Inds,std::size_t index>
   typename std::enable_if<(index<std::tuple_size<decltype(Specials)>::value),void>::type InitSpecials()
   {
      std::get<index>(Specials).template Init<GenTempl::FirstInd<Inds>::value>(pConn);
      InitSpecials<typename GenTempl::LastIndices<Inds>::type,index+1>();
   }

   template<class Inds,std::size_t index>
   typename std::enable_if<index==std::tuple_size<decltype(Specials)>::value,void>::type InitSpecials()
   {

   }

	static std::string LoadQueryStr()
	{
      return "select "+TypeManip::CreateColumnList<typename TypeManip::GetValuesIndices<Obj,0>::type,Obj,0>()+
				" from "+std::string(ObjInfo<Obj>::TableName)+" where f_guid=?1";
	}

	template<std::size_t size>
	typename std::enable_if<size!=0,void>::type InitQueryImpl()
	{
		LoadQ=pConn->Query(LoadQueryStr(),"Loader::Load()");
      TypeManipSQL::BindOargs<0,Conn,0>(LoadQ,Values);
	}

	template<std::size_t size>
	typename std::enable_if<size==0,void>::type InitQueryImpl()
	{

	}

	template<std::size_t size>
	typename std::enable_if<size!=0,bool>::type LoadValues(Object* pObj)
	{
		LoadQ.arg(pObj->GetID());
		LoadQ.exec();
		if(LoadQ.next())
		{
         TypeManipSQL::MoveValues<typename TypeManip::GetValuesIndices<Obj,0>::type,0>(static_cast<Obj*>(pObj),Values,pConn);
			return true;
		}
		return false;
	}

	template<std::size_t size>
	typename std::enable_if<size==0,bool>::type LoadValues(Object*)
	{
		return true;
	}

   template<class Inds,std::size_t index>
   typename std::enable_if<(index<std::tuple_size<decltype(Specials)>::value),void>::type InitObjSpecials(Obj* pObj)
   {
      std::get<index>(Specials).InitProp(pObj->GetID(),std::get<GenTempl::FirstInd<Inds>::value>(ObjInfo<Obj>::info).Get(pObj),pConn);
      InitObjSpecials<typename GenTempl::LastIndices<Inds>::type,index+1>(pObj);
   }

   template<class Inds,std::size_t index>
   typename std::enable_if<index==std::tuple_size<decltype(Specials)>::value,void>::type InitObjSpecials(Obj*)
   {

   }

   void InitQuery();
public:

	bool Fill(Object* pObj);
	Object* Create();
};

template<class Obj,class Conn>
void ObjLoader<Obj,Conn,0>::InitQuery()
{
   InitQueryImpl<std::tuple_size<decltype(Values)>::value>();
   InitSpecials<typename TypeManip::GetSpecialIndices<Obj,0,Conn>::type,0>();
}

template<class Obj,class Conn>
Object* ObjLoader<Obj,Conn,0>::Create()
{
	return new Obj;
}

template<class Obj,class Conn>
bool ObjLoader<Obj,Conn,0>::Fill(Object* pObj)
{
	if(Obj::ParentTypeID==TypeID::TypeInvalid||pConn->FillObj(Obj::ParentTypeID,pObj))
	{
      if(LoadValues<std::tuple_size<decltype(Values)>::value>(pObj))
		{
         InitObjSpecials<typename TypeManip::GetSpecialIndices<Obj,0,Conn>::type,0>(static_cast<Obj*>(pObj));
			return true;
		}
	}
	return false;
}

template<std::size_t ObjTypeID>
class ObjTypePresent
{
	typedef char (&yes)[1];
	typedef char (&no)[2];
	template<class T>
	struct Helper{};
	template<std::size_t id>
	static yes test(const GetObjTypeByID<id>*,Helper<typename GetObjTypeByID<id>::type>* =nullptr);
	static no test(...);
public:
	static constexpr bool value=(sizeof(yes)==sizeof(test(static_cast<GetObjTypeByID<ObjTypeID>*>(nullptr))));
};

template<class Conn,std::size_t Features,bool Present>
struct FillLoadersFillers
{

};

template<class Conn>
struct FillLoadersFillers<Conn,0,true>
{
	template<std::size_t LastTypeID>
	static typename std::enable_if<LastTypeID==TypeID::TypeInvalid,
	std::vector<ObjLoaderBase<Conn,0>*>>::type FillLoaders(Connection<Conn,0>*)
	{
		return std::vector<ObjLoaderBase<Conn,0>*>();
	}

	template<std::size_t LastTypeID>
	static typename std::enable_if<LastTypeID!=TypeID::TypeInvalid,
	std::vector<ObjLoaderBase<Conn,0>*>>::type FillLoaders(Connection<Conn,0>* pConn)
	{
		std::vector<ObjLoaderBase<Conn,0>*> lc=
				std::move(FillLoadersFillers<Conn,0,ObjTypePresent<LastTypeID-1>::value>::template FillLoaders<LastTypeID-1>(pConn));
		ObjLoader<typename GetObjTypeByID<LastTypeID>::type,Conn,0>* pLoader=
				new ObjLoader<typename GetObjTypeByID<LastTypeID>::type,Conn,0>();
		if(pLoader)
		{
			pLoader->pConn=pConn;
			pLoader->InitQuery();
		}
		lc.push_back(pLoader);
		printd7("Adding loader for type %u\n",LastTypeID);
		return lc;
	}
};

template<class Conn>
struct FillLoadersFillers<Conn,0,false>
{
	template<std::size_t LastTypeID>
	static typename std::enable_if<LastTypeID==TypeID::TypeInvalid,
	std::vector<ObjLoaderBase<Conn,0>*>>::type FillLoaders(Connection<Conn,0>*)
	{
		return std::vector<ObjLoaderBase<Conn,0>*>();
	}

	template<std::size_t LastTypeID>
	static typename std::enable_if<LastTypeID!=TypeID::TypeInvalid,
	std::vector<ObjLoaderBase<Conn,0>*>>::type FillLoaders(Connection<Conn,0>* pConn)
	{
		std::vector<ObjLoaderBase<Conn,0>*> lc=
				std::move(FillLoadersFillers<Conn,0,ObjTypePresent<LastTypeID-1>::value>::template FillLoaders<LastTypeID-1>(pConn));
		lc.push_back(nullptr);
		printd7("Not adding loader for type %u\n",LastTypeID);
		return lc;
	}
};


}

#endif
