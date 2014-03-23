#pragma once

#pragma warning(disable:4996)

#include <boost/signals2.hpp>
#include <memory>
namespace boost
{
#if _CPPLIB_VER >= 520
	template<typename Visitor, typename R1, typename R2, typename B1>
	inline void visit_each( Visitor& v, const std::tr1::_Bind<R1, R2, B1>& t )
	{
		static_assert( false, "Don't use std::bind, Please use boost::bind!" );
	}
#endif
}
namespace signals = boost::signals2;

