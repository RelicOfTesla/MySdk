#pragma once

template<typename R, typename T> inline
R union_cast( const T& v )
{
	union
	{
		T v;
		R r;
	} u;
	u.v = v;
	return u.r;
}