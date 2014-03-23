#pragma once

#include "mini_mpl.hpp"
#include <sdk/expand_macro.h>
#if _CPPLIB_VER >= 520
#include <type_traits>
#elif HAS_BOOST
#include <boost/tr1/type_traits.hpp>
#else
#error "not support <type_traits>";
#endif


namespace
{
	template<typename F>
	struct function_deref
	{
		typedef typename std::remove_pointer <
		typename std::remove_cv<F>::type
		>::type type;
	};
};

namespace
{
	template<typename F>
	struct get_result_of;

	template<typename F>
	struct get_result_of<const F> : get_result_of<F>
	{};

	template<typename F>
	struct get_result_of<F*> : get_result_of<F>
	{};

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, CALL_STYLE) \
    template<typename R __comma_if(N) __any_typename_T(N) > \
    struct get_result_of< R CALL_STYLE( __any_typename_T(N) ) > \
    { \
		typedef R type; \
	};

	__expand_repeat(9, DEF_FUNCTION, __cdecl);
	__expand_repeat(9, DEF_FUNCTION, __stdcall);



#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, CALL_STYLE) \
	template<typename R, typename ClassT  __comma_if(N) __any_typename_T(N) > \
	struct get_result_of< R ( CALL_STYLE ClassT::* )( __any_typename_T(N) ) > \
	{ \
		typedef R type; \
	};

	__expand_repeat(9, DEF_FUNCTION, __thiscall);
	__expand_repeat(9, DEF_FUNCTION, __stdcall);
	__expand_repeat(9, DEF_FUNCTION, __cdecl);
#pragma pop_macro("DEF_FUNCTION")
	//////////////////////////////////////////////////////////////////////////

};

namespace
{
	template<typename F>
	struct get_function_class
	{
		typedef mpl::na type;
	};

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, CALL_STYLE) \
	template<typename R, typename classtype  __comma_if(N) __any_typename_T(N) > \
	struct get_function_class< R( CALL_STYLE classtype::* )( __any_typename_T(N) ) > \
	{ \
		typedef classtype type; \
	};

	__expand_repeat(9, DEF_FUNCTION, __thiscall);
	__expand_repeat(9, DEF_FUNCTION, __stdcall);
	__expand_repeat(9, DEF_FUNCTION, __cdecl);

#pragma pop_macro("DEF_FUNCTION")
}

#if HAS_BOOST
#include <boost/bind.hpp>
#include <boost/function.hpp>
namespace
{

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, Suffix) \
	template<typename R, typename classtype  __comma_if(N) __any_typename_T(N) > \
	struct get_function_class< boost::_mfi::mf ## N ## Suffix <R, classtype __comma_if(N) __any_typename_T(N)> > \
	{ \
		typedef classtype type; \
	};

	__expand_repeat(9, DEF_FUNCTION, );
	__expand_repeat(9, DEF_FUNCTION, _stdcall);
#pragma pop_macro("DEF_FUNCTION")

}
#endif
//////////////////////////////////////////////////////////////////////////
namespace
{
	template<typename F>
	struct get_args_type;

	template<typename F>
	struct get_args_type <const F> : get_args_type<F>
	{

	};
	template<typename F>
	struct get_args_type <F*> : get_args_type<F>
	{

	};

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, CALL_STYLE) \
	template<typename R  __comma_if(N) __any_typename_T(N) > \
	struct get_args_type< R CALL_STYLE( __any_typename_T(N) ) > \
	{ \
		typedef mpl::vector<__any_typename_T(N)> type; \
	};

	__expand_repeat(9, DEF_FUNCTION, __stdcall);
	__expand_repeat(9, DEF_FUNCTION, __cdecl);
	//////////////////////////////////////////////////////////////////////////

#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, CALL_STYLE) \
    template<typename R, typename ClassT  __comma_if(N) __any_typename_T(N) > \
    struct get_args_type< R( CALL_STYLE ClassT::* )( __any_typename_T(N) ) > \
    { \
        typedef mpl::vector<ClassT  __comma_if(N) __any_typename_T(N)> type; \
    };

	__expand_repeat(9, DEF_FUNCTION, __thiscall);
	__expand_repeat(9, DEF_FUNCTION, __stdcall);
	__expand_repeat(9, DEF_FUNCTION, __cdecl);
#pragma pop_macro("DEF_FUNCTION")
}
//////////////////////////////////////////////////////////////////////////
#if HAS_BOOST
#include <boost/bind.hpp>
#include <boost/function.hpp>
namespace
{
	template<>
	struct get_args_type< boost::_bi::list0 >
	{
		typedef mpl::vector<> type;
	};

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, _) \
    template< __any_typename_T(N) > \
	struct get_args_type< boost::_bi::list ## N <__any_typename_T(N)> > \
    { \
        typedef mpl::vector<__any_typename_T(N)> type; \
    };

	__expand_repeat_from_to(1, 9, DEF_FUNCTION, );

	//////////////////////////////////////////////////////////////////////////
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, Suffix) \
    template<typename R, typename ClassT  __comma_if(N) __any_typename_T(N) > \
	struct get_args_type< boost::_mfi::mf ## N ## Suffix <R, ClassT  __comma_if(N) __any_typename_T(N) > > \
    { \
        typedef mpl::vector<ClassT __comma_if(N) __any_typename_T(N)> type; \
    };

	__expand_repeat(9, DEF_FUNCTION, );
	__expand_repeat(9, DEF_FUNCTION, _stdcall);
#pragma pop_macro("DEF_FUNCTION")
};


#endif
