#ifndef __DBOBJ_EDITOR_H__
#define __DBOBJ_EDITOR_H__

#include "DBObj/Features.h"
#include "DBObj/Connection.h"
#include "DB/ConnectionBase/ConnectionBase.h"

namespace DBObj
{

template<std::size_t Features>
class Editor;

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class ObjEditor
{

};

template<std::size_t Features,class Condition=void>
class ObjEditorBase
{

};

template<std::size_t Features,class Condition=void>
class ObjectEditorGlueBase
{

};

template<class Conn,std::size_t Features,class Condition=void>
class ObjectEditorGlue
{

};

template<std::size_t Features,class Condition=void>
class Editor
{

};

template<std::size_t Features>
class ObjEditorBase<Features,void>
{
public:
   virtual void Save(Object* pObj,Editor<Features>* pEd)=0;
   virtual void SaveNew(Object* pObj,Editor<Features>* pEd)=0;
   virtual void Delete(Object* pObj,Editor<Features>* pEd)=0;
   virtual void ClearTable()=0;
   virtual void CheckTable()=0;
   virtual void MarkDeletedChildren(Object* pObj,Editor<Features>* pEditor)=0;
   virtual void CutLinks(Object* pObj,Editor<Features>* pEditor)=0;
   virtual ~ObjEditorBase(){}
};


template<std::size_t Features>
class ObjectEditorGlueBase<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
{
protected:
   friend class Editor<Features>;
   virtual void SaveObject(Object* pObj)=0;
   virtual void SaveNewObject(Object* pObj)=0;
   virtual void DeleteObject(Object* pObj)=0;
   virtual std::size_t GetMaxID()=0;
   virtual void GetDeletedIDs(std::vector<std::size_t>& vec)=0;
   virtual void SaveDeletedIDs(const std::vector<std::size_t>& vec)=0;
   virtual Object* Create(std::size_t ObjTypeID)=0;
   virtual void CheckPrimaryTables()=0;
public:
   virtual ~ObjectEditorGlueBase(){}
};

template<class Conn,std::size_t Features>
class ObjectEditorGlue<Conn,Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>
   : public ObjectEditorGlueBase<Features>
{
protected:
   friend class Editor<Features>;
   Connection<Conn,Features>* pConn;
   typename Connection<Conn,Features>::DBQuery SaveObjectQ;
   typename Connection<Conn,Features>::DBQuery SaveNewObjectQ;
   typename Connection<Conn,Features>::DBQuery DeleteObjectQ;
   void SaveObject(Object *pObj) override;
   void SaveNewObject(Object *pObj) override;
   void DeleteObject(Object *pObj) override;
   std::size_t GetMaxID() override;
   void GetDeletedIDs(std::vector<std::size_t>& vec) override;
   void SaveDeletedIDs(const std::vector<std::size_t> &vec) override;
   Object* Create(std::size_t ObjTypeID) override;
   void CheckPrimaryTables() override;
   void InitQueries(Connection<Conn,Features>* pConnection);
public:
   ~ObjectEditorGlue(){}
};


template<std::size_t Features>
class Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>
{
protected:

   template<class Obj,class Conn,std::size_t Feat>
   friend class ObjEditor;

   struct EditorData : public ObjectData
   {
      bool bChanged=false;
      bool bToBeRemoved=false;
      bool bNew=false;
   };

   std::unique_ptr<ObjectEditorGlueBase<Features>> pGlue;
   std::vector<ObjEditorBase<Features>*> Editors;
   std::vector<Object*> ChangedObjs;
   std::vector<Object*> NewObjs;
   std::vector<std::size_t> DeletedIDs;

