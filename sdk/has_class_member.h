#pragma once


struct has_class_member_comm
{
	template<typename U, U >
	struct tester
	{
	};

	struct yes_type
	{
		char arr[4];
	};
	struct not_type
	{
		char arr[8];
	};
};

template<typename ClassType, typename DefType>
struct has_class_member_t : DefType
{
	using DefType::check_sig;
	typedef has_class_member_comm::not_type not_type;
	typedef has_class_member_comm::yes_type yes_type;

	template<typename U>
	static not_type check_sig( ... );

	enum
	{
		size = sizeof( check_sig<ClassType>( 0 ) ),
		value = size == sizeof( yes_type )
	};
};
#define DefHasClassMember(TNAME, MEMBER_TYPE, MEMBER_NAME) \
struct TNAME : has_class_member_comm { \
	template<typename T> \
	static yes_type check_sig( tester< MEMBER_TYPE,  &T::MEMBER_NAME >* p = 0 ); \
};

/*
struct def_has_f1 : has_class_member_comm
{
	template<typename T>
	static yes_type check_sig( tester< void(T::*)(),  &T::f1 >* p = 0 );
};

// DefHasClassMember(def_has_f1, void(T::*)(), f1);

has_class_member_t< ctest, def_has_f1 >::value

*/
