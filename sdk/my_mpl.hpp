#pragma once

#include "user_config.h"
#include <sdk/expand_macro.h>

namespace mpl
{
	struct na;

	template<int I>
	struct int_
	{
		enum
		{
			value = I
		};
	};

#pragma push_macro("DEF_FUNCTION")

	template <
		int MAX_SIZE,
		typename T1 = mpl::na, typename T2 = mpl::na, typename T3 = mpl::na, typename T4 = mpl::na, typename T5 = mpl::na,
		typename T6 = mpl::na, typename T7 = mpl::na, typename T8 = mpl::na, typename T9 = mpl::na, typename T10 = mpl::na >
	struct base_vector
	{
		enum { max_size = MAX_SIZE };


		template<typename I>
		struct index_type;

		template<>
		struct index_type< int_<0> >
		{
			typedef T1 type;
		};
		template<>
		struct index_type< int_<1> >
		{
			typedef T2 type;
		};
		template<>
		struct index_type< int_<2> >
		{
			typedef T3 type;
		};
		template<>
		struct index_type< int_<3> >
		{
			typedef T4 type;
		};
		template<>
		struct index_type< int_<4> >
		{
			typedef T5 type;
		};
		template<>
		struct index_type< int_<5> >
		{
			typedef T6 type;
		};
		template<>
		struct index_type< int_<6> >
		{
			typedef T7 type;
		};
		template<>
		struct index_type< int_<7> >
		{
			typedef T8 type;
		};
		template<>
		struct index_type< int_<8> >
		{
			typedef T9 type;
		};
		template<>
		struct index_type< int_<9> >
		{
			typedef T10 type;
		};

	};

	template < typename T1 = mpl::na, typename T2 = mpl::na, typename T3 = mpl::na, typename T4 = mpl::na, typename T5 = mpl::na,
			   typename T6 = mpl::na, typename T7 = mpl::na, typename T8 = mpl::na, typename T9 = mpl::na, typename T10 = mpl::na >
	struct vector;

#undef DEF_FUNCTION
#define DEF_FUNCTION(_m, N, _d) \
    template< __any_typename_T(N) > \
    struct vector< __any_typename_T(N) > : base_vector<N  __comma_if(N) __any_typename_T(N) > \
    {};

	__expand_repeat(9, DEF_FUNCTION, _);
};

namespace mpl
{
	template<typename V, typename I>
	struct at
	{
		typedef typename V::index_type<I>::type type;
	};

	template<typename V>
	struct size
	{
		enum
		{
			value = V::max_size
		};
	};

	template<typename T>
	struct next;

	template<int IValue>
	struct next< mpl::int_<IValue> >
	{
		typedef mpl::int_ < IValue + 1 > type;
	};
};

namespace mpl
{
	template<typename V, typename T>
	struct push_back;

#undef DEF_FUNCTION
#define DEF_FUNCTION(_m, N, _d) \
    template< __any_typename_T(N) __comma_if(N)  typename new_element > \
    struct push_back< vector<__any_typename_T(N)>, new_element  > \
    { \
        typedef vector<__any_typename_T(N) __comma_if(N) new_element> type; \
    };

	__expand_repeat(9, DEF_FUNCTION, _);
#pragma pop_macro("DEF_FUNCTION")
}

namespace mpl
{
	template<typename T, typename F>
	struct inserter
	{
		typedef T state;
		typedef F operation;
	};


	template<typename SrcV, typename inserter_t, typename I = mpl::int_<0>>
	struct copy_impl
	{
		typedef typename mpl::at<SrcV, I>::type		current_node_type;
		typedef typename inserter_t::operation		operation;
		typedef typename inserter_t::state			state;

		typedef typename operation::apply<state, current_node_type>::type		new_state;
		typedef mpl::inserter<new_state, operation>		new_inserter;
		typedef typename copy_impl < SrcV, new_inserter, typename mpl::next<I>::type >::type	type;
	};

	template<typename SrcV, typename inserter_t>
	struct copy_impl < SrcV, inserter_t, mpl::int_ < mpl::size<SrcV>::value - 1 > >
	{
		typedef typename inserter_t::state state;
		typedef state new_state;
		typedef new_state type;
	};

	template<typename V, typename inserter_t>
	struct copy
	{
		typedef typename copy_impl<V, inserter_t>::type type;
	};

	template<typename inserter_t>
	struct transform_inserter
	{
		template<typename V, typename T>
		struct apply
		{
			typedef typename inserter_t::apply<T>::type new_et;
			typedef typename mpl::push_back<V, new_et>::type type;
		};
	};

	template<typename V, typename inserter_t>
	struct transform
	{
		typedef typename copy < V,
				mpl::inserter< mpl::vector<>, transform_inserter<inserter_t> >
				>::type type;
	};
};
