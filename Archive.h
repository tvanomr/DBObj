#ifndef __DBOBJ_ARCHIVE_H__
#define __DBOBJ_ARCHIVE_H__

#include "DBObj/ArchiveCoord.h"
#include "DBObj/ArchiveKey.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class Archive{};

template<class Obj,class Conn>
class Archive<Obj,Conn,0>
      :public ArchiveKey<Obj,Conn,0>, public ArchiveCoord<Obj,Conn,0>{};

}

#endif
