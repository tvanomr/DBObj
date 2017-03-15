#ifndef __DBOBJ_MAPLINK_H__
#define __DBOBJ_MAPLINK_H__

#include "DBObj/Object.h"
#include "DBObj/ObjInfo.h"
#include <map>
#include "DBObj/ObjInfoUtilMapLink.h"

namespace DBObj
{

template<class Key,class Child,std::size_t index>
class ChildrenMap;

template<class Key,class Child>
class ChildrenMapLoaderBase
{
public:
   virtual Child* LoadOne(const Key&,std::size_t)=0;
   virtual void LoadAll(std::map<Key,Child*>&,std::size_t)=0;
   virtual ~ChildrenMapLoaderBase(){}
   template<std::size_t index>
   void Attach(ChildrenMap<Key,Child,index>& children);
};

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features,class Condition=void>
class ChildrenMapLoader
{

};

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features,class Condition=void>
class MapLinkLoader
{

};

template<class Key,class Child,std::size_t index=0>
class ChildrenMap
{
protected:
   friend class ChildrenMapLoaderBase<Key,Child>;
   template<class _Key,class _Parent,class _Child,std::size_t _index,class _Conn,std::size_t _Features>
   friend class ChildrenMapLoader;
	template<class _Key,class _Parent,class _Child,std::size_t _index>
	friend class MapLink;
	mutable std::map<Key,Child*> objects;
	mutable bool bLoaded;
   Object* pParent;
	ChildrenMapLoaderBase<Key,Child>* pLoader;
	void RemoveChild(const Key& key);
	bool AddChild(const Key& key,Child* pChild);
public:
	typedef typename std::map<Key,Child*>::const_iterator const_iterator;
   ChildrenMap(Object* pThis);
   Child* operator [] (const Key& key) const;
	std::size_t size() const;
	const_iterator begin() const;
	const_iterator end() const;
	bool FindChild(const Child* pChild,Key& key) const;
	bool Loaded() const;
	void Load() const;
};

template<class Key,class Parent,class Child,std::size_t index>
class MapLink
{
protected:
	mutable Parent* pParent;
	mutable bool bLoaded;
	Child* pChild;

   template<class _Key,class _Parent,class _Child,std::size_t _index,class _Conn,std::size_t _Features>
   friend class MapLinkLoader;

   ChildrenMap<Key,Child,index>& GetMap(Parent* pParent) const
	{
      return std::get<TypeManip::GetChildrenMapIndex<typename std::remove_cv<decltype(ObjInfo<Parent>::info)>::type,Key,Child,index>::value>(ObjInfo<Parent>::info).Get(pParent);
	}
public:
	MapLink(Child* pThis);
	//all change functions will do nothing if MapLink can't be inserted into new position
	//all change dunctions are slow
	bool SetPK(Parent* pParentPtr,const Key& Value);
	bool SetParent(Parent* pParentPtr);
	bool SetKey(const Key& Value);
	Parent* operator -> ();
	Parent* Ptr() const;
	bool IsValid() const;
	std::size_t GetID() const;
   Key GetKey() const;
	void Load();
	Child* GetThis() const;
};

template<class Key,class Child>
template<std::size_t index>
void ChildrenMapLoaderBase<Key,Child>::Attach(ChildrenMap<Key,Child,index>& children)
{
   children.pLoader=this;
   children.bLoaded=false;
}

template<class Key,class Child,std::size_t index>
ChildrenMap<Key,Child,index>::ChildrenMap(Object* pThis):bLoaded(false),pParent(pThis),pLoader(nullptr){}

template<class Key,class Child,std::size_t index>
Child* ChildrenMap<Key,Child,index>::operator [] (const Key& key) const
{
	auto it=objects.find(key);
	if(it!=objects.end())
		return it->second;
	else
      if(pLoader&&!bLoaded)
		{
         Child* pObj=pLoader->LoadOne(key,pParent->GetID());
			if(pObj)
				objects.insert(std::make_pair(key,pObj));
			return pObj;
		}
	return nullptr;
}

template<class Key,class Child,std::size_t index>
std::size_t ChildrenMap<Key,Child,index>::size() const
{
	Load();
	return objects.size();
}

template<class Key,class Child,std::size_t index>
auto ChildrenMap<Key,Child,index>::begin() const ->const_iterator
{
	Load();
	return objects.begin();
}

template<class Key,class Child,std::size_t index>
auto ChildrenMap<Key,Child,index>::end() const ->const_iterator
{
	Load();
	return objects.end();
}

template<class Key,class Child,std::size_t index>
void ChildrenMap<Key,Child,index>::RemoveChild(const Key& key)
{
	Load();
	auto it=objects.find(key);
	if(it!=objects.end())
		objects.erase(it);
}

template<class Key,class Child,std::size_t index>
bool ChildrenMap<Key, Child, index>::AddChild(const Key& key, Child* pChild)
{
	Load();
	auto it=objects.find(key);
	if(it==objects.end())
	{
		objects.insert(std::make_pair(key,pChild));
		return true;
	}
	return false;
}

template<class Key,class Child,std::size_t index>
bool ChildrenMap<Key,Child,index>::FindChild(const Child* pChild,Key& key) const
{
	Load();
	for(auto it=objects.begin();it!=objects.end();++it)
		if(it->second==pChild)
		{
			key=it->first;
			return true;
		}
	return false;
}

template<class Key,class Child,std::size_t index>
bool ChildrenMap<Key,Child,index>::Loaded() const
{
	return bLoaded;
}

template<class Key,class Child,std::size_t index>
void ChildrenMap<Key,Child,index>::Load() const
{
	if(bLoaded)
		return;
	else
	{
		if(pLoader)
		{
			objects.clear();
         pLoader->LoadAll(objects,pParent->GetID());
		}
		bLoaded=true;
	}
}

template<class Key,class Parent,class Child,std::size_t index>
MapLink<Key,Parent,Child,index>::MapLink(Child* pThis):pParent(nullptr),bLoaded(false),pChild(pThis){}

template<class Key,class Parent,class Child,std::size_t index>
bool MapLink<Key,Parent,Child,index>::SetPK(Parent* pParentPtr,const Key& Value)
{
	if(pParentPtr)
		if(!GetMap(pParentPtr).AddChild(Value,pChild))
			return false;
	if(pParent)
	{
		if((pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load())))
		{
			Key key;
			if(GetMap(pParent).FindChild(pChild,key))
				GetMap(pParent).RemoveChild(key);
		}
	}
	pParent=pParentPtr;
	return true;
}

template<class Key,class Parent,class Child,std::size_t index>
bool MapLink<Key,Parent,Child,index>::SetParent(Parent* pParentPtr)
{
	if(pParent)
	{
		if((pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load())))
		{
			Key key;
			if(GetMap(pParent).FindChild(pChild,key))
         {
				if(pParentPtr)
					if(!GetMap(pParentPtr).AddChild(key,pChild))
						return false;
				GetMap(pParent).RemoveChild(key);
				pParent=pParentPtr;
				return true;
			}
		}
	}
	return false;
}

template<class Key,class Parent,class Child,std::size_t index>
bool MapLink<Key,Parent,Child,index>::SetKey(const Key& Value)
{
	if(pParent)
	{
		if((pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load())))
		{
			Key key;
			if(GetMap(pParent).FindChild(pChild,key))
			{
				if(GetMap(pParent).AddChild(Value,pChild))
				{
					GetMap(pParent).RemoveChild(key);
					return true;
				}
			}
		}
	}
	return false;
}

template<class Key,class Parent,class Child,std::size_t index>
Parent* MapLink<Key,Parent,Child,index>::operator -> ()
{
	if(pParent)
		return pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
	else
		return nullptr;
}

template<class Key,class Parent,class Child,std::size_t index>
Parent* MapLink<Key,Parent,Child,index>::Ptr() const
{
	if(pParent)
		return pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
	else
		return nullptr;
}

template<class Key,class Parent,class Child,std::size_t index>
bool MapLink<Key,Parent,Child,index>::IsValid() const
{
	if(pParent)
		return (pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load()));
	else
		return false;
}

template<class Key,class Parent,class Child,std::size_t index>
std::size_t MapLink<Key,Parent,Child,index>::GetID() const
{
	return pParent?reinterpret_cast<Object*>(pParent)->GetID():0;
}

template<class Key,class Parent,class Child,std::size_t index>
Key MapLink<Key,Parent,Child,index>::GetKey() const
{
   Key key=Key();
   if(IsValid())
   {
      GetMap(pParent).FindChild(pChild,key);
   }
   return key;
}

template<class Key,class Parent,class Child,std::size_t index>
void MapLink<Key,Parent,Child,index>::Load()
{
	if(pParent)
		pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
}

template<class Key,class Parent,class Child,std::size_t index>
Child* MapLink<Key,Parent,Child,index>::GetThis() const
{
	return pChild;
}

}

#endif
