#pragma once

#include "stream_t.hpp"
#include "../dynamic_function_type.hpp"
#include <sdk/expand_macro.h>

namespace
{
	template<typename Stream, typename Function>
	struct prev_stream_invoke
	{
		static inline void invoke( Stream& stm, const Function& func )
		{
		}
	};
};

namespace
{
	template<typename V>
	struct vector_call;


#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_mm, N, _d) \
	template< __any_typename_T(N) > \
    struct vector_call< mpl::vector<__any_typename_T(N)> > \
    { \
        template<typename Stream, typename Function> \
        static inline void invoke( Stream& stm, const Function& func ) \
        { \
            BOOST_PP_ENUM_BINARY_PARAMS(N, T,  a; BOOST_PP_INTERCEPT); \
            stm  __expand_params0(N, >> a) ; \
            prev_stream_invoke<Stream, Function>::invoke( stm, func ); \
            func( __expand_params0(N, a) ); \
        } \
    };

	__expand_repeat(9, DEF_FUNCTION, );
#pragma pop_macro("DEF_FUNCTION")

#if HAS_BOOST_MPL_VECTOR_N
	template<>
	struct vector_call< boost::mpl::vector0<> > : vector_call< mpl::vector<> >
	{};
	template<typename T1>
	struct vector_call< boost::mpl::vector1<T1> > : vector_call< mpl::vector<T1> >
	{};
	template<typename T1, typename T2>
	struct vector_call< boost::mpl::vector2<T1, T2> > : vector_call< mpl::vector<T1, T2> >
	{};
	template<typename T1, typename T2, typename T3>
	struct vector_call< boost::mpl::vector3<T1, T2, T3> > : vector_call< mpl::vector<T1, T2, T3> >
	{};
	template<typename T1, typename T2, typename T3, typename T4>
	struct vector_call< boost::mpl::vector4<T1, T2, T3, T4> > : vector_call< mpl::vector<T1, T2, T3, T4> >
	{};
#endif
}

template<typename T>
struct handler_object;

namespace
{
	template<typename Stream>
	struct transform_serialize_obj
	{
		template<typename T>
		struct apply
		{
			typedef typename std::conditional <
			!serialization::has_serialize_t<Stream, T>::value,
			handler_object<T>, T
			>::type type;
		};
	};
};

template<typename Stream, typename Function>
inline void StreamCall( Stream& stm, const Function& func )
{
	typedef typename get_function_params<Function>::type params_list;
	typedef typename mpl::transform< params_list, transform_serialize_obj<Stream> >::type sps_list;
	vector_call < sps_list >::invoke( stm, func );
}


template<typename Stream>
struct stream_binder_t
{
	struct bind_call_base_t
	{
		virtual void invoke( Stream&) = 0;
	};

	template<typename F>
	struct bind_caller_t : bind_call_base_t
	{
		F m_func;
		bind_caller_t( const F& func ) : m_func( func )
		{}

		virtual void invoke( Stream& istm )
		{
			StreamCall( istm, m_func );
		}
	};

	template<typename F>
	static inline shared_ptr<bind_call_base_t> create_bind_call( const F& func )
	{
		shared_ptr<bind_call_base_t> p( new bind_caller_t<F>( func ) );
		return p;
	}
};

/*
typedef stream_binder_t<binary_istream>::bind_call_base_t bind_call_base;

template<typename F>
shared_ptr<bind_call_base> create_bind_call(const F& func)
{
	return stream_binder_t<binary_istream>::create_bind_call(func);
}
*/