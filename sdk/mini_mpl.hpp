#pragma once

#include "user_config.h"

#if HAS_BOOST && !_DEBUG
#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/transform.hpp>
#define HAS_BOOST_MPL_VECTOR_N 1
namespace mpl
{
	using boost::mpl::vector;
	using boost::mpl::push_back;
	using boost::mpl::at;
	using boost::mpl::size;
	using boost::mpl::int_;
	using boost::mpl::next;
	using boost::mpl::inserter;
	using boost::mpl::copy;
	using boost::mpl::transform;

	using boost::mpl::na;
};
#else

#define HAS_BOOST_MPL_VECTOR_N 0
#include "my_mpl.hpp"

#endif