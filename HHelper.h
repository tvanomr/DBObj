#ifndef __DBOBJ_HIERARCHY_HELPER_H__
#define __DBOBJ_HIERARCHY_HELPER_H__

#include "DBObj/Object.h"

namespace DBObj
{
   template <class Parent,std::size_t ObjectTypeID>
   class HHelper : public Parent
   {
   public:
      static constexpr std::size_t ObjTypeID=ObjectTypeID;
      static constexpr std::size_t ParentTypeID=Parent::ObjTypeID;
		std::size_t GetTypeID() const override;
		bool Descends(std::size_t TypeID) const override;
   };

   template<class Parent,std::size_t ObjectTypeID>
   std::size_t HHelper<Parent,ObjectTypeID>::GetTypeID() const
   {
      return ObjectTypeID;
   }

   template<class Parent,std::size_t ObjectTypeID>
   bool HHelper<Parent,ObjectTypeID>::Descends(std::size_t TypeIDValue) const
   {
      return (TypeIDValue==ObjectTypeID)?true:Parent::Descends(TypeIDValue);
   }
}

#endif
