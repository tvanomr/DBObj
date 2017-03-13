#ifndef __DBOBJ_ARCHIVE_COORD_H__
#define __DBOBJ_ARCHIVE_COORD_H__

#include "DBObj/ArchiveBase.h"
#include "DBObj/SQLUtil.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class ArchiveCoord : public virtual ArchiveBase<Obj,Conn,Features>{};

template<class Obj,class Conn>
class ArchiveCoord<Obj,Conn,0,
      typename std::enable_if<TypeManip::HavePropIndices<Obj,ValueType::Coord1,ValueType::CoordRange>::value,void>::type>
      : public virtual ArchiveBase<Obj,Conn,0>
{
protected:
   typedef typename TypeManip::GetPropIndices<Obj,ValueType::Coord1,ValueType::CoordRange> CoordIndices;
   QueryStorage<Conn,0,GenTempl::Length<typename CoordIndices::values>::value> CoordQueries,DeleteQueries;
public:
   template<std::size_t... inds,class... Types>
   typename std::enable_if<GenTempl::IsUniqueSeq<inds...>::value &&
   GenTempl::HaveValues<typename CoordIndices::values,inds...>::value &&
   (sizeof...(Types)==2*sizeof...(inds)),void>::type
   LoadCoordStart(const Types&... ranges)
   {
      if(!CoordQueries.template HaveQuery<inds...>())
      {
         CoordQueries.template CreateSelectQuery<Obj,ValueType::Coord1,inds...>(*(this->pConn),this->values,this->GetSelectClause(),TypeManipSQL::GetOrderByClause<Obj>(),"ArchiveCoord::LoadCoord()");
      }
      this->pCurrentQ=CoordQueries.template ArgAndRun<inds...>(ranges...);
   }

   template<std::size_t... inds,class... Types>
   void LoadCoord(std::vector<Obj>& data,const Types&... ranges)
   {
      LoadCoordStart<inds...>(ranges...);
      this->ResultToVector(data);
   }

   template<std::size_t... inds,class... Types>
   typename std::enable_if<GenTempl::IsUniqueSeq<inds...>::value &&
   GenTempl::HaveValues<typename CoordIndices::values,inds...>::value &&
   (sizeof...(Types)==2*sizeof...(inds)),void>::type
   DeleteCoord(const Types&... ranges)
   {
      if(!DeleteQueries.template HaveQuery<inds...>())
      {
         DeleteQueries.template CreateDeleteQuery<Obj,ValueType::Coord1,inds...>(*this->pConn,"delete from "+std::string(ObjInfo<Obj>::TableName),std::string(),"ArchiveCoord::DeleteCoord()");
      }
      DeleteQueries.template ArgAndRun<inds...>(ranges...);
   }
};

}

#endif
