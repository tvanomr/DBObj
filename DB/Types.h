#ifndef DBTYPES_H_
#define DBTYPES_H_

namespace DB
{
	enum Types
	{
		TypeUnknown=0,
		TypeString=1,
		TypeInteger=2,
		TypeSmallint=3,
		TypeDatetime=4,
		TypeDouble=5,
		TypeClob=6,
		TypeBlob=7
	};

   template<class T>
   struct GetColType
   {
      static constexpr Types value=TypeUnknown;
   };

   struct ColTypeIsString
   {
      static constexpr Types value=TypeString;
   };

   struct ColTypeIsSmallInt
   {
      static constexpr Types value=TypeSmallint;
   };

   struct ColTypeIsInteger
   {
      static constexpr Types value=TypeInteger;
   };

   struct ColTypeIsDatetime
   {
      static constexpr Types value=TypeDatetime;
   };

   struct ColTypeIsDouble
   {
      static constexpr Types value=TypeDouble;
   };

   struct ColTypeIsBlob
   {
      static constexpr Types value=TypeBlob;
   };

   template<>
   struct GetColType<bool>:public ColTypeIsSmallInt{};

   template<>
   struct GetColType<short int>:public ColTypeIsInteger{};

   template<>
   struct GetColType<int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<long int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<long long int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<unsigned short int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<unsigned int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<unsigned long int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<unsigned long long int>: public ColTypeIsInteger{};

   template<>
   struct GetColType<float>: public ColTypeIsDouble{};

   template<>
   struct GetColType<double>: public ColTypeIsDouble{};
}


#endif /* DBTYPES_H_ */
