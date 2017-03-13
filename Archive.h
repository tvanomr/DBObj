#ifndef __DBOBJ_ARCHIVE_H__
#define __DBOBJ_ARCHIVE_H__

#include "DBObj/ArchiveCoord.h"
#include "DBObj/ArchiveKey.h"

namespace DBObj
{

template<class Obj,class Conn,std::size_t Features,class Condition=void>
class Archive : public ArchiveKey<Obj,Conn,Features>, public ArchiveCoord<Obj,Conn,Features>{};

}

#endif
