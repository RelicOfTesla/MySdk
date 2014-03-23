#pragma once

#include "config.h"

namespace mui
{
	struct ITemplateDB;
	struct IFormThread;
	struct IControlPack;
};

namespace mui
{
	struct RegContext
	{
		std::string register_name;
		std::function< shared_ptr<IControl>( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent ) > pfn_CreateFrom_Temp;
	};

	struct IUiApp
	{
		virtual void RegisterClass( arg_sptr_c<RegContext>::type ) = 0;
		virtual shared_ptr<const RegContext> GetRegisterContext( mui_argstr ) const = 0;

		virtual shared_ptr<IFormThread> GetFormThread( UINT ) = 0;
		virtual void InsertFormThread( arg_sptr<IFormThread>::type ) = 0;
	};



	template<typename TList>
	static void Register_ContextList( arg_sptr<IUiApp>::type pApp, const TList& vlist )
	{
		typedef typename TList::const_iterator const_iterator;
		for( const_iterator it = vlist.begin(); it != vlist.end(); ++it )
		{
			pApp->RegisterClass( *it );
		}
	}
};

namespace mui
{
	shared_ptr<IUiApp> GetGlobalApp();
};

namespace mui
{
	void Register_DefaultLib( arg_sptr<IUiApp>::type );


};