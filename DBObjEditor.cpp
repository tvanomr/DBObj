#include "DBObj/Editor.h"

namespace DBObj
{
void Editor<0>::SaveOne(Object* pObj,std::size_t ObjTypeID)
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

void Editor<0>::SaveOneNew(Object* pObj,std::size_t ObjTypeID)
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

void Editor<0>::DeleteOne(Object* pObj,std::size_t ObjTypeID)
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

Editor<0>::~Editor()
{
   for(auto ptr: Editors)
      if(ptr)
         delete ptr;
   for(auto ptr:NewObjs)
      if(ptr)
         delete ptr;
}

void Editor<0>::MarkChanged(Object* pObj)
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

void Editor<0>::MarkDeleted(Object* pObj)
{
   MarkDeletedOne(pObj);
   if(pObj)
   {
      if(pObj->GetTypeID()<=Editors.size()&&pObj->GetTypeID()!=TypeInvalid)
         if(Editors[pObj->GetTypeID()-1])
            Editors[pObj->GetTypeID()-1]->MarkDeletedChildren(pObj,this);
   }
}

void Editor<0>::MarkDeletedOne(Object* pObj)
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

bool Editor<0>::IsToBeDeleted(Object* pObj) const
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      return static_cast<EditorData*>(pObj->GetData())->bToBeRemoved;
   }
   return false;
}

bool Editor<0>::IsChanged(Object* pObj)
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      return static_cast<EditorData*>(pObj->GetData())->bChanged;
   }
   return false;
}

bool Editor<0>::IsNew(Object* pObj)
{
   if(pObj)
   {
      if(!pObj->HaveData())
         pObj->SetData(new EditorData);
      return static_cast<EditorData*>(pObj->GetData())->bNew;
   }
   return false;
}

void Editor<0>::Save()
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

void Editor<0>::CheckAllTables()
{
   for(auto pEd:Editors)
      if(pEd)
         pEd->CheckTable();
   if(pGlue)
      pGlue->CheckPrimaryTables();
}

void Editor<0>::ClearTable(std::size_t ObjTypeID)
{
   if(ObjTypeID!=TypeID::TypeInvalid &&
         ObjTypeID<=Editors.size() &&
         Editors[ObjTypeID-1])
      Editors[ObjTypeID-1]->ClearTable();
}

void Editor<0>::ClearAllTables()
{
   for(auto pEd:Editors)
      if(pEd)
         pEd->ClearTable();
}

}