   void SaveOne(Object* pObj,std::size_t ObjTypeID);
   void SaveOneNew(Object* pObj,std::size_t ObjTypeID);
   void DeleteOne(Object* pObj,std::size_t ObjTypeID);

public:
   template<class Conn>
   void Init(Connection<Conn,Features>* pConnection);
   template<class Obj>
   Obj* NewObject();
   template<class Obj>
   bool NewObject(Obj*& pObj);
   void MarkChanged(Object* pObj);
   void MarkDeleted(Object* pObj);
   void MarkDeletedOne(Object* pObj);
   bool IsToBeDeleted(Object* pObj) const;
   bool IsChanged(Object* pObj);
   bool IsNew(Object* pObj);
   void Save();
   void CheckAllTables();
   void ClearTable(std::size_t ObjTypeID);
   template<class Obj>
   void ClearTable();
   void ClearAllTables();
   ~Editor();
};

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::InitQueries(Connection<Conn,0>* pConnection)
{
   pConn=pConnection;
   SaveObjectQ=pConn->Query("update tbl_object set f_type=?2,f_enabled=?3 where f_guid=?1",
                            "ObjectEditorGlue::SaveObject()");
   SaveNewObjectQ=pConn->Query("insert into tbl_object (f_guid,f_type,f_enabled) values (?1,?2,?3)",
                               "ObjectEditorGlue::SaveNewObject()");
   DeleteObjectQ=pConn->Query("delete from tbl_object where f_guid=?1",
                              "ObjectEditorGlue::DeleteObject()");
}

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::SaveObject(Object* pObj)
{
   SaveObjectQ.arg(pObj->GetID(),pObj->GetTypeID(),pObj->bEnabled);
   SaveObjectQ.exec();
}

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::SaveNewObject(Object* pObj)
{
   SaveNewObjectQ.arg(pObj->GetID(),pObj->GetTypeID(),pObj->bEnabled);
   SaveNewObjectQ.exec();
}

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::DeleteObject(Object* pObj)
{
   DeleteObjectQ.arg(pObj->GetID());
   DeleteObjectQ.exec();
   pConn->RemoveObjectPtr(pObj);
}

template<class Conn,std::size_t Features>
std::size_t ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::GetMaxID()
{
   typename Connection<Conn,Features>::DBQuery query=pConn->Query("select max(f_guid) from tbl_object",
                                             "ObjectEditorGlue::GetMaxID()");
   std::size_t ret=0;
   query.oarg(ret);
   query.exec();
   return query.next()?ret:0;
}

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::GetDeletedIDs(std::vector<std::size_t>& vec)
{
   typename Connection<Conn,Features>::DBQuery query=pConn->Query("select f_guid from tbl_deleted_ids",
                                             "ObjectEditorGlue::GetDeletedIDs()");
   std::size_t id;
   query.oarg(id);
   query.exec();
   vec.clear();
   while(query.next())
      vec.push_back(id);
}

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::SaveDeletedIDs(const std::vector<std::size_t>& vec)
{
   pConn->DirectExec("delete from tbl_deleted_ids","ObjectEditorGlue::SaveDeletedIDs()");
   typename Connection<Conn,Features>::DBQuery query=pConn->Query("insert into tbl_deleted_ids (f_guid) values (?1)",
                                             "ObjectEditorGlue::SaveDeletedIDs()");
   for(auto id:vec)
   {
      query.arg(id);
      query.exec();
   }
}

template<class Conn,std::size_t Features>
Object* ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::Create(std::size_t ObjTypeID)
{
   return pConn->NewObject(ObjTypeID);
}

template<class Conn,std::size_t Features>
void ObjectEditorGlue<Conn,Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::SQL),void>::type>::CheckPrimaryTables()
{
   DB::ConnectionBase::PLAINCOLUMNS cols={
      {std::string("f_guid"),DB::Types::TypeInteger},
      {std::string("f_type"),DB::Types::TypeInteger},
      {std::string("f_enabled"),DB::Types::TypeSmallint}
   };
   pConn->CheckTable("tbl_object",cols,{"f_guid"});
   pConn->CheckTable("tbl_deleted_ids",
   {{std::string("f_guid"),DB::Types::TypeInteger}},{"f_guid"});
}



template<class Obj,std::size_t Features>
Obj* Editor<Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::NewObject()
{
   Obj* pRet=pGlue?static_cast<Obj*>(pGlue->Create(Obj::ObjTypeID)):nullptr;
   if(pRet)
   {
      pRet->SetData(new EditorData);
      static_cast<EditorData*>(pRet->GetData())->bNew=true;
      pRet->bEnabled=true;
      NewObjs.push_back(pRet);
   }
   return pRet;
}

template<class Obj,std::size_t Features>
bool Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::NewObject(Obj*& pObj)
{
   return (pObj=NewObject<Obj>());
}


