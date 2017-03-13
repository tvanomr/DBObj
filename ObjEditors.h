#ifndef __DBOBJ_OBJ_EDITORS_H__
#define __DBOBJ_OBJ_EDITORS_H__

#include "DBObj/ObjInfo.h"
#include "DBObj/Connection.h"
#include "DBObj/Object.h"
#include "DBObj/ObjLink.h"
#include "DBObj/MapLink.h"
#include "DBObj/Loader.h"
#include "DBObj/Editor.h"
#include "DBObj/ObjInfoUtil.h"
#include "DBObj/ObjInfoSQLUtil.h"
#include "DB/ConnectionBase/ConnectionBase.h"
#include "Templates/TupleManip.h"
#include <string>

namespace DBObj
{



template<class Obj,class Conn>
class ObjEditor<Obj,Conn,0>: public ObjEditorBase<0>
{
protected:
   template<bool val>
   friend class FillObjEditorsFillers;
   Connection<Conn,0>* pConn;
   typename Conn::DBQuery SaveQ;
   typename Conn::DBQuery SaveNewQ;
   typename Conn::DBQuery DeleteQ;
   typename TypeManip::GetSpecialEditorT<Obj,0,Conn>::type SpecialEds;

   template<std::size_t index,class Inds>
   typename std::enable_if<index!=std::tuple_size<decltype(SpecialEds)>::value,void>::type CallCheckTable()
   {
      std::get<index>(SpecialEds).template CheckTable<GenTempl::FirstInd<Inds>::value>(pConn);
      CallCheckTable<index+1,typename GenTempl::LastIndices<Inds>::type>();
   }

   template<std::size_t index,class Inds>
   typename std::enable_if<index==std::tuple_size<decltype(SpecialEds)>::value,void>::type CallCheckTable(){}


   template<std::size_t index,class Inds>
   typename std::enable_if<index!=std::tuple_size<decltype(SpecialEds)>::value,void>::type InitSpecialEds()
   {
      std::get<index>(SpecialEds).template Init<GenTempl::FirstInd<Inds>::value>(pConn);
      InitSpecialEds<index+1,typename GenTempl::LastIndices<Inds>::type>();
   }

   template<std::size_t index,class Inds>
   typename std::enable_if<index==std::tuple_size<decltype(SpecialEds)>::value,void>::type InitSpecialEds(){}

   template<std::size_t index,class Inds>
   typename std::enable_if<index!=std::tuple_size<decltype(SpecialEds)>::value,void>::type SaveSpecial(Obj* pObj)
   {
      std::get<index>(SpecialEds).Save(pObj->GetID(),std::get<GenTempl::FirstInd<Inds>::value>(ObjInfo<Obj>::info).Get(pObj),pConn);
      SaveSpecial<index+1,typename GenTempl::LastIndices<Inds>::type>(pObj);
   }

   template<std::size_t index,class Inds>
   typename std::enable_if<index==std::tuple_size<decltype(SpecialEds)>::value,void>::type SaveSpecial(Obj*){}

   template<std::size_t index>
   typename std::enable_if<index==std::tuple_size<decltype(SpecialEds)>::value,void>::type DeleteSpecial(Obj*){}

   template<std::size_t index>
   typename std::enable_if<index!=std::tuple_size<decltype(SpecialEds)>::value,void>::type DeleteSpecial(Obj* pObj)
   {
      std::get<index>(SpecialEds).Delete(pObj->GetID());
      DeleteSpecial<index+1>(pObj);
   }

   typedef typename TypeManip::GetValuesIndices<Obj,0>::type ValueIndices;
   static constexpr std::size_t NumProps=std::tuple_size<typename TypeManip::ObjInfoTT<Obj>::type>::value;

   // link manipulation implementation
   template<class GetF,class Type,std::size_t... ValueTypes>
   void MarkDeletedChildrenMarker(Obj*,const PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>&,Editor<0>*)
   {

   }

   template<class GetF,class Child,std::size_t index,std::size_t... ValueTypes>
   void MarkDeletedChildrenMarker(Obj* pObj,const PropInfo<GetF,Children<Child,index>,GenTempl::Values<ValueTypes...>>& pi,Editor<0>* pEditor)
   {
      auto it=pi.Get(pObj).begin();
      auto end=pi.Get(pObj).end();
      for(;it!=end;++it)
         pEditor->MarkDeleted(*it);
   }

   template<class GetF,class Key,class Child,std::size_t index,std::size_t... ValueTypes>
   void MarkDeletedChidlrenMarker(Obj* pObj,const PropInfo<GetF,ChildrenMap<Key,Child,index>,GenTempl::Values<ValueTypes...>>& pi,Editor<0>* pEditor)
   {
      auto it=pi.Get(pObj).begin();
      auto end=pi.Get(pObj).end();
      for(;it!=end;++it)
         pEditor->MarkDeleted(it->second);
   }

