#pragma once

namespace
{
	template<typename F>
	struct get_native_function_type
	{
		typedef F type;
		//static_assert(std::is_function<type>::value || std::is_member_function_pointer<type>::value, "not function");
	};

// 	template<typename F>
// 	struct get_native_function_type< F* > : get_native_function_type<F>
// 	{};
};


#include <memory>
#if _CPPLIB_VER>=520
namespace
{
	template<typename F>
	struct get_native_function_type< std::function<F> > : get_native_function_type<F>
	{};
}
#include <type_traits>
#include <sdk/expand_macro.h>
namespace
{
	template<typename R, typename BindT>
	struct get_native_function_type< std::tr1::_Bind<R, R, BindT> > : get_native_function_type<BindT>
	{};
	//

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, _d) \
	template<typename F __comma_if(N) __any_typename_T(N)> \
	struct get_native_function_type< std::tr1::_Bind ## N <F __comma_if(N) __any_typename_T(N) > > \
		: get_native_function_type<F> \
	{};

	__expand_repeat(9, DEF_FUNCTION, );
#pragma pop_macro("DEF_FUNCTION")

	template<typename F, bool _Indirect>
	struct get_native_function_type< std::tr1::_Callable_fun<F, _Indirect> >
			: get_native_function_type<typename std::remove_const<F>::type>
	{};

	template<typename F, typename ClassT, bool _Indirect>
	struct get_native_function_type< std::tr1::_Callable_pmf<F, ClassT, _Indirect> >
			: get_native_function_type<typename std::remove_const<F>::type>
	{};
}
#endif


#include <sdk/user_config.h>
#if HAS_BOOST
#include <boost/bind.hpp>
#include <boost/function.hpp>
namespace
{
	template<typename F>
	struct get_native_function_type< boost::function<F> > : get_native_function_type< F >
	{};

	template<typename R, typename F, typename ArgT>
	struct get_native_function_type< boost::_bi::bind_t<R, F, ArgT> > : get_native_function_type< F >
	{};
}

namespace
{
#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, _d) \
	template<typename R, typename ClassT  __comma_if(N) __any_typename_T(N) > \
	struct get_native_function_type< boost::_mfi::mf ## N <R,ClassT  __comma_if(N) __any_typename_T(N) > > \
		: get_native_function_type< R(ClassT::*)(__any_typename_T(N)) > \
	{};

	__expand_repeat(9, DEF_FUNCTION, );
#pragma pop_macro("DEF_FUNCTION")

}

#endif