#include "StdAfx_ui.h"
#include "IFormThread.hpp"
#include "IUiApp.h"
#include <sdk/str_format.h>
//////////////////////////////////////////////////////////////////////////
namespace mui
{
	shared_ptr<IFormThread> GetCurrentFormThread()
	{
		UINT TID = GetCurrentThreadId();
		shared_ptr<IUiApp> pApp = mui::GetGlobalApp();
		if( !pApp )
		{
			//throw except_app_exit("app exit");
			return nullptr;
		}
		shared_ptr<IFormThread> pThread = pApp->GetFormThread( TID );
		if( !pThread )
		{
			pThread.reset( new CFormThread( TID ) );
			mui::GetGlobalApp()->InsertFormThread( pThread );
		}
		return pThread;
	}

	shared_ptr<IControl> CreateControl_FromTemplate(
		arg_sptr<IFormThread>::type pThread,
		arg_sptr_c<ITemplateDB>::type pTemp,
		arg_sptr<IControlPack>::type _pParent
	)
	{
		dbg_assert( GetCurrentThreadId() == pThread->GetThreadId() );

		if( !pTemp || pTemp->empty() )
		{
			return nil_control;
		}
		mui_string classname = pTemp->GetChild( "class" )->as_mui_string();
		if( classname.empty() )
		{
			return nil_control;
		}
		_tcslwr( const_cast<TCHAR*>( classname.c_str() ) );

		shared_ptr<IControlPack> pParent = _pParent;
		if( !pParent )
		{
			pParent = dynamic_pointer_cast<IControlPack>( pThread );
		}
		dbg_assert( pParent );
		if( !pParent )
		{
			throw std::runtime_error( "not parent" );
		}

		shared_ptr<const RegContext> pctx = mui::GetGlobalApp()->GetRegisterContext( classname );
		if( !pctx )
		{
			throw std::runtime_error( std::string( "Not register class " ) + classname );
		}
		shared_ptr<IControl> pControl = pctx->pfn_CreateFrom_Temp( pTemp, pParent );
		if( !pControl )
		{
			UINT nID = pTemp->GetChild( "id" )->as_UINT();
			std::string ec = str_format( "Create [%d] %s Control Error!", nID, classname.c_str() );
			throw std::logic_error( ec );
		}
		bool ok = pParent->InsertChild( pControl );
		dbg_assert( ok );
		shared_ptr<IControlPack> pCurrentPack = dynamic_pointer_cast<IControlPack>( pControl );

		shared_ptr<const ITemplateDB> pChildList = pTemp->GetChild( "childs" );
		if( !pChildList->empty() )
		{
			if( !pCurrentPack )
			{
				UINT nID = pTemp->GetChild( "id" )->as_UINT();
				std::string ec = str_format( "this control([%d]%s) not support child control", nID, classname.c_str() );
				throw std::runtime_error( ec );
			}
			for( shared_ptr<ITemplateIterator> piter = pChildList->first_iter(); !piter->empty(); piter->move_next() )
			{
				shared_ptr<const ITemplateDB> pNode = piter->get_value();
				shared_ptr<IControl> pNew = CreateControl_FromTemplate( pThread, pNode, pCurrentPack );
				dbg_assert( pCurrentPack->CheckHasChild( pNew ) );
			}
		}
		return pControl;
	}
};
//////////////////////////////////////////////////////////////////////////
CFormThread::CFormThread( UINT TID ) : m_TID( TID )
{

}
CFormThread::~CFormThread()
{
#if _MUI_DESTROY_CHECK
	dbg_assert( m_priv_maps.empty() );
#endif
	m_priv_maps.clear();
}


UINT CFormThread::GetThreadId()
{
	return m_TID;
}

void CFormThread::DoModal( BOOL bWaltAll )
{
	MSG msg = {0};
	while( TControlPackImpl<IControlPack>::GetChildSize() > 0 )
	{
		if( !GetMessage( &msg, 0, 0, 0 ) )
		{
			if( !bWaltAll )
			{
				break;
			}
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		TControlPackImpl<IControlPack>::Modal();
	}
}

bool CFormThread::InsertMap( PRIV_ID id, weak_ptr<IBaseControl> p )
{
	return m_priv_maps.insert( std::make_pair( id, p ) ).second;
}
bool CFormThread::RemoveMap( PRIV_ID id )
{
	return m_priv_maps.erase( id ) == 1;
}

shared_ptr<IBaseControl> CFormThread::GetMap( PRIV_ID id )
{
	return m_priv_maps[id].lock();
}