	template<std::size_t ind>
	typename std::enable_if<(ind<NumProps),void>::type MarkDeletedChildrenImpl(Obj* pObj,Editor<0>* pEditor)
	{
		MarkDeletedChildrenMarker(pObj,std::get<ind>(ObjInfo<Obj>::info),pEditor);
		MarkDeletedChildrenImpl<ind+1>(pObj,pEditor);
	}

	template<std::size_t ind>
	typename std::enable_if<(ind==NumProps),void>::type MarkDeletedChildrenImpl(Obj*,Editor<0>*)
	{

	}

   template<std::size_t size>
   typename std::enable_if<size!=0,void>::type MarkDeletedChidlrenInterface(Obj* pObj,Editor<0>* pEditor)
   {
		MarkDeletedChildrenImpl<0>(pObj,pEditor);
   }

   template<std::size_t size>
   typename std::enable_if<size==0,void>::type MarkDeletedChildrenInterface(Obj *pObj,Editor<0>* pEditor)
   {

   }

   template<class GetF,class Type,std::size_t... ValueTypes>
   void CutLinksCutter(Obj*,const PropInfo<GetF,Type,GenTempl::Values<ValueTypes...>>&,Editor<0>*)
   {

   }

   template<std::size_t ind,class GetF,class Parent,std::size_t index,std::size_t... ValueTypes>
   void CutLinksCutter(Obj* pObj,const PropInfo<GetF,ObjLink<Parent,Obj,index>,GenTempl::Values<ValueTypes...>>& pi,Editor<0>* pEditor)
   {
      pEditor->MarkChanged(pi.Get(pObj).Ptr());
      pi.Get(pObj)=nullptr;
   }

   template<std::size_t ind,class GetF,class Key,class Parent,std::size_t index,std::size_t... ValueTypes>
   void CutLinksCutter(Obj* pObj,const PropInfo<GetF,MapLink<Key,Parent,Obj,index>,GenTempl::Values<ValueTypes...>>& pi,Editor<0>* pEditor)
   {
      pEditor->MarkChanged(pi.Get(pObj).Ptr());
      pi.Get(pObj).SetParent(nullptr);
   }

	template<std::size_t ind>
	typename std::enable_if<(ind<NumProps),void>::type
	CutLinksImpl(Obj* pObj,Editor<0>* pEditor)
	{
		CutLinksCutter(pObj,std::get<ind>(ObjInfo<Obj>::info),pEditor);
		CutLinksImpl<ind+1>(pObj,pEditor);
	}

	template<std::size_t ind>
	typename std::enable_if<(ind==NumProps),void>::type
	CutLinksImpl(Obj*,Editor<0>*)
	{

	}

   template<std::size_t size>
   typename std::enable_if<size!=0,void>::type
   CutLinksInterface(Obj* pObj,Editor<0>* pEditor)
   {
		CutLinksImpl<0>(pObj,pEditor);
   }

   template<std::size_t size>
   typename std::enable_if<size==0,void>::type
   CutLinksInterface(Obj*,Editor<0>*)
   {

   }


public:
   void InitQueries(Connection<Conn,0>* pConnection);

