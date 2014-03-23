#pragma once

#include <sdk/noncopyable.h>

namespace archive
{
// 	template<>
// 	struct pod_archive<XXX_Stream>
// 	{
// 		static inline void read(XXX_Stream& stm, pod_buffer& );
// 		static inline void write(XXX_Stream& stm, const pod_const_data& );
// 	};
};

namespace serialization
{
	/*
		template<typename Archive>
		static inline void serialize(Archive & ar, XXX_ValueType & val, const unsigned int version)
		{}

		template<>
		struct serialize_t<XXX_StreamType, XXX_ValueType>
		{
			template<typename Archive>
			static inline void invoke(Archive & ar, XXX_ValueType & val, const unsigned int version)
			{}
		};
		template<>
		struct serialize_t<XXX_StreamType, XXX_ValueType> : serialize_load_save
		{
			template<typename Archive>
			static inline void save(Archive & ar, XXX_ValueType & val, const unsigned int version)
			{}
			template<typename Archive>
			static inline void load(Archive & ar, XXX_ValueType & val, const unsigned int version)
			{}
		};
	*/
	template<typename ValueType>
	struct version
	{
		enum { value = 0 };
	};
}

#define SERIALIZE_CLASS_VERSION(Class,Version) \
	namespace serialization{ \
	template<> struct version<Class> { \
	enum { value = Version }; \
}; };


//////////////////////////////////////////////////////////////////////////

namespace archive
{
	struct pod_const_data
	{
		pod_const_data( const void* pv, size_t _len ) : data( pv ), len( _len )
		{}
		pod_const_data( const char szStr[] ) : data( szStr ), len( sizeof( szStr ) )
		{}

		const void* data;
		const size_t len;
	};

	struct pod_buffer
	{
		pod_buffer( void* pv, size_t _len ) : buffer( pv ), buflen( _len )
		{}

		void* buffer;
		size_t buflen;
	};

	template<typename StreamType>
	struct pod_archive;

};

//////////////////////////////////////////////////////////////////////////
namespace serialization
{
	typedef archive::pod_const_data pod_const_data;
	typedef archive::pod_buffer pod_buffer;

	template<typename Archive>
	struct is_iarchive
	{
		enum
		{
			value = ( Archive::direction == 0 ),
		};
	};
	template<typename Archive>
	struct is_oarchive
	{
		enum
		{
			value = ( Archive::direction == 1 ),
		};
	};
	template<typename Archive>
	struct get_pod_type
	{
		typedef typename std::conditional< is_oarchive<Archive>::value, archive::pod_const_data, archive::pod_buffer >::type type;
	};

}

namespace serialization
{
	struct system_defined_flag
	{};
	struct serialize_load_save
	{};
};

namespace detail
{
	template<typename T>
	struct is_can_memory_copy
	{
		typedef typename std::remove_const< typename std::remove_reference<T>::type >::type not_const_ref_type;
		typedef typename std::remove_all_extents<not_const_ref_type>::type array_element_type;
		enum
		{
			is_array_t = std::is_array<not_const_ref_type>::value,
			is_char_array = ( is_array_t&&  std::is_same<array_element_type, char>::value ),
			is_wchar_array = ( is_array_t&&  std::is_same<array_element_type, wchar_t>::value ),
			is_pod_t = std::is_pod<not_const_ref_type>::value,
			is_pointer_t = std::is_pointer<not_const_ref_type>::value,

			value = ( is_pod_t&&  !is_pointer_t&&  !is_char_array && !is_wchar_array )
		};
	};
}

namespace serialization
{
	namespace detail
	{
		template<typename ClassType, bool is_cls = std::is_class<ClassType>::value>
		struct has_serialize_member_function
		{
			template <class U, U x>
			struct test;

			struct yes_type
			{
				char val[4];
			};
			struct no_type
			{
				char val[16];
			};

			struct Archive;

			template<class U>
			static yes_type check_sig1(
				U*,
				test <
				void ( ClassType::*)( Archive&, const unsigned int ),
				&U::serialize
				> * = NULL
			);
			template<class U>
			static no_type check_sig1( ... );

			enum
			{
				nsize = sizeof( check_sig1<ClassType>( 0 ) ),
				value = ( nsize == sizeof( yes_type ) )
			};
		};

		template<typename ClassType>
		struct has_serialize_member_function<ClassType, false>
		{
			enum
			{
				value = 0
			};
		};


		template<typename ValueType>
		struct has_global_serialize_function
		{
			struct yes_type
			{
				char val[4];
			};
			struct no_type
			{
				char val[16];
			};

			struct Archive;
			static Archive g_arg1;
			static ValueType g_arg2;

			typedef decltype( serialization::serialize( g_arg1, g_arg2, 0 ) ) result_type;

			enum
			{
				value = !std::is_same<result_type, serialization::system_defined_flag>::value
			};
		};
	};

	template<typename StreamType, typename ValueType>
	struct has_user_serialize_t
	{
		typedef typename std::remove_const<ValueType>::type not_const_value_type;

		enum
		{
			_is_def_class = !std::is_convertible< serialize_t<StreamType, not_const_value_type>, system_defined_flag >::value,
			is_def_rw_class = ( _is_def_class && std::is_convertible< serialize_t<StreamType, not_const_value_type>, serialize_load_save >::value ),
			is_def_global_func = detail::has_global_serialize_function<ValueType>::value,
			is_def_member_func = detail::has_serialize_member_function<ValueType>::value,
			is_user_def = ( _is_def_class || is_def_global_func || is_def_member_func ),
			value = is_user_def,
		};
	};

	template<typename StreamType, typename ValueType>
	struct has_serialize_t
	{
		enum
		{
			is_can_copy = ::detail::is_can_memory_copy<ValueType>::value,
			is_user_def = has_user_serialize_t<StreamType, ValueType>::value,
			is_pod_type = ( std::is_same<archive::pod_buffer, ValueType>::value || std::is_same<archive::pod_const_data, ValueType>::value ),
			value = ( is_can_copy || is_user_def || is_pod_type )
		};
	};
};

namespace serialization
{
	template<typename StreamType, typename ValueType>
	struct pod_serialize_t
	{
		template<typename Archive>
		static inline void invoke( Archive& ar, pod_buffer& val )
		{
			archive::pod_archive<StreamType>::read( ar.get_stream(), val );
		}
		template<typename Archive>
		static inline void invoke( Archive& ar, pod_const_data& val )
		{
			archive::pod_archive<StreamType>::write( ar.get_stream(), val );
		}

	};
	template<typename StreamType, typename ValueType>
	struct convert_to_pod_serialize_t
	{
		template<typename Archive>
		static inline void invoke( Archive& ar, ValueType& val )
		{
			typedef get_pod_type<Archive>::type pod_type;
#if 1
			pod_serialize_t<StreamType, ValueType>::invoke( ar, pod_type( &val, sizeof( val ) ) );
#else
			ar& pod_type( &val, sizeof( val ) );
#endif
		}

	};
	template<typename StreamType, typename ValueType>
	struct serialize_t : system_defined_flag
	{
		template<typename Archive>
		static inline system_defined_flag invoke( Archive& ar, ValueType& val );/*
        {
        	throw std::runtime_error("error");
        	return system_defined_flag();
        }*/
	};

	template<typename StreamType, typename ValueType>
	struct serialize_rw_t // : system_defined_flag
	{
		struct call_read
		{
			template<typename Archive>
			static inline void invoke( Archive& ar, ValueType& v, const unsigned int version )
			{
				serialize_t<StreamType, ValueType>::load( ar, v, version );
			}
		};
		struct call_write
		{
			template<typename Archive>
			static inline void invoke( Archive& ar, ValueType& v, const unsigned int version )
			{
				serialize_t<StreamType, ValueType>::save( ar, v, version );
			}
		};

		template<typename Archive>
		static inline void invoke( Archive& ar, ValueType& v )
		{
			typedef typename std::conditional< is_iarchive<Archive>::value, call_read, call_write>::type caller;
			caller::invoke( ar, v, version<ValueType>::value );
		}

	};

	template<typename Archive, typename ValueType>
	static inline system_defined_flag serialize( Archive& ar, ValueType& val, const unsigned int version );/*
	{
		throw std::runtime_error("error");
		return system_defined_flag();
	}*/

	template<typename StreamType, typename ValueType>
	struct gfunction_serialize_t
	{
		template<typename Archive>
		static inline void invoke( Archive& ar, ValueType& val )
		{
			serialize( ar, val, version<ValueType>::value );
		}
	};

	template<typename StreamType, typename ValueType>
	struct value_member_serialize_t
	{
		template<typename Archive>
		static inline void invoke( Archive& ar, ValueType& val )
		{
			val.serialize( ar, version<ValueType>::value );
		}
	};

	struct empty_caller;

	template<typename StreamType, typename ValueType>
	struct get_serialize_caller
	{
		typedef has_user_serialize_t<StreamType, ValueType> user_def_t;
		typedef has_serialize_t<StreamType, ValueType> all_def_t;

