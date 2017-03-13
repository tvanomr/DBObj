#ifndef __GENERAL_TEMPLATES_HAVE_ANY_H__
#define __GENERAL_TEMPLATES_HAVE_ANY_H__

namespace GenTempl
{

struct HaveAny
{
   typedef char (&yes)[2];
   typedef char (&no)[3];
   template<class Type>
   struct Helper{};
};

}

#endif