   void Save(Object* pObj,Editor<0>* pEd) override;
   void SaveNew(Object* pObj,Editor<0>* pEd) override;
   void Delete(Object* pObj,Editor<0>* pEd) override;
   void ClearTable() override;
   void CheckTable() override;
   void MarkDeletedChildren(Object *pObj, Editor<0> *pEd) override;
   void CutLinks(Object *pObj, Editor<0> *pEd) override;
   ~ObjEditor(){}
};

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::InitQueries(Connection<Conn,0>* pConnection)
{
   pConn=pConnection;
   SaveQ=pConn->Query(std::string("update ")+ObjInfo<Obj>::TableName+" set "+
                      TypeManip::CreateUpdateList<ValueIndices,Obj,0>(2)+
                      " where f_guid=?1","ObjEditor::Save()");
   SaveNewQ=pConn->Query(std::string("insert into ")+ObjInfo<Obj>::TableName+std::string(" (f_guid")+
                         std::string((GenTempl::HaveIndices<ValueIndices>::value!=0)?",":"")+
                         TypeManip::CreateColumnList<ValueIndices,Obj,0>()+
                         std::string(") values (?1")+
                         std::string((GenTempl::HaveIndices<ValueIndices>::value!=0)?",":"")+
                         TypeManip::CreatePlaceholderList<ValueIndices,Obj,0>(2)+
                         ")","ObjEditor::SaveNew()");
   DeleteQ=pConn->Query(std::string("delete from ")+ObjInfo<Obj>::TableName+" where f_guid=?1","ObjEditor::Delete()");
   InitSpecialEds<0,typename TypeManip::GetSpecialEditorIndices<Obj,0,Conn>::type>();
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::Save(Object* pObj,Editor<0>* pEd)
{
   pEd->SaveOne(pObj,Obj::ParentTypeID);
   SaveQ.arg(pObj->GetID());
   TypeManipSQL::ArgAll<ValueIndices,Conn,0>(static_cast<Obj*>(pObj),SaveQ);
   SaveQ.exec();
   SaveSpecial<0,typename TypeManip::GetSpecialEditorIndices<Obj,0,Conn>::type>(static_cast<Obj*>(pObj));
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::SaveNew(Object* pObj,Editor<0>* pEd)
{
   pEd->SaveOneNew(pObj,Obj::ParentTypeID);
   SaveNewQ.arg(pObj->GetID());
   TypeManipSQL::ArgAll<ValueIndices,Conn,0>(static_cast<Obj*>(pObj),SaveNewQ);
   SaveNewQ.exec();
   SaveSpecial<0,typename TypeManip::GetSpecialEditorIndices<Obj,0,Conn>::type>(static_cast<Obj*>(pObj));
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::Delete(Object* pObj,Editor<0>* pEd)
{
   DeleteQ.arg(pObj->GetID());
   DeleteQ.exec();
   pEd->DeleteOne(pObj,Obj::ParentTypeID);
   DeleteSpecial<0>(static_cast<Obj*>(pObj));
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::ClearTable()
{
   pConn->DirectExec(std::string("delete from ")+ObjInfo<Obj>::TableName,"ObjEditor::ClearTable()");
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::CheckTable()
{
   DB::ConnectionBase::PLAINCOLUMNS cols;
   cols.push_back({std::string("f_guid"),DB::Types::TypeInteger});
   TypeManipSQL::GetColumnInfo<Obj,0,ValueIndices>(cols);
   pConn->CheckTable(ObjInfo<Obj>::TableName,cols,{"f_guid"});
   CallCheckTable<0,typename TypeManip::GetSpecialEditorIndices<Obj,0,Conn>::type>();
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::MarkDeletedChildren(Object* pObj,Editor<0>* pEditor)
{
   MarkDeletedChidlrenInterface<NumProps>(static_cast<Obj*>(pObj),pEditor);
}

template<class Obj,class Conn>
void ObjEditor<Obj,Conn,0>::CutLinks(Object* pObj,Editor<0>* pEditor)
{
   CutLinksInterface<NumProps>(static_cast<Obj*>(pObj),pEditor);
}

template<bool bPresent>
struct FillObjEditorsFillers
{

};

template<>
struct FillObjEditorsFillers<true>
{
   template<std::size_t LastTypeID,class Conn>
   static typename std::enable_if<LastTypeID==TypeID::TypeInvalid,
   std::vector<ObjEditorBase<0>*>>::type FillEditors(Connection<Conn,0>*)
   {
      return std::vector<ObjEditorBase<0>*>();
   }

   template<std::size_t LastTypeID,class Conn>
   static typename std::enable_if<LastTypeID!=TypeID::TypeInvalid,
   std::vector<ObjEditorBase<0>*>>::type FillEditors(Connection<Conn,0>* pConn)
   {
      std::vector<ObjEditorBase<0>*> ec=
            std::move(FillObjEditorsFillers<ObjTypePresent<LastTypeID-1>::value>::template FillEditors<LastTypeID-1,Conn>(pConn));
      ObjEditor<typename GetObjTypeByID<LastTypeID>::type,Conn,0>* pEditor=
            new ObjEditor<typename GetObjTypeByID<LastTypeID>::type,Conn,0>();
      if(pEditor)
         pEditor->InitQueries(pConn);
      ec.push_back(pEditor);
      printd7("Adding editor for type %u\n",LastTypeID);
      return ec;
   }
};

template<>
struct FillObjEditorsFillers<false>
{
   template<std::size_t LastTypeID,class Conn>
   static typename std::enable_if<LastTypeID==TypeID::TypeInvalid,
   std::vector<ObjEditorBase<0>*>>::type FillEditors(Connection<Conn,0>*)
   {
      return std::vector<ObjEditorBase<0>*>();
   }

   template<std::size_t LastTypeID,class Conn>
   static typename std::enable_if<LastTypeID!=TypeID::TypeInvalid,
   std::vector<ObjEditorBase<0>*>>::type FillEditors(Connection<Conn,0>* pConn)
   {
      std::vector<ObjEditorBase<0>*> ec=
            std::move(FillObjEditorsFillers<ObjTypePresent<LastTypeID-1>::value>::template FillEditors<LastTypeID-1,Conn>(pConn));
      ec.push_back(nullptr);
      printd7("Not adding editor for type %u\n",LastTypeID);
      return ec;
   }
};

template<class Conn>
void Editor<0>::Init(Connection<Conn,0>* pConnection)
{
   std::unique_ptr<ObjectEditorGlue<Conn,0>> pGluePtr(new ObjectEditorGlue<Conn,0>);
   pGluePtr->InitQueries(pConnection);
   pGlue.reset(pGluePtr.release());
   Editors=std::move(FillObjEditorsFillers<ObjTypePresent<1>::value>::template FillEditors<TypeID::TypeEnd,Conn>(pConnection));
}


}

#endif