template<class Obj,std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::ClearTable()
{
   ClearTable(Obj::ObjTypeID);
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::SaveOne(Object* pObj,std::size_t ObjTypeID)
{
   if(pGlue)
   {
      if(ObjTypeID<=Editors.size())
      {
         if(ObjTypeID!=TypeID::TypeInvalid)
         {
            if(Editors[ObjTypeID-1])
               Editors[ObjTypeID-1]->Save(pObj,this);
         }
         else
            pGlue->SaveObject(pObj);
      }
   }
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::SaveOneNew(Object* pObj,std::size_t ObjTypeID)
{
   if(pGlue)
   {
      if(ObjTypeID<=Editors.size())
      {
         if(ObjTypeID!=TypeID::TypeInvalid)
         {
            if(Editors[ObjTypeID-1])
               Editors[ObjTypeID-1]->SaveNew(pObj,this);
         }
         else
            pGlue->SaveNewObject(pObj);
      }
   }
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::DeleteOne(Object* pObj,std::size_t ObjTypeID)
{
   if(pGlue)
   {
      if(ObjTypeID<=Editors.size())
      {
         if(ObjTypeID!=TypeID::TypeInvalid)
         {
            if(Editors[ObjTypeID-1])
               Editors[ObjTypeID-1]->Delete(pObj,this);
         }
         else
            pGlue->DeleteObject(pObj);
      }
   }
}

template<std::size_t Features>
Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::~Editor()
{
   for(auto ptr: Editors)
      if(ptr)
         delete ptr;
   for(auto ptr:NewObjs)
      if(ptr)
         delete ptr;
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::MarkChanged(Object* pObj)
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      if(!(static_cast<EditorData*>(pObj->GetData())->bChanged ||
           static_cast<EditorData*>(pObj->GetData())->bNew ||
           static_cast<EditorData*>(pObj->GetData())->bToBeRemoved))
      {
         static_cast<EditorData*>(pObj->GetData())->bChanged=true;
         ChangedObjs.push_back(pObj);
      }
   }
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::MarkDeleted(Object* pObj)
{
   MarkDeletedOne(pObj);
   if(pObj)
   {
      if(pObj->GetTypeID()<=Editors.size()&&pObj->GetTypeID()!=TypeInvalid)
         if(Editors[pObj->GetTypeID()-1])
            Editors[pObj->GetTypeID()-1]->MarkDeletedChildren(pObj,this);
   }
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::MarkDeletedOne(Object* pObj)
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      if(!static_cast<EditorData*>(pObj->GetData())->bToBeRemoved)
      {
         static_cast<EditorData*>(pObj->GetData())->bToBeRemoved=true;
         if(!(static_cast<EditorData*>(pObj->GetData())->bNew ||
              static_cast<EditorData*>(pObj->GetData())->bChanged))
            ChangedObjs.push_back(pObj);
      }
   }
}

template<std::size_t Features>
bool Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::IsToBeDeleted(Object* pObj) const
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      return static_cast<EditorData*>(pObj->GetData())->bToBeRemoved;
   }
   return false;
}

template<std::size_t Features>
bool Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::IsChanged(Object* pObj)
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      return static_cast<EditorData*>(pObj->GetData())->bChanged;
   }
   return false;
}

template<std::size_t Features>
bool Editor<Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::IsNew(Object* pObj)
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      return static_cast<EditorData*>(pObj->GetData())->bNew;
   }
   return false;
}

