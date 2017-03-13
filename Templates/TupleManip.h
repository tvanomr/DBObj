#ifndef __GENERAL_TEMPLATES_TUPLE_MANIPULATION_H__
#define __GENERAL_TEMPLATES_TUPLE_MANIPULATION_H__

#include <tuple>

namespace GenTempl
{

template<class Tuple>
struct FirstType{};

template<class Type,class... Types>
struct FirstType<std::tuple<Type,Types...>>
{
   typedef Type type;
};

template<class Tuple>
struct LastTypes{};

template<class Type,class... Types>
struct LastTypes<std::tuple<Type,Types...>>
{
   typedef std::tuple<Types...> type;
};

}

#endif
