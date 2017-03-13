#ifndef __DBOBJ_OBJ_LINK_TYPE_H__
#define __DBOBJ_OBJ_LINK_TYPE_H__

#include "DBObj/Types.h"
#include "DBObj/ObjLink.h"
#include "DBObj/ObjInfoUtil.h"
#include <set>

namespace DBObj
{

template <class Parent,class Child,std::size_t index,class Conn>
class ObjLinkLoader<Parent,Child,index,Conn,0>
{
public:
   static void SetPtr(ObjLink<Parent,Child,index>& to,std::size_t id,Connection<Conn,0>* pConn)
   {
      to.pParent=pConn->template GetTempPtr<Parent>(id);
   }
};

template<class Parent,class Child,std::size_t index,class Conn>
class ChildrenLoader<Parent,Child,index,Conn,0> : public ChildrenLoaderBase<Child>
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
   struct GetLinkInd<std::tuple<PropInfo<GetF,ObjLink<Parent,Child,index>,GenTempl::Values<ValueTypes...>>,PIs...>,ind>
   {
      static constexpr std::size_t value=ind;
   };

   typename Conn::DBQuery LoadQ;
   std::size_t id;
   Connection<Conn,0>* pConn;
public:
   void InitQuery(Connection<Conn,0>* pConnection);
   void LoadAll(std::vector<Child *> &vec,std::size_t ParentID) override;
};

template<class Parent,class Child,std::size_t index,class Conn>
void ChildrenLoader<Parent,Child,index,Conn,0>::InitQuery(Connection<Conn,0>* pConnection)
{
   pConn=pConnection;
   LoadQ=pConn->Query(std::string("select f_guid from ")+ObjInfo<Child>::TableName+std::string(" where ")+
                      std::get<GetLinkInd<typename std::remove_cv<decltype(ObjInfo<Child>::info)>::type>::value>(ObjInfo<Child>::info).ColumnName+
                      std::string("=?1"),"ChildrenLoader::LoadAll()");
   LoadQ.oarg(id);
}

template<class Parent,class Child,std::size_t index,class Conn>
void ChildrenLoader<Parent,Child,index,Conn,0>::LoadAll(std::vector<Child*>& vec,std::size_t ParentID)
{
   Child* pChild;
   LoadQ.arg(ParentID);
   LoadQ.exec();
   std::set<std::size_t> inds;
   for(auto child: vec)
      inds.insert(child->GetID());
   while(LoadQ.next())
   {
      if(inds.find(id)==inds.end())
      {
         pChild=pConn->template GetObject<Child>(id);
         if(pChild && std::get<GetLinkInd<typename TypeManip::ObjInfoTT<Child>::type>::value>(ObjInfo<Child>::info).Get(pChild).GetID()==ParentID)
            vec.push_back(pChild);
      }
   }
}

namespace TypeManip
{

template<class Parent,class Child,std::size_t index>
struct TypeInfo<ObjLink<Parent,Child,index>,0,void>
{
   typedef std::tuple<std::size_t> IntType;
   template<class Conn,class Values,std::size_t _index>
   static void MoveValue(ObjLink<Parent,Child,index>& value,Values& values,Connection<Conn,0>* pConn)
   {
      ObjLinkLoader<Parent,Child,index,Conn,0>::SetPtr(value,std::get<_index>(values),pConn);
   }
   template<class Conn>
   static void Arg(const ObjLink<Parent,Child,index>& value,typename Connection<Conn,0>::DBQuery& query)
   {
      query.arg(value.GetID());
   }
};

template<class Child,std::size_t index>
struct TypeInfo<Children<Child,index>,0,void>
{
   template<class Conn,class Parent>
   struct Special
   {
      ChildrenLoader<Parent,Child,index,Conn,0> Loader;
      template<std::size_t ind>
      void Init(Connection<Conn,0>* pConn)
      {
         Loader.InitQuery(pConn);
      }
      void InitProp(std::size_t,Children<Child,index>& prop,Connection<Conn,0>*)
      {
         Loader.Attach(prop);
      }
   };
};

}

}

#endif
