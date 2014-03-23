#pragma once

#include "user_config.h"
#if HAS_BOOST
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/expr_if.hpp>
#include <boost/preprocessor/comma_if.hpp>
#else
#pragma message( __FILE__ ": not support expand macro")
//#error "not support expand macro"
#endif

#define __expand_repeat			BOOST_PP_REPEAT
//	def(count, user_macro, data)
//		user_macro(count-1, 0, data) user_macro(count-1, 1, data) ... user_macro(count-1, count-1, data)
//	def(3, my_macro, data)	// expands to [ my_macro(2,0,data) my_macro(2,1,data) my_macro(2,2,data) ]

#define __expand_repeat_from_to		BOOST_PP_REPEAT_FROM_TO
//	def(first, last, user_macro, data)
//	user_macro(z, first, data) user_macro(z, first + 1, data) ... user_macro(z, last - 1, data)


#define __expand_params0		BOOST_PP_ENUM_PARAMS
//	def(count, param)
//		param##0, param##1, ... param##count-1
//	def(3, class T)		// expands to [class T0, class T1, class T2]

#define __expand_params2		BOOST_PP_ENUM_BINARY_PARAMS
//	def(count, p1, p2)
//	def(3, T, p)		// expands to [T0 p0, T1 p1, T2 p2]
//	p1##0 p2##0, p1##1 p2##1, ... p1##count-1 p2##count-1

#define __macro_cat(a, b)		BOOST_PP_CAT(a,b)
//	def(x, BOOST_PP_CAT(y, z))	// expands to xyz

//#define expr_if(cond, expr)		BOOST_PP_EXPR_IF(cond, expr)
//	if(cond!=0) expands to [expr]

#define __comma_if(cond)		BOOST_PP_COMMA_IF(cond)
//	if(cond!=0) expands to [,]


#define __any_typename_T(N)		__expand_params0(N, typename T)
#define __any_params_t(N)		__expand_params2( N, T, const& __a )
#define __any_args_t(N)			__expand_params0( N, __a )

#define __DEF_OPER_CALL( R, N, MEMBER ) \
	template< __any_typename_T > \
	R operator()( __any_param_t(N) ) \
	{ \
	return ( MEMBER )( __any_param_t(N) ); \
	}