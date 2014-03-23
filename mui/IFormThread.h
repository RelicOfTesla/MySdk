#pragma once

#include "config.h"

#include "IControl.h"
#include "TemplateProp.h"

namespace mui
{
	struct IFormThread
	{
		typedef UINT PRIV_ID;

		virtual void DoModal( BOOL bWaltAll ) = 0;
		virtual UINT GetThreadId() = 0;

		virtual bool InsertMap( PRIV_ID id, weak_ptr<IBaseControl> ) = 0;
		virtual bool RemoveMap( PRIV_ID id ) = 0;
		virtual shared_ptr<IBaseControl> GetMap( PRIV_ID ) = 0;

	};

	shared_ptr<IControl> CreateControl_FromTemplate( arg_sptr<IFormThread>::type,
			arg_sptr_c<ITemplateDB>::type,
			arg_sptr<IControlPack>::type parent );

};


namespace mui
{
	struct except_app_exit : std::runtime_error
	{
		except_app_exit( const std::string& s ) : std::runtime_error( s )
		{}
	};
	shared_ptr<IFormThread> GetCurrentFormThread();
};