template<std::size_t Features>
void Editor<Features,
typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::Save()
{
   printd8("Saving\n");
   if(pGlue)
   {
      printd8("%u objects changed, %u new objects\n",ChangedObjs.size(),NewObjs.size());
      //remove links of all objects to be deleted
      for(Object* pObj:ChangedObjs)
         if(static_cast<EditorData*>(pObj->GetData())->bToBeRemoved)
            if(pObj->GetTypeID()<=Editors.size()&&pObj->GetTypeID()!=TypeID::TypeInvalid)
               Editors[pObj->GetTypeID()-1]->CutLinks(pObj,this);

      //new objects can also be marked for deletion
      for(Object* pObj:NewObjs)
         if(static_cast<EditorData*>(pObj->GetData())->bToBeRemoved)
            if(pObj->GetTypeID()<=Editors.size() &&
                  pObj->GetTypeID()!=TypeID::TypeInvalid &&
                  Editors[pObj->GetTypeID()-1])
               Editors[pObj->GetTypeID()-1]->CutLinks(pObj,this);

      //assing IDs to new objects;
      std::size_t MaxID=pGlue->GetMaxID();
      std::vector<std::size_t> FreeIDs;
      pGlue->GetDeletedIDs(FreeIDs);
      YesIReallyWantToChangeObjectID changer;
      for(Object* pObj:NewObjs)
         if(!static_cast<EditorData*>(pObj->GetData())->bToBeRemoved &&
               pObj->GetTypeID()<=Editors.size() &&
               pObj->GetTypeID()!=TypeID::TypeInvalid &&
               Editors[pObj->GetTypeID()-1])
         {
            if(FreeIDs.size())
            {
               changer.ChangeObjectID(pObj,FreeIDs.back());
               FreeIDs.pop_back();
            }
            else
               changer.ChangeObjectID(pObj,++MaxID);
         }

      //save free IDs in case something goes wrong later
      pGlue->SaveDeletedIDs(FreeIDs);

      //save all objects that are not to be deleted
      for(Object* pObj:ChangedObjs)
         if(!static_cast<EditorData*>(pObj->GetData())->bToBeRemoved &&
               pObj->GetTypeID()<=Editors.size() &&
               pObj->GetTypeID()!=TypeID::TypeInvalid &&
               Editors[pObj->GetTypeID()-1])
         {
            Editors[pObj->GetTypeID()-1]->Save(pObj,this);
            static_cast<EditorData*>(pObj->GetData())->bChanged=false;
         }

      for(Object* pObj:NewObjs)
      {
         if(!static_cast<EditorData*>(pObj->GetData())->bToBeRemoved &&
               pObj->GetTypeID()<=Editors.size() &&
               pObj->GetTypeID()!=TypeID::TypeInvalid &&
               Editors[pObj->GetTypeID()-1])
         {
            Editors[pObj->GetTypeID()-1]->SaveNew(pObj,this);
            static_cast<EditorData*>(pObj->GetData())->bNew=false;
            static_cast<EditorData*>(pObj->GetData())->bChanged=false;
         }
         else
         {
            printd8("Not saving new object because ");
            if(static_cast<EditorData*>(pObj->GetData())->bToBeRemoved)
               printd8("it is to be removed\n");
            else if(!(pObj->GetTypeID()<=Editors.size()))
               printd8("type id is out of range\n");
            else if(!(pObj->GetTypeID()!=TypeID::TypeInvalid))
               printd8("the type is invalid\n");
            else if(!Editors[pObj->GetTypeID()-1])
               printd8("there is no editor for this type of object\n");
         }
      }

      //delete objects that are to be deleted
      for(Object* pObj:ChangedObjs)
         if(static_cast<EditorData*>(pObj->GetData())->bToBeRemoved &&
               pObj->GetTypeID()<=Editors.size() &&
               pObj->GetTypeID()!=TypeID::TypeInvalid &&
               Editors[pObj->GetTypeID()-1])
         {
            Editors[pObj->GetTypeID()-1]->Delete(pObj,this);
            FreeIDs.push_back(pObj->GetID());
            delete pObj;
         }

      for(Object* pObj:NewObjs)
         if(static_cast<EditorData*>(pObj->GetData())->bToBeRemoved)
            delete pObj;

      //clear lists
      ChangedObjs.clear();
      NewObjs.clear();
      //save FreeIDs
      pGlue->SaveDeletedIDs(FreeIDs);
   }
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::CheckAllTables()
{
   for(auto pEd:Editors)
      if(pEd)
         pEd->CheckTable();
   if(pGlue)
      pGlue->CheckPrimaryTables();
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::ClearTable(std::size_t ObjTypeID)
{
   if(ObjTypeID!=TypeID::TypeInvalid &&
         ObjTypeID<=Editors.size() &&
         Editors[ObjTypeID-1])
      Editors[ObjTypeID-1]->ClearTable();
}

template<std::size_t Features>
void Editor<Features,
      typename std::enable_if<HaveFeature(Features,DBObj::Features::Connections),void>::type>::ClearAllTables()
{
   for(auto pEd:Editors)
      if(pEd)
         pEd->ClearTable();
}

}

#include "DBObj/ObjEditors.h"

#endif
