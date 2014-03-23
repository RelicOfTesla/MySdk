#pragma once

#if HAS_BOOST
#include <boost/noncopyable.hpp>
using boost::noncopyable;
#else

class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}
private:  // emphasize the following members are private
	noncopyable( const noncopyable&);
	const noncopyable& operator=( const noncopyable&);
};

#endif