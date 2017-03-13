#include "DBObj/Object.h"

namespace DBObj
{
void YesIReallyWantToChangeObjectID::ChangeObjectID(Object *pObj, std::size_t NewID)
{
   pObj->id=NewID;
}

}
