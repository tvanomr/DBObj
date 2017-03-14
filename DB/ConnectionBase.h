#ifndef DBCONNECTIONBASE_H_
#define DBCONNECTIONBASE_H_

#include "DB/ConnectionBase/Types.h"
#include <string>
#include <vector>

namespace DB
{
   class ConnectionBase
	{
	public:
		struct PlainColumn
		{
			std::string Name;
			Types type;
		};
		typedef std::vector<PlainColumn> PLAINCOLUMNS;
	};
}

#endif /* DBCONNECTIONBASE_H_ */