		typedef typename std::conditional < user_def_t::is_def_member_func, value_member_serialize_t<StreamType, ValueType>,
				typename std::conditional < user_def_t::is_def_rw_class, serialize_rw_t<StreamType, ValueType>,
				typename std::conditional < user_def_t::_is_def_class, serialize_t<StreamType, ValueType>,
				typename std::conditional < user_def_t::is_def_global_func, gfunction_serialize_t<StreamType, ValueType>,
				typename std::conditional < all_def_t::is_can_copy, convert_to_pod_serialize_t<StreamType, ValueType>,
				typename std::conditional < all_def_t::is_pod_type, pod_serialize_t<StreamType, ValueType>,
				empty_caller
				>::type >::type >::type >::type >::type >::type type;
	};
}
//////////////////////////////////////////////////////////////////////////
namespace archive
{
	enum
	{
		archive_in = 0,
		archive_out = 1,
	};

	template<typename StreamType_, int direct>
	struct base_archive  : noncopyable
	{
		enum
		{
			direction = direct
		};

		typedef StreamType_ StreamType;
	};

	//////////////////////////////////////////////////////////////////////////

	template<typename StreamType>
	struct iarchive_t : base_archive<StreamType, archive_in>
	{
		StreamType& m_stm;

		iarchive_t( StreamType* stm ) : m_stm( *stm )
		{}
		iarchive_t( StreamType& stm ) : m_stm( stm )
		{}

		StreamType& get_stream()const
		{
			return m_stm;
		}

		typedef iarchive_t<StreamType> this_type;

		template<typename ValueType>
		this_type& operator >> ( ValueType& val )
		{
			typedef serialization::get_serialize_caller<StreamType, ValueType>::type caller;
			static_assert( !std::is_same<caller, serialization::empty_caller>::value, "not define serialize function/class" );
			caller::invoke( *this, val );
			return *this;
		}
		template<typename ValueType>
		this_type& operator &( ValueType& val )
		{
			typedef serialization::get_serialize_caller<StreamType, ValueType>::type caller;
			static_assert( !std::is_same<caller, serialization::empty_caller>::value, "not define serialize function/class" );
			caller::invoke( *this, val );
			return *this;
		}
// 	private:
// 		template<typename ValueType>
// 		this_type& operator << (ValueType& val) { return *this; }
	};

	template<typename StreamType>
	struct oarchive_t : base_archive<StreamType, archive_out>
	{
		StreamType& m_stm;

		oarchive_t( StreamType* stm ) : m_stm( *stm )
		{}
		oarchive_t( StreamType& stm ) : m_stm( stm )
		{}

		StreamType& get_stream()const
		{
			return m_stm;
		}


		typedef oarchive_t<StreamType> this_type;

		template<typename ValueType>
		this_type& operator << ( const ValueType& val )
		{
			typedef serialization::get_serialize_caller<StreamType, ValueType>::type caller;
			static_assert( !std::is_same<caller, serialization::empty_caller>::value, "not define serialize function/class" );
			typedef std::remove_const<ValueType>::type not_const_type;
			caller::invoke( *this, *const_cast<not_const_type*>( &val ) );
			return *this;
		}
		template<typename ValueType>
		this_type& operator &( const ValueType& val )
		{
			typedef serialization::get_serialize_caller<StreamType, ValueType>::type caller;
			static_assert( !std::is_same<caller, serialization::empty_caller>::value, "not define serialize function/class" );
			typedef std::remove_const<ValueType>::type not_const_type;
			caller::invoke( *this, *const_cast<not_const_type*>( &val ) );
			return *this;
		}
// 	private:
// 		template<typename ValueType>
// 		this_type& operator >> (ValueType& val){ return *this; }
	};


	template<typename StreamType >
	struct new_iarchive_t : iarchive_t<StreamType>
	{
		typedef iarchive_t<StreamType> super_type;
		StreamType m_newstm;
		new_iarchive_t() : super_type( m_newstm )
		{}

		using super_type::operator>>;
		using super_type::operator&;
	};

	template<typename StreamType >
	struct new_oarchive_t : oarchive_t<StreamType>
	{
		typedef oarchive_t<StreamType> super_type;
		StreamType m_newstm;
		new_oarchive_t() : super_type( m_newstm )
		{}

		using super_type::operator<<;
		using super_type::operator&;
	};

};
//////////////////////////////////////////////////////////////////////////

namespace global_operation
{
	template<typename StreamType, typename ValueType>
	static inline StreamType& operator<<( StreamType& stm, const ValueType& val )
	{
		archive::iarchive_t<StreamType> ar( stm );
		ar& val;
		return stm;
	}
	template<typename StreamType, typename ValueType>
	static inline StreamType& operator>>( StreamType& stm, const ValueType& val )
	{
		archive::oarchive_t<StreamType> ar( stm );
		ar& val;
		return stm;
	}
};