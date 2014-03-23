#pragma once

#include "function_arg.hpp"
#include <sdk/expand_macro.h>

#pragma pack(push, 1)
/*

// get function args list
template<typename F>
struct get_function_args
{
	typedef typename mpl::vector<...> type;
};

// get function params list
template<typename F, typename transform_applay = def_transform_applay >
struct get_function_params
{
typedef typename mpl::vector<...> type;
};

// get function context
template<typename F>
struct get_function_context
{
	template<typename R, typename CT, typename V>
	struct mpl_function_context_type
	{
		typedef R result_type;
		typedef CT class_type;
		typedef V args_vector_type;
	};

	typedef typename mpl_function_context_type<...> type;
}
*/

namespace
{
	template<typename V = mpl::vector<>, typename P = mpl::int_<0> >
	struct hold_result_info
	{
		enum { reserve_size = P::value };
		typedef V vector_type;

		typedef hold_result_info < V, mpl::int_ < reserve_size + 1 > > inc_reserve;
	};

	template<typename RawList>
	struct hold_inserter
	{
		template<typename old_result_type, int ArgI>
		struct swap_hold
		{
			enum
			{
				reserve_size = old_result_type::reserve_size,
				rawfunc_index = ArgI + reserve_size,
			};
			typedef typename mpl::at < RawList, mpl::int_ < rawfunc_index - 1 > >::type arg_node;
			typedef arg_node arg_type;
			//typedef typename instance_arg_type<arg_node>::type arg_type;
			typedef typename mpl::push_back<typename old_result_type::vector_type, arg_type>::type new_vector;
			typedef hold_result_info<new_vector, mpl::int_<reserve_size> > type;
		};

		template<typename old_result_type, typename T>
		struct apply
		{
			typedef typename old_result_type::inc_reserve type;
		};

		template<typename old_result_type, int ArgI>
		struct apply<old_result_type, boost::arg<ArgI> >
				: swap_hold<old_result_type, ArgI >
		{};

		template<typename old_result_type, int ArgI>
		struct apply<old_result_type, std::tr1::_Ph<ArgI> >
				: swap_hold<old_result_type, ArgI >
		{};
	};

	template<typename _RawList, typename HoldList>
	struct FunctionSwaper
	{
		typedef typename mpl::copy < HoldList,
				mpl::inserter<hold_result_info<>, hold_inserter<_RawList> >
				>::type each_result;
		typedef typename each_result::vector_type type;
	};
}

//////////////////////////////////////////////////////////////////////////
//namespace
//{
template<typename R, typename CT, typename V>
struct mpl_function_context_type
{
	typedef R result_type;
	typedef CT class_type;
	typedef V args_vector_type;
};

template<typename F>
struct get_function_context
{
	typedef typename get_result_of<F>::type				result_type;
	typedef typename get_function_class<F>::type		class_type;
	typedef typename get_args_type<F>::type				args_type;

	typedef mpl_function_context_type<result_type, class_type, args_type> type;
};

template<typename F>
struct get_function_args
{
	typedef typename get_function_context<F>::type context;
	typedef typename context::args_vector_type type;
};

struct transform_get_param
{
	template<typename T>
	struct apply
	{
		static_assert( !std::is_same<T, mpl::na>::value, "error arg" );

		typedef typename std::remove_const <
		typename std::remove_reference<T>::type
		>::type type;
	};
};

template<typename F>
struct get_function_params
{
	typedef typename get_function_args<F>::type args_list;

	typedef typename mpl::transform<args_list, transform_get_param>::type type;
};
//};



#if _CPPLIB_VER>=520
namespace
{
	template<typename F>
	struct get_function_context< std::function<F> >  : get_function_context<F>
	{};
};
namespace
{
	template<typename T>
	struct mpl_make_std_bind_args;

	template<typename F>
	struct mpl_deref_std_bind_func
	{
		typedef F type;
	};
	template<typename F, typename CT, bool bUnk>
	struct mpl_deref_std_bind_func< std::tr1::_Callable_pmf<F, CT, bUnk> >
	{
		typedef F type;
	};
	template<typename F, bool bUnk>
	struct mpl_deref_std_bind_func< std::tr1::_Callable_fun<F, bUnk> >
	{
		typedef F type;
	};

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, _d) \
	template<typename F  __comma_if(N) __any_typename_T(N) > \
	struct mpl_make_std_bind_args< std::tr1::_Bind ## N <F __comma_if(N) __any_typename_T(N)> > \
    { \
        typedef typename mpl_deref_std_bind_func<F>::type			left; \
        typedef typename mpl::vector<__any_typename_T(N)>		right; \
    };

	__expand_repeat(9, DEF_FUNCTION, );
#pragma pop_macro("DEF_FUNCTION")

	template<typename R, typename BindT>
	struct get_function_context< std::tr1::_Bind<R, R, BindT> >
	{
		typedef R result_type;
		typedef typename mpl_make_std_bind_args<BindT>::left	raw_func;
		typedef typename get_function_class<raw_func>::type		class_type;
		typedef typename get_args_type<raw_func>::type			raw_list;
		typedef typename mpl_make_std_bind_args<BindT>::right	hold_list;

		typedef typename FunctionSwaper<raw_list, hold_list>::type args_type;
		typedef mpl_function_context_type<result_type, class_type, args_type> type;
	};
};

#endif

#if HAS_BOOST
#include <boost/bind.hpp>
#include <boost/function.hpp>
namespace
{
	template<typename F>
	struct get_function_context< boost::function<F> > : get_function_context<F>
	{};
};
namespace
{
	template<typename R, typename F, typename ArgT>
	struct get_function_context< boost::_bi::bind_t<R, F, ArgT> >
	{
		typedef R result_type;
		typedef typename get_function_class<F>::type	class_type;
		typedef typename get_args_type<F>::type			raw_list;
		typedef typename get_args_type<ArgT>::type		hold_list;

		typedef typename FunctionSwaper<raw_list, hold_list>::type args_type;
		typedef mpl_function_context_type<result_type, class_type, args_type> type;
	};
};
#endif

#pragma pack(pop)