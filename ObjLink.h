#ifndef __DBOBJ_OBJLINK_H__
#define __DBOBJ_OBJLINK_H__

#include "DBObj/ObjInfo.h"
#include <vector>
#include "DBObj/ObjInfoUtilObjLink.h"
#include "DBObj/Object.h"

namespace DBObj
{

template<class Child,std::size_t index>
class Children;

template<class Child>
class ChildrenLoaderBase
{
public:
	virtual void LoadAll(std::vector<Child*>& vec,std::size_t id)=0;
	virtual ~ChildrenLoaderBase(){}
   template<std::size_t index>
   void Attach(Children<Child,index>&);
};


template <class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
class ObjLinkLoader
{

};

template<class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
class ChildrenLoader
{

};

template<class Child,std::size_t index=0>
class Children
{
protected:
   template<class _Parent,class _Child,std::size_t _index,class _Conn,std::size_t _Features>
   friend class ChildrenLoader;

   friend class ChildrenLoaderBase<Child>;
	mutable std::vector<Child*> objects;
	mutable bool bLoaded;
	ChildrenLoaderBase<Child>* pLoader;
   Object* pParent;
public:
	typedef typename std::vector<Child*>::const_iterator const_iterator;
   Children(Object* pThis);
	Child* operator[] (std::size_t i);
	const Child* operator[] (std::size_t i) const;
	std::size_t size() const;
	const_iterator begin() const;
	const_iterator end() const;
	void RemoveChild(Child* pChild);
	void AddChild(Child* pChild);
	bool Loaded() const;
	void Load() const;
};

template<class Parent,class Child,std::size_t index=0>
class ObjLink
{
protected:
	mutable Parent* pParent;
	Child* pChild;

	template<class _Parent,class _Child,std::size_t _index,class Conn,std::size_t Features>
	friend class ObjLinkLoader;
	
	Children<Child,index>& GetList(Parent * ptr)
	{
      return std::get<TypeManip::GetChildrenIndex<typename std::remove_cv<decltype(ObjInfo<Parent>::info)>::type,Child,index>::value>(ObjInfo<Parent>::info).Get(ptr);
	}
public:
	ObjLink(Child* pThis);
	void operator = (Parent* pNewParent);
	Parent* operator -> ();
	Parent* Ptr();
	bool IsValid() const;
	std::size_t GetID() const;
	void Load();
};

template<class Child>
template<std::size_t index>
void ChildrenLoaderBase<Child>::Attach(Children<Child,index>& children)
{
   children.pLoader=this;
   children.bLoaded=false;
}

template<class Child,std::size_t index>
Children<Child,index>::Children(Object* pThis):bLoaded(false),pLoader(nullptr),pParent(pThis)
{

}

template<class Child,std::size_t index>
Child* Children<Child,index>::operator [] (std::size_t i)
{
	Load();
	return (i<objects.size())?objects[i]:nullptr;
}

template<class Child,std::size_t index>
const Child* Children<Child,index>::operator [] (std::size_t i) const
{
	Load();
   return (i<objects.size())?objects[i]:nullptr;
}

template<class Child,std::size_t index>
std::size_t Children<Child,index>::size() const
{
	Load();
	return objects.size();
}

template<class Child,std::size_t index>
auto Children<Child,index>::begin() const ->const_iterator
{
	Load();
	return objects.begin();
}

template<class Child,std::size_t index>
auto Children<Child,index>::end() const ->const_iterator
{
	Load();
	return objects.end();
}

template<class Child,std::size_t index>
void Children<Child,index>::RemoveChild(Child* pChild)
{
	for(auto it=objects.begin();it!=objects.end();++it)
		if(*it==pChild)
      {
			objects.erase(it);
         break;
      }
}

template<class Child,std::size_t index>
void Children<Child,index>::AddChild(Child* pChild)
{
	for(auto it=objects.begin();it!=objects.end();it++)
		if(*it==pChild)
			return;
	objects.push_back(pChild);
}

template<class Child,std::size_t index>
bool Children<Child,index>::Loaded() const
{
	return bLoaded;
}

template<class Child,std::size_t index>
void Children<Child,index>::Load() const
{
	if(bLoaded)
		return;
	else
	{
		if(pLoader)
         pLoader->LoadAll(objects,pParent->GetID());
		bLoaded=true;
	}
}

template<class Parent,class Child,std::size_t index>
ObjLink<Parent,Child,index>::ObjLink(Child* pThis):pParent(nullptr),pChild(pThis)
{

}

template<class Parent,class Child,std::size_t index>
void ObjLink<Parent,Child,index>::operator = (Parent* pNewParent)
{
	if(pParent)
		if((pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load())))
			GetList(pParent).RemoveChild(pChild);
	if(pNewParent)
		GetList(pNewParent).AddChild(pChild);
	pParent=pNewParent;
}

template<class Parent,class Child,std::size_t index>
Parent* ObjLink<Parent,Child,index>::operator -> ()
{
	if(pParent)
		return pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
	else
		return nullptr;
}

template<class Parent,class Child,std::size_t index>
Parent* ObjLink<Parent,Child,index>::Ptr()
{
	if(pParent)
		return pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
	else
		return nullptr;
}

template<class Parent,class Child,std::size_t index>
bool ObjLink<Parent,Child,index>::IsValid() const
{
	if(pParent)
		return pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
	else
		return false;
}

template<class Parent,class Child,std::size_t index>
std::size_t ObjLink<Parent,Child,index>::GetID() const
{
	return pParent?reinterpret_cast<Object*>(pParent)->GetID():0;
}

template<class Parent,class Child,std::size_t index>
void ObjLink<Parent,Child,index>::Load()
{
	if(pParent)
		pParent=static_cast<Parent*>(reinterpret_cast<Object*>(pParent)->Load());
}
}

#endif
