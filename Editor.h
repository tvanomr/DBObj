#ifndef __DBOBJ_EDITOR_H__
#define __DBOBJ_EDITOR_H__

#include "DBObj/Connection.h"
#include "DB/ConnectionBase/ConnectionBase.h"

namespace DBObj
{

template<std::size_t Features>
class Editor;

template<std::size_t Features>
class ObjEditorBase
{

};

template<class Obj,class Conn,std::size_t Features>
class ObjEditor
{

};

template<>
class ObjEditorBase<0>
{
public:
   virtual void Save(Object* pObj,Editor<0>* pEd)=0;
   virtual void SaveNew(Object* pObj,Editor<0>* pEd)=0;
   virtual void Delete(Object* pObj,Editor<0>* pEd)=0;
   virtual void ClearTable()=0;
   virtual void CheckTable()=0;
   virtual void MarkDeletedChildren(Object* pObj,Editor<0>* pEditor)=0;
   virtual void CutLinks(Object* pObj,Editor<0>* pEditor)=0;
   virtual ~ObjEditorBase(){}
};

template<std::size_t Features>
class ObjectEditorGlueBase
{

};

template<>
class ObjectEditorGlueBase<0>
{
protected:
   friend class Editor<0>;
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
class ObjectEditorGlue
{

};

template<class Conn>
class ObjectEditorGlue<Conn,0> : public ObjectEditorGlueBase<0>
{
protected:
   friend class Editor<0>;
   Connection<Conn,0>* pConn;
   typename Conn::DBQuery SaveObjectQ;
   typename Conn::DBQuery SaveNewObjectQ;
   typename Conn::DBQuery DeleteObjectQ;
   void SaveObject(Object *pObj) override;
   void SaveNewObject(Object *pObj) override;
   void DeleteObject(Object *pObj) override;
   std::size_t GetMaxID() override;
   void GetDeletedIDs(std::vector<std::size_t>& vec) override;
   void SaveDeletedIDs(const std::vector<std::size_t> &vec) override;
   Object* Create(std::size_t ObjTypeID) override;
   void CheckPrimaryTables() override;
   void InitQueries(Connection<Conn,0>* pConnection);
public:
   ~ObjectEditorGlue(){}
};

template<class Conn>
void ObjectEditorGlue<Conn,0>::InitQueries(Connection<Conn,0>* pConnection)
{
   pConn=pConnection;
   SaveObjectQ=pConn->Query("update tbl_object set f_type=?2,f_enabled=?3 where f_guid=?1",
                            "ObjectEditorGlue::SaveObject()");
   SaveNewObjectQ=pConn->Query("insert into tbl_object (f_guid,f_type,f_enabled) values (?1,?2,?3)",
                               "ObjectEditorGlue::SaveNewObject()");
   DeleteObjectQ=pConn->Query("delete from tbl_object where f_guid=?1",
                              "ObjectEditorGlue::DeleteObject()");
}

template<class Conn>
void ObjectEditorGlue<Conn,0>::SaveObject(Object* pObj)
{
   SaveObjectQ.arg(pObj->GetID(),pObj->GetTypeID(),pObj->bEnabled);
   SaveObjectQ.exec();
}

template<class Conn>
void ObjectEditorGlue<Conn,0>::SaveNewObject(Object* pObj)
{
   SaveNewObjectQ.arg(pObj->GetID(),pObj->GetTypeID(),pObj->bEnabled);
   SaveNewObjectQ.exec();
}

template<class Conn>
void ObjectEditorGlue<Conn,0>::DeleteObject(Object* pObj)
{
   DeleteObjectQ.arg(pObj->GetID());
   DeleteObjectQ.exec();
   pConn->RemoveObjectPtr(pObj);
}

template<class Conn>
std::size_t ObjectEditorGlue<Conn,0>::GetMaxID()
{
   typename Conn::DBQuery query=pConn->Query("select max(f_guid) from tbl_object",
                                             "ObjectEditorGlue::GetMaxID()");
   std::size_t ret=0;
   query.oarg(ret);
   query.exec();
   return query.next()?ret:0;
}

template<class Conn>
void ObjectEditorGlue<Conn,0>::GetDeletedIDs(std::vector<std::size_t>& vec)
{
   typename Conn::DBQuery query=pConn->Query("select f_guid from tbl_deleted_ids",
                                             "ObjectEditorGlue::GetDeletedIDs()");
   std::size_t id;
   query.oarg(id);
   query.exec();
   vec.clear();
   while(query.next())
      vec.push_back(id);
}

template<class Conn>
void ObjectEditorGlue<Conn,0>::SaveDeletedIDs(const std::vector<std::size_t>& vec)
{
   pConn->DirectExec("delete from tbl_deleted_ids","ObjectEditorGlue::SaveDeletedIDs()");
   typename Conn::DBQuery query=pConn->Query("insert into tbl_deleted_ids (f_guid) values (?1)",
                                             "ObjectEditorGlue::SaveDeletedIDs()");
   for(auto id:vec)
   {
      query.arg(id);
      query.exec();
   }
}

template<class Conn>
Object* ObjectEditorGlue<Conn,0>::Create(std::size_t ObjTypeID)
{
   return pConn->NewObject(ObjTypeID);
}

template<class Conn>
void ObjectEditorGlue<Conn,0>::CheckPrimaryTables()
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

template<std::size_t Features>
class Editor
{

};

template<>
class Editor<0>
{
protected:

   template<class Obj,class Conn,std::size_t Feature>
   friend class ObjEditor;

   struct EditorData : public ObjectData
   {
      bool bChanged=false;
      bool bToBeRemoved=false;
      bool bNew=false;
   };

   std::unique_ptr<ObjectEditorGlueBase<0>> pGlue;
   std::vector<ObjEditorBase<0>*> Editors;
   std::vector<Object*> ChangedObjs;
   std::vector<Object*> NewObjs;
   std::vector<std::size_t> DeletedIDs;

   void SaveOne(Object* pObj,std::size_t ObjTypeID);
   void SaveOneNew(Object* pObj,std::size_t ObjTypeID);
   void DeleteOne(Object* pObj,std::size_t ObjTypeID);

public:
   template<class Conn>
   void Init(Connection<Conn,0>* pConnection);
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


template<class Obj>
Obj* Editor<0>::NewObject()
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

template<class Obj>
bool Editor<0>::NewObject(Obj*& pObj)
{
   return (pObj=NewObject<Obj>());
}


template<class Obj>
void Editor<0>::ClearTable()
{
   ClearTable(Obj::ObjTypeID);
}

}

#include "DBObj/ObjEditors.h"

#endif
