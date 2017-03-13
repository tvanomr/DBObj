#ifndef __GENERAL_TEMPLATES_FUNCTIONS_H__
#define __GENERAL_TEMPLATES_FUNCTIONS_H__

#include <vector>

namespace GenTempl
{

template<class T,class... Ts>
std::vector<T> Vectorize(const T& value1,const Ts&... values);

template<class T>
void VectorizeImpl(std::vector<T>& vec,std::size_t i){}

template<class T,class... Ts>
void VectorizeImpl(std::vector<T> &vec, std::size_t i,const T& value,const Ts&... values)
{
   vec[i++]=value;
   VectorizeImpl(vec,i,values...);
}

template<class T,class... Ts>
std::vector<T> Vectorize(const T& value1,const Ts&... values)
{
   std::vector<T> ret(sizeof...(values)+1);
   ret[0]=value1;
   VectorizeImpl(ret,1,value2,values...);
   return ret;
}

}

#endif
