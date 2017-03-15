#ifndef __DBOBJ_OBJECT_H__
#define __DBOBJ_OBJECT_H__

#include <memory>
#include "DBObj/TypeID.h"
#include "DBObj/ObjInfo.h"
#include "DB/Types.h"

namespace DBObj
{

class Object;

template<class Key,class Parent,class Child,std::size_t index=0>
class MapLink;

class ObjectData
{
public:
	virtual ~ObjectData(){}
};

class Object
{
	template<class Obj>
	friend class ObjPtr;
	template<class Parent,class Child,std::size_t index>
	friend class ObjLink;
   template<class Key,class Parent,class Child,std::size_t index>
   friend class MapLink;

	friend class YesIReallyWantToChangeObjectID;
protected:
	std::size_t id;
	std::unique_ptr<ObjectData> pData;
	virtual Object* Load(){return this;}
public:
	bool bEnabled;
   static constexpr std::size_t ObjTypeID=TypeID::TypeInvalid;
	static constexpr std::size_t ParentTypeID=TypeID::TypeInvalid;
	Object():id(0),bEnabled(false){}
	std::size_t GetID() const
	{
		return id;
	}
	virtual bool Descends(std::size_t TypeID) const
	{
		return TypeID==ObjTypeID;
	}
	virtual ~Object(){}
	virtual std::size_t GetTypeID() const
	{
		return ObjTypeID;
	}
	ObjectData* GetData()
	{
		return pData.get();
	}
	bool HaveData() const
	{
		return (bool) pData;
	}
	void SetData(ObjectData* pNewData)
	{
		pData.reset(pNewData);
	}
	void RemoveData()
	{
		pData.reset(nullptr);
	}
	ObjectData* ReleaseData()
	{
		return pData.release();
	}
};

class YesIReallyWantToChangeObjectID
{
public:
   void ChangeObjectID(Object* pObj,std::size_t NewID){pObj->id=NewID;}
};

template<class Obj>
class ObjPtr
{
	Object* pThis;
public:
	ObjPtr(Obj* ptr=nullptr);
	ObjPtr(ObjPtr<Obj>&& ptr);
	ObjPtr<Obj>& operator = (ObjPtr<Obj>&& ptr);
	ObjPtr<Obj>& operator = (Obj* ptr);
	operator Obj*();
	Obj* operator -> ();
	bool Load();
	bool IsValid() const;
	std::size_t GetID() const;
	template<class T>
	T* ToType();
	Obj* Ptr();
};

template<class Obj>
ObjPtr<Obj>::ObjPtr(Obj* ptr):pThis(ptr)
{

}

template<class Obj>
ObjPtr<Obj>::ObjPtr(ObjPtr<Obj>&& ptr):pThis(ptr.pThis)
{
	ptr.pThis=nullptr;
}

template<class Obj>
ObjPtr<Obj>& ObjPtr<Obj>::operator = (ObjPtr<Obj>&& ptr)
{
	pThis=ptr.pThis;
	ptr.pThis=nullptr;
	return *this;
}

template<class Obj>
ObjPtr<Obj>& ObjPtr<Obj>::operator = (Obj* ptr)
{
	pThis=ptr;
	return *this;
}

template<class Obj>
ObjPtr<Obj>::operator Obj*()
{
	return static_cast<Obj*>(pThis?(pThis=pThis->Load()):nullptr);
}

template<class Obj>
Obj* ObjPtr<Obj>::operator -> ()
{
	return static_cast<Obj*>(pThis?(pThis=pThis->Load()):nullptr);
}

template<class Obj>
bool ObjPtr<Obj>::Load()
{
	return pThis?(pThis=pThis->Load()):false;
}

template<class Obj>
bool ObjPtr<Obj>::IsValid() const
{
	return pThis;
}

template<class Obj>
std::size_t ObjPtr<Obj>::GetID() const
{
	return pThis?pThis->GetID():0;
}

template<class Obj>
template<class T>
T* ObjPtr<Obj>::ToType()
{
	if(pThis)
		if(pThis->Descends(T::ObjTypeID))
			return static_cast<T*>(pThis);
	return nullptr;
}

template<class Loader>
class CustomLoaderProp
{

};

}


#endif
