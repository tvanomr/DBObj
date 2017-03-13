#ifndef __DBOBJ_FEATURES_H__
#define __DBOBJ_FEATURES_H__

#include <cstddef>

namespace DBObj
{

enum class Features : std::size_t
{
   Connections = 1,   //depends on connections with DBQuery objects and arg() function
   SQL         = 3    //SQL implementations always has Connections
};

constexpr bool HaveFeature(std::size_t Feat,Features Req)
{
   return (Feat & Req)!=0
}

}

#endif
