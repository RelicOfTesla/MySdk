#pragma once

#include <sdk/shared_ptr.h>
#include <sdk/function_arg.hpp>
#include <sdk/expand_macro.h>

#include <sdk/boost_lib/signals.hpp>

template<typename function_type, bool must_valid_func = false, bool expired_throw = true >
struct slot_function
{
	shared_ptr< signals::slot<function_type> > m_ptr;

	template<typename F>
	slot_function( const F& f ) : m_ptr( new signals::slot<function_type>( f ) )
	{}

	slot_function()
	{}

	size_t tracked_count()const
	{
		if( m_ptr )
		{
			return m_ptr->tracked_objects().size();
		}
		return 0;
	}
	bool expired() const
	{
		if( m_ptr )
		{
			return m_ptr->expired();
		}
		return true;
	}

	typedef typename get_result_of<function_type>::type result_type;


#pragma push_macro("SLOT_DEF_FUNC")
#define SLOT_DEF_FUNC(_m, N, _d) \
	BOOST_PP_EXPR_IF(N, template< __any_typename_T(N) > ) \
	result_type operator()( __any_params_t(N) ) \
    { \
        if( !m_ptr ) \
		{ \
            if( must_valid_func ) \
            { \
                throw std::runtime_error( "not initialize function" ); \
            } \
			return result_type(); \
		} \
        try \
        { \
            return ( *m_ptr )( __any_args_t( N ) ); \
        } \
		catch(signals::expired_slot&) \
		{ \
			if (expired_throw) \
			{ \
				throw; \
			} \
			return result_type(); \
		} \
    }

	__expand_repeat( 9, SLOT_DEF_FUNC, _ );

#pragma pop_macro("SLOT_DEF_FUNC")
};
