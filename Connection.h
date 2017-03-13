#ifndef __DBOBJ_CONNECTION_H__
#define __DBOBJ_CONNECTION_H__

#include "DBObj/Object.h"
#include "DBObj/Loader.h"
#include <map>

namespace DBObj
{
template<class Conn,std::size_t Features>
class Connection
{

};

template<class Conn>
class Connection<Conn,0> : public Conn
{
protected:

	template<class Parent,class Child,std::size_t index,class _Conn,std::size_t _Features>
	friend class ObjLinkLoader;
   template<class _Key,class _Parent,class _Child,std::size_t _index,class _Conn,std::size_t _Features>
   friend class MapLinkLoader;

	typedef std::vector<ObjLoaderBase<Conn,0>*> LoadersVec;

	template<std::size_t LastTypeID>
	LoadersVec FillLoaders()
	{
		return FillLoadersFillers<Conn,0,ObjTypePresent<LastTypeID>::value>::template FillLoaders<LastTypeID>(this);
	}

	class FirstObject: public Object
	{
	protected:
		Connection<Conn,0>* pConn;
		std::size_t ObjTypeID;
		Object* Load();
	public:
		FirstObject(std::size_t ObjID,std::size_t ObjTypeIDValue,Connection<Conn,0>* pConnection):
			pConn(pConnection),ObjTypeID(ObjTypeIDValue)
		{
			id=ObjID;
		}
		std::size_t GetObjTypeID() const
		{
			return ObjTypeID;
		}
	};

	std::vector<ObjLoaderBase<Conn,0>*> Loaders;
	YesIReallyWantToChangeObjectID ChangeID;
	typename Conn::DBQuery LoadObj;
	std::size_t ObjTypeID;
	bool bEnabled;
	std::map<std::size_t,Object*> Objects;
	template<class Obj>
	Obj* GetTempPtr(std::size_t ObjID);
public:
	template<class Type,class bPtr=void>
	struct TypeFilter
	{
		typedef Type type;
		static Type Move(Type&& value)
		{
			return value;
		}
	};
	Connection();
	~Connection();
	ObjLoaderBase<Conn,0>* GetObjLoader(std::size_t TypeIDValue);
	bool FillObj(std::size_t TypeIDValue,Object* pObj);
	Object* GetObject(std::size_t ObjID);
	template<class Obj>
	Obj* GetObject(std::size_t ObjID);
	template<class Obj>
	ObjPtr<Obj> GetObjPtr(std::size_t ObjID);
	void AssignNewID(Object* pObj);

   //slow (no delayed loading) but noone cares - not instantiated unless used since it's template function
   template<class Obj>
   void LoadAllObjectsOfType(std::vector<Obj*>& objects);

   //internal functions, do not use unless you really want to
   void RemoveObjectPtr(Object* pObj);
   Object* NewObject(std::size_t ObjTypeID);
};

template<class Conn>
Object* Connection<Conn,0>::FirstObject::Load()
{
	Object* pObj=pConn->GetObject(id);
   if(pObj)
   {
      if(!pObj->Descends(ObjTypeID))
      {
         printd7("invalid type, %u,%u\n",ObjTypeID,pObj->GetTypeID());
         delete pObj;
         pObj=nullptr;
      }
   }
	delete this;
	return pObj;
}

template<class Conn>
Connection<Conn,0>::Connection():Loaders(std::move(FillLoaders<TypeEnd>()))
{
	LoadObj=this->Query("select f_type,f_enabled from tbl_object where f_guid=?1","Connection::GetObject()");
	LoadObj.oarg(ObjTypeID,bEnabled);
}

template<class Conn>
Connection<Conn,0>::~Connection()
{
	for(auto ptr:Loaders)
		if(ptr)
			delete ptr;
	for(auto& obj:Objects)
		delete obj.second;
}

template<class Conn>
ObjLoaderBase<Conn,0>* Connection<Conn,0>::GetObjLoader(std::size_t TypeIDValue)
{
	return (TypeIDValue<=Loaders.size()&&TypeIDValue>0)?Loaders[TypeIDValue-1]:nullptr;
}

template<class Conn>
bool Connection<Conn,0>::FillObj(std::size_t TypeIDValue,Object* pObj)
{
	ObjLoaderBase<Conn,0>* pLoader;
	return (pLoader=GetObjLoader(TypeIDValue))?pLoader->Fill(pObj):false;
}

template<class Conn>
template<class Obj>
Obj* Connection<Conn,0>::GetTempPtr(std::size_t ObjID)
{
	auto it=Objects.find(ObjID);
	if(it!=Objects.end())
		return static_cast<Obj*>(it->second->Descends(Obj::ObjTypeID)?it->second:nullptr);
	return reinterpret_cast<Obj*>(new FirstObject(ObjID,Obj::ObjTypeID,this));
}

template<class Conn>
template<class Obj>
ObjPtr<Obj> Connection<Conn,0>::GetObjPtr(std::size_t ObjID)
{
	return ObjPtr<Obj>(GetTempPtr<Obj>(ObjID));
}

template<class Conn>
Object* Connection<Conn,0>::GetObject(std::size_t ObjID)
{
	auto it=Objects.find(ObjID);
	if(it!=Objects.end())
		return it->second;
	ObjLoaderBase<Conn,0>* pLoader;
	LoadObj.arg(ObjID);
	LoadObj.exec();
	if(LoadObj.next())
	{
#ifndef LOAD_DISABLED_OBJECTS
		if(bEnabled)
#endif
		{
			pLoader=GetObjLoader(ObjTypeID);
			if(pLoader)
			{
				Object* pObj=pLoader->Create();
				if(pObj)
				{
					ChangeID.ChangeObjectID(pObj,ObjID);
					pObj->bEnabled=bEnabled;
					if(pLoader->Fill(pObj))
					{
						Objects.insert(std::make_pair(ObjID,pObj));
						return pObj;
					}
				}
			}
		}
	}
	printd7("Not Loading object %u\n",ObjID);
	return nullptr;
}

template<class Conn>
template<class Obj>
void Connection<Conn,0>::LoadAllObjectsOfType(std::vector<Obj*>& objects)
{
   typename Conn::DBQuery query=this->Query(std::string("select f_guid from ")+ObjInfo<Obj>::TableName,
                                            std::string("Connection::LoadAllObjectsOfType()"));
   std::size_t id=0;
   Obj* pObj;
   query.oarg(id);
   objects.clear();
   query.exec();
   while(query.next())
   {
      pObj=GetObjPtr<Obj>(id);
      if(pObj)
         objects.push_back(pObj);
   }
}

template<class Conn>
template<class Obj>
Obj* Connection<Conn,0>::GetObject(std::size_t ObjID)
{
	Object* pObj=GetObject(ObjID);
	return static_cast<Obj*>(pObj->Descends(Obj::ObjTypeID)?pObj:nullptr);
}

template<class Conn>
void Connection<Conn,0>::RemoveObjectPtr(Object* pObj)
{
   if(pObj)
   {
      auto it=Objects.find(pObj->GetID());
      if(it!=Objects.end())
         if(it->second==pObj)
            Objects.erase(it);
   }
}

template<class Conn>
Object* Connection<Conn,0>::NewObject(std::size_t ObjTypeID)
{
   ObjLoaderBase<Conn,0>* pLoader=GetObjLoader(ObjTypeID);
   return pLoader?pLoader->Create():nullptr;
}

}

#endif
