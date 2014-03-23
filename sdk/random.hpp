#pragma once

#include <limits>
#include "dbg_assert.h"

namespace detail
{
#pragma pack(push,1)
	template<typename RT, typename HT, typename LT = HT>
	struct combine_value_t
	{
		union
		{
			RT rv;
			struct
			{
				HT hv;
				LT lv;
			};
		};

		combine_value_t( HT hv_, LT lv_ ) : hv( hv_ ), lv( lv_ )
		{}
	};
#pragma pack(pop)


	template<int size>
	struct get_max_mem_limit_impl;

	template<>
	struct get_max_mem_limit_impl<1>
	{
		enum { value = 0xff };
	};
	template<>
	struct get_max_mem_limit_impl<2>
	{
		enum { value = 0xffff };
	};
	template<>
	struct get_max_mem_limit_impl<4>
	{
		enum { value = 0xffffffff };
	};
	template<>
	struct get_max_mem_limit_impl<8>
	{
		enum { value = 0xffffffffffffffff };
	};

	template<typename T>
	struct get_max_num_limit
	{
		enum
		{
			value = get_max_mem_limit_impl<sizeof( T )>::value
		};
	};

	typedef unsigned __int64 max_mask_type;

	template<typename T, max_mask_type rand_max_value, bool is_greater_or_equal_max>
	struct rand_impl_t;

	template<typename T, max_mask_type rm, max_mask_type tm>
	struct rand_type_proxy_t : rand_impl_t < T, rm, ( rm >= tm ) >
	{
	};

	template<typename T>
	struct rand_t : rand_type_proxy_t < T, RAND_MAX, get_max_num_limit<T>::value >
	{

	};
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int8, rand_max_value, true>
	{
		static inline unsigned __int8 invoke()
		{
			static_assert( rand_max_value == RAND_MAX, "error RAND_MAX" );
			static_assert( RAND_MAX >= 0xff, "error RAND_MAX" );
			return rand() & 0xff;
		}
	};
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int8, rand_max_value, false>
	{
	};
	//

	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int16, rand_max_value, true>
	{
		static inline unsigned __int16 invoke()
		{
			static_assert( rand_max_value == RAND_MAX, "error RAND_MAX" );
			static_assert( RAND_MAX >= 0xffff, "error RAND_MAX" );
			return rand() & 0xffff;
		}
	};
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int16, rand_max_value, false>
	{
		static inline unsigned __int16 invoke()
		{
			typedef rand_t<unsigned __int8> rand_u8;
			combine_value_t<unsigned __int16, unsigned __int8> v( rand_u8::invoke(), rand_u8::invoke() );
			return v.rv;
		}
	};
	//
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int32, rand_max_value, true>
	{
		static inline unsigned __int32 invoke()
		{
			static_assert( rand_max_value == RAND_MAX, "error RAND_MAX" );
			static_assert( RAND_MAX >= 0xffffffff, "error RAND_MAX" );
			return rand() & 0xffffffff;
		}
	};
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int32, rand_max_value, false>
	{
		static inline unsigned __int32 invoke()
		{
			typedef rand_t<unsigned __int16> rand_u16;
			combine_value_t<unsigned __int32, unsigned __int16> v( rand_u16::invoke(), rand_u16::invoke() );
			return v.rv;
		}
	};
	//
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int64, rand_max_value, true>
	{
		static inline unsigned __int64 invoke()
		{
			static_assert( rand_max_value == RAND_MAX, "error RAND_MAX" );
			static_assert( RAND_MAX >= 0xffffffffffffffff, "error RAND_MAX" );
			return rand() & 0xffffffffffffffff;
		}
	};
	template<max_mask_type rand_max_value>
	struct rand_impl_t<unsigned __int64, rand_max_value, false>
	{
		static inline unsigned __int64 invoke()
		{
			typedef rand_t<unsigned __int32> rand_u32;
			combine_value_t<unsigned __int64, unsigned __int32> v( rand_u32::invoke(), rand_u32::invoke() );
			return v.rv;
		}
	};
	//

	//////////////////////////////////////////////////////////////////////////

	template<> struct rand_t<__int8> : rand_t<unsigned __int8> {};
	template<> struct rand_t<__int16> : rand_t<unsigned __int16> {};
	template<> struct rand_t<__int32> : rand_t<unsigned __int32> {};
	template<> struct rand_t<__int64> : rand_t<unsigned __int64> {};

	template<> struct rand_t<double>;
	template<> struct rand_t<float>;

#pragma push_macro("max")
#pragma push_macro("min")
#undef min
#undef max

	template<typename T >
	static T random_t( T min_ = std::numeric_limits<T>::min(), T max_ = std::numeric_limits<T>::max() )
	{
		dbg_assert( min_ <= max_ );

		if( min_ > max_ )
		{
			throw std::range_error( "error random min-max range" );
		}

		if( max_ - min_ + 1 == 0 )
		{
			return rand_t<T>::invoke();
		}

		return T( rand_t<T>::invoke() % ( max_ - min_ + 1 ) + min_ );
	};

#pragma pop_macro("max")
#pragma pop_macro("min")
};


using detail::random_t;

