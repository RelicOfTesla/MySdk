#pragma once

#include <c++10/memory.h>

using std::shared_ptr;
using std::enable_shared_from_this;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::const_pointer_cast;

template<typename T>
struct arg_sptr
{
#if USE_SAFE_ARG_SPTR
	typedef shared_ptr<T> type;
#else
	typedef const shared_ptr<T>& type;
#endif
};

template<typename T>
struct arg_sptr_c
{
#if USE_SAFE_ARG_SPTR
	typedef shared_ptr<const T> type;
#else
	typedef const shared_ptr<const T>& type;
#endif
};