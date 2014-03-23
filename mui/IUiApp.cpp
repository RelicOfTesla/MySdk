#include "StdAfx_ui.h"
#include "IUiApp.hpp"
#include "IFormThread.h"
//////////////////////////////////////////////////////////////////////////
void CUiApp::RegisterClass( arg_sptr_c<RegContext>::type pctx )
{
	mui_string ls = pctx->register_name;
	_tcslwr( const_cast<TCHAR*>( ls.c_str() ) );
	m_regmap[ls] = pctx;
}

shared_ptr<const RegContext> CUiApp::GetRegisterContext( mui_argstr s ) const
{
	mui_string ls = s;
	_tcslwr( const_cast<TCHAR*>( ls.c_str() ) );
	auto it = m_regmap.find(ls);
	if (it != m_regmap.end())
	{
		return it->second;
	}
	return shared_ptr<const RegContext>();
}

shared_ptr<IFormThread> CUiApp::GetFormThread( UINT TID )
{
	return m_threadmap[TID];
}

void CUiApp::InsertFormThread( arg_sptr<IFormThread>::type p )
{
	m_threadmap[p->GetThreadId()] = p;
}

//////////////////////////////////////////////////////////////////////////

namespace mui
{
	extern shared_ptr<IUiApp> g_app;

	shared_ptr<IUiApp> g_app( new CUiApp );

	shared_ptr<IUiApp> GetGlobalApp()
	{
		return g_app;
	}

	void CleanGlobalApp()
	{
		g_app.reset();
	}
};

#include <array>

namespace mui
{
	std::list< shared_ptr<RegContext> > GetRegContextList_BaseLib();

	void Register_DefaultLib( arg_sptr<IUiApp>::type pApp )
	{
		Register_ContextList( pApp, GetRegContextList_BaseLib() );
	}
}

