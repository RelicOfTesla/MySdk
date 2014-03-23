#pragma once

#include "config.h"

namespace mui
{
	struct ITemplateDB;

	struct ITemplateIterator
	{
		virtual void move_next() = 0;
		virtual bool empty() const = 0;
		virtual shared_ptr<ITemplateDB> get_value() const = 0;
	};

	struct ITemplateDB
	{
		virtual bool empty() const = 0 ;
		virtual shared_ptr<const ITemplateDB> GetChild( mui_argstr ) const = 0;
		virtual shared_ptr<const ITemplateDB> GetParent() const = 0;
		virtual shared_ptr<const ITemplateDB> GetRoot() const = 0;
		virtual UINT as_UINT() const = 0;
		virtual mui_string as_mui_string() const = 0;

		virtual shared_ptr<ITemplateIterator> first_iter() const = 0;
	};

	inline UINT OptionUintValue( arg_sptr_c<ITemplateDB>::type pDB, const char* key, UINT defval )
	{
		shared_ptr<const ITemplateDB> pChild = pDB->GetChild( key );
		if( pChild->empty() )
		{
			return defval;
		}
		return pChild->as_UINT();
	}
};
