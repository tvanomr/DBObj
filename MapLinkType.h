#ifndef __DBOBJ_MAP_LINK_TYPE_H__
#define __DBOBJ_MAP_LINK_TYPE_H__

#include "DBObj/MapLink.h"
#include "DBObj/Types.h"
#include "DBObj/ObjInfoUtil.h"

namespace DBObj
{

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
class ChildrenMapLoader<Key,Parent,Child,index,Conn,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
   : public ChildrenMapLoaderBase<Key,Child>
{
protected:
   template<class T,std::size_t ind=0>
   struct GetLinkInd
   {

   };

   template<class GetF,class Type,std::size_t... ValueTypes,class... PIs,std::size_t ind>
   struct GetLinkInd<std::tuple<PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>,PIs...>,ind>
         :public GetLinkInd<std::tuple<PIs...>,ind+1>
   {

   };

   template<class GetF,std::size_t... ValueTypes,class... PIs,std::size_t ind>
   struct GetLinkInd<std::tuple<PropInfo<GetF,MapLink<Key,Parent,Child,index>,GenTempl::Values<ValueTypes...>>,PIs...>,ind>
   {
      static constexpr std::size_t value=ind;
   };

   typename Connection<Conn,Features>::DBQuery LoadOneQ;
   typename Connection<Conn,Features>::DBQuery LoadAllQ;
   std::size_t id;
   Key key;
   Connection<Conn,Features>* pConn;
public:
   void InitQueries(Connection<Conn,Features>* pConnection);
   Child* LoadOne(const Key&,std::size_t) override;
   void LoadAll(std::map<Key,Child*>&,std::size_t) override;
};

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
void ChildrenMapLoader<Key,Parent,Child,index,Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
::InitQueries(Connection<Conn,Features>* pConnection)
{
   pConn=pConnection;
   LoadOneQ=pConn->Query(std::string("select f_guid from ")+ObjInfo<Child>::TableName+
                      std::string(" where ")+
                      std::get<GetLinkInd<typename std::remove_cv<decltype(ObjInfo<Child>::info)>::type>::value>(ObjInfo<Child>::info).ColumnName+
                      std::string("=?1 and ")+
                      std::get<GetLinkInd<typename std::remove_cv<decltype(ObjInfo<Child>::info)>::type>::value>(ObjInfo<Child>::info).ColumnName+
                      std::string("_key=?2"),"ChildrenMapLoader::LoadOne()");
   LoadOneQ.oarg(id);
   LoadAllQ=pConn->Query(std::string("select f_guid,")+
                         std::get<GetLinkInd<typename std::remove_cv<decltype(ObjInfo<Child>::info)>::type>::value>(ObjInfo<Child>::info).ColumnName+
                         std::string("_key from ")+ObjInfo<Child>::TableName+std::string(" where ")+
                         std::get<GetLinkInd<typename std::remove_cv<decltype(ObjInfo<Child>::info)>::type>::value>(ObjInfo<Child>::info).ColumnName+
                         std::string("=?1"),"ChildrenMapLoader::LoadAll()");
   LoadAllQ.oarg(id,key);
}

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
Child* ChildrenMapLoader<Key,Parent,Child,index,Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
::LoadOne(const Key & Value,std::size_t ParentID)
{
   LoadOneQ.arg(ParentID,Value);
   LoadOneQ.exec();
   if(LoadOneQ.next())
      return pConn->template GetObject<Child>(id);
   return nullptr;
}

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
void ChildrenMapLoader<Key,Parent,Child,index,Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
::LoadAll(std::map<Key, Child *>& children, std::size_t ParentID)
{
   children.clear();
   LoadAllQ.arg(ParentID);
   LoadAllQ.exec();
   while(LoadAllQ.next())
   {
      Child* pChild=pConn->template GetObject<Child>(id);
      if(pChild)
         children.insert(std::make_pair(key,pChild));
   }
}

template<class Key,class Parent,class Child,std::size_t index,class Conn,std::size_t Features>
class MapLinkLoader<Key,Parent,Child,index,Conn,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
public:
   static void SetParentPtr(MapLink<Key,Parent,Child,index>& link,std::size_t id,Connection<Conn,Features>* pConn)
   {
      link.pParent=pConn->template GetTempPtr<Parent>(id);
   }
};

namespace TypeManip
{

template<class Key,class Parent,class Child,std::size_t index,std::size_t Features>
struct TypeInfo<MapLink<Key,Parent,Child,index>,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>
{
   typedef std::tuple<Key,std::size_t> IntType;
   static constexpr const char* Suffixes[2]={"_key",""};
   template<class Conn,class Values,std::size_t _index>
   static void MoveValue(MapLink<Key,Parent,Child,index>& value,Values& values,Connection<Conn,Features>* pConn)
   {
      MapLinkLoader<Key,Parent,Child,index,Conn,Features>::SetParentPtr(value,std::get<_index+1>(values),pConn);
   }
   template<class Conn>
   static void Arg(const MapLink<Key,Parent,Child,index>& value,typename Connection<Conn,Features>::DBQuery& query)
   {
      query.arg(value.GetKey(),value.GetID());
   }
};

template<class Key,class Parent,class Child,std::size_t index,std::size_t Features>
constexpr const char* TypeInfo<MapLink<Key,Parent,Child,index>,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::Suffixes[2];

template<class Key,class Child,std::size_t index,std::size_t Features>
struct TypeInfo<ChildrenMap<Key,Child,index>,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
   template<class Conn,class Parent>
   struct Special
   {
      ChildrenMapLoader<Key,Parent,Child,index,Conn,Features> Loader;
      template<std::size_t ind>
      void Init(Connection<Conn,Features>* pConn)
      {
         Loader.InitQueries(pConn);
      }
      void InitProp(std::size_t,ChildrenMap<Key,Child,index>& prop,Connection<Conn,Features>*)
      {
         Loader.Attach(prop);
      }
   };
};

}

}

#endif
