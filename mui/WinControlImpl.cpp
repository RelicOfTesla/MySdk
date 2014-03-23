#include "StdAfx_ui.h"
#include "WinControlImpl.hpp"
#include "IFormThread.h"


LOGFONT GetSystemLogFont()
{
	static LOGFONT GlobalFont = {0};
	static bool isinit = false;
	if( !isinit )
	{
		static HFONT hDesktopFont = ( HFONT )GetStockObject( DEFAULT_GUI_FONT );
		GetObject( hDesktopFont, sizeof( GlobalFont ), &GlobalFont );
		isinit = true;
	}
	return GlobalFont;
}
bool SetFontSize( LOGFONT& lFont, size_t nFontSize )
{
	if( nFontSize > 0 )
	{
		static UINT LogPixelsy = 0;
		if( !LogPixelsy )
		{
			HDC hDC = GetDC( 0 );
			LogPixelsy = GetDeviceCaps( hDC, LOGPIXELSY );
			ReleaseDC( 0, hDC );
		}
		lFont.lfHeight = -MulDiv( nFontSize, LogPixelsy, 72 );
		return true;
	}
	return false;
}


namespace mui
{
	shared_ptr<const ITemplateDB> GetTemplateNode( arg_sptr_c<ITemplateDB>::type pSrc, mui_argstr name, mui_argstr classname )
	{
		if( shared_ptr<const ITemplateDB> p = pSrc->GetChild( name ) )
		{
			if( !p->empty() )
			{
				return p;
			}
		}

		if( shared_ptr<const ITemplateDB> p = pSrc->GetRoot()->GetChild( "template" )->GetChild( classname )->GetChild( name ) )
		{
			if( !p->empty() )
			{
				return p;
			}
		}
		for( shared_ptr<const ITemplateDB> pCur = pSrc->GetParent(); !pCur->empty(); pCur = pCur->GetParent() )
		{
			shared_ptr<const ITemplateDB> p = pCur->GetChild( name );
			if( !p->empty() )
			{
				return p;
			}
		}
		return pSrc->GetChild( "" );
	}

	struct CTemplateFinder
	{
		CTemplateFinder( arg_sptr_c<ITemplateDB>::type pSrc, mui_argstr classname )
			: m_pDB( pSrc ), m_classname( classname )
		{
		}

		shared_ptr<const ITemplateDB> Get( mui_argstr name )const
		{
			return GetTemplateNode( m_pDB, name, m_classname );
		}

		const shared_ptr<const ITemplateDB> m_pDB;
		mui_argstr m_classname;
	};


};

namespace mui
{
	struct WinControl_InitProp
	{
		size_t x, y, cx, cy;
		mui_string title;
		DWORD ExStyle, Style;
		bool reset_style;
	};

	WinControl_InitProp LoadInitProp_FromTemplate( mui_argstr _control_typename, arg_sptr_c<ITemplateDB>::type pTemp )
	{
		WinControl_InitProp result;

		std::string control_typename = _control_typename;
		strlwr( ( char*)control_typename.c_str() );
		CTemplateFinder finder( pTemp, control_typename );

		result.x = pTemp->GetChild( "x" )->as_UINT();
		result.y = pTemp->GetChild( "y" )->as_UINT();
		result.cx = pTemp->GetChild( "cx" )->as_UINT();
		result.cy = pTemp->GetChild( "cy" )->as_UINT();
		result.title = pTemp->GetChild( "title" )->as_mui_string();

		result.ExStyle = finder.Get( "ex_style" )->as_UINT();
		result.Style = finder.Get( "style" )->as_UINT();
		result.reset_style = finder.Get( "reset_style" )->as_UINT() ? true : false;

		return result;
	};

	//////////////////////////////////////////////////////////////////////////

	namespace Win
	{


		CWinControlImplBase::CWinControlImplBase()
		{
			m_hWnd = 0;
			m_raw_wnd_proc = nullptr;
		}
		CWinControlImplBase::~CWinControlImplBase()
		{
#if _MUI_DESTROY_CHECK
			dbg_assert( m_raw_wnd_proc == nullptr );
			dbg_assert( m_hWnd == 0 );
#endif
		}

		LONG SetWindowLongT( HWND hWnd, UINT nIndex, LONG NewValue )
		{
			if( IsWindowUnicode( hWnd ) )
			{
				// in "SysListView32" control
				return SetWindowLongW( hWnd, nIndex, NewValue );
			}
			else
			{
				return SetWindowLongA( hWnd, nIndex, NewValue );
			}
		}

		void CWinControlImplBase::initialize( mui_argstr control_typename, HWND hWnd, arg_sptr_c<ITemplateDB>::type )
		{
			m_hWnd = hWnd;
			mui::GetCurrentFormThread()->InsertMap( ( IFormThread::PRIV_ID )m_hWnd, shared_from_this() );
			m_user_wndproc = boost::bind( &CWinControlImplBase::BottomWndProc, this, _1, _2, _3 );
			m_raw_wnd_proc = ( WNDPROC )SetWindowLongT( hWnd, GWL_WNDPROC, ( LONG )&CWinControlImplBase::_WndProc );
		}
		void CWinControlImplBase::uninitialize()
		{
			SetWindowLongT( m_hWnd, GWL_WNDPROC, ( LONG )m_raw_wnd_proc );
			m_raw_wnd_proc = nullptr;

			mui::GetCurrentFormThread()->RemoveMap( ( IFormThread::PRIV_ID )m_hWnd );
			m_hWnd = 0;
		}
		bool CWinControlImplBase::is_invalid()const
		{
			return m_hWnd == 0 || !IsWindow(m_hWnd);
		}
		LONG CALLBACK CWinControlImplBase::_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			try
			{
				auto pThread = mui::GetCurrentFormThread();
				if( pThread )
				{
					shared_ptr<IBaseControl> rp = pThread->GetMap( ( UINT )hWnd );
					if( shared_ptr<CWinControlImplBase> p = dynamic_pointer_cast<CWinControlImplBase>( rp ) )
					{
						return p->m_user_wndproc( uMsg, wParam, lParam );
					}
				}
			}
			catch( const std::exception&)
			{
				static int last_tick = GetTickCount();
				static int exception_count = 0;
				if (GetTickCount() - last_tick > 5 * 1000)
				{
					exception_count = 0;
				}
				if (++exception_count < 10)
				{
					DefWindowProc( hWnd, uMsg, wParam, lParam );
				}
				//dbg_assert(0);
				throw;
			}
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}

		Win::IWinControl::slot_wndproc CWinControlImplBase::SetWndProc( slot_wndproc new_proc )
		{
			dbg_assert( new_proc.tracked_count() > 0 );
			slot_wndproc old = m_user_wndproc;
			m_user_wndproc = new_proc;
			return old;
		}
		LONG CWinControlImplBase::BottomWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			if( uMsg == WM_DESTROY )
			{
				LONG ret = m_raw_wnd_proc( m_hWnd, uMsg, wParam, lParam );
				this->uninitialize();
				return ret;
			}
			if( !m_raw_wnd_proc )
			{
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
			return m_raw_wnd_proc( m_hWnd, uMsg, wParam, lParam );
		}
		HWND CWinControlImplBase::GetHWND()const
		{
			return m_hWnd;
		}

		//////////////////////////////////////////////////////////////////////////

		shared_ptr<CWinControlImplEx> CWinControlImplEx::Create( mui_argstr control_typename, mui_argstr wnd_classname,
				arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			auto initprop = LoadInitProp_FromTemplate( control_typename, pTemp );
			HWND hWndParent = 0;
			if( shared_ptr<const IWinControl> pWndParent = dynamic_pointer_cast<const IWinControl>( pParent ) )
			{
				hWndParent = pWndParent->GetHWND();
			}
			initprop.Style |= WS_VISIBLE;
			if( hWndParent )
			{
				initprop.Style |= WS_CHILD;
			}
			if( hWndParent == 0 )
			{
				RECT srect;
				SystemParametersInfo( SPI_GETWORKAREA, 0, &srect, 0 );
				if( initprop.x == 0 )
				{
					initprop.x = ( srect.right - srect.left - initprop.cx ) / 2;
				}
				if( initprop.y == 0 )
				{
					initprop.y = ( srect.bottom - srect.top - initprop.cy ) / 2;
				}
			}
			HWND hWnd = CreateWindowEx( initprop.ExStyle, wnd_classname.c_str(), initprop.title.c_str(), initprop.Style,
										initprop.x, initprop.y, initprop.cx, initprop.cy, hWndParent, 0, 0, 0 );
			UINT ec = GetLastError();
			if( IsWindow( hWnd ) )
			{
				if( initprop.reset_style )
				{
					SetWindowLong( hWnd, GWL_STYLE, initprop.Style );
					SetWindowLong( hWnd, GWL_EXSTYLE, initprop.ExStyle );
				}
				shared_ptr<CWinControlImplEx> p( new CWinControlImplEx() );
				p->initialize( control_typename, hWnd, pTemp );
				return p;
			}
			return shared_ptr<CWinControlImplEx>();
		}

		//////////////////////////////////////////////////////////////////////////
		CWinControlImplEx::CWinControlImplEx()
		{
			m_ID = 0;
			m_hFont = 0;
		}
		CWinControlImplEx::~CWinControlImplEx()
		{
			DeleteObject( m_hFont );
		}

		void CWinControlImplEx::initialize( mui_argstr control_typename, HWND hWnd, arg_sptr_c<ITemplateDB>::type pTemp )
		{
			__super::initialize( control_typename, hWnd, pTemp );

			m_ID = pTemp->GetChild( "id" )->as_UINT();

			m_next_wndproc = SetWndProc( boost::bind( &CWinControlImplEx::OnMsg, this, _1, _2, _3 ) );

			LOGFONT lFont = {0};
			{
				CTemplateFinder finder( pTemp, control_typename );

				lFont = GetSystemLogFont();
				mui_argstr fontname = finder.Get( "font-name" )->as_mui_string();
				if( fontname.size() )
				{
					strncpy_s( lFont.lfFaceName, fontname.c_str(), sizeof( lFont.lfFaceName ) );
				}
				size_t fontsize = finder.Get( "font-size" )->as_UINT();
				SetFontSize( lFont, fontsize );
				bool fontbold = finder.Get( "font-bold" )->as_UINT() ? true : false;
				if( fontbold )
				{
					lFont.lfWeight = 700;
				}
			}
			HFONT hFont = CreateFontIndirect( &lFont );
			::SendMessage( m_hWnd, WM_SETFONT, ( WPARAM )hFont, TRUE );
		}

		LONG CWinControlImplEx::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			switch( uMsg )
			{
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
				{
					HWND hChild = ( HWND )lParam;
					if( hChild != m_hWnd )
					{
						return ::SendMessage( hChild, uMsg, wParam, lParam );
					}
				}
				break;
			case WM_COMMAND:
				{
					HWND hChild = ( HWND )lParam;
					if( hChild != m_hWnd )
					{
						return ::SendMessage( hChild, uMsg, wParam, lParam );
					}
				}
				break;
			case WM_SETFONT:
				DeleteObject( m_hFont );
				m_hFont = ( HFONT )wParam;
				break; // return m_next_wndproc(uMsg, wParam, lParam);
			case WM_GETFONT:
				return ( LONG )m_hFont;
			}
			return m_next_wndproc( uMsg, wParam, lParam );
		}


		IControl::CONTROL_ID CWinControlImplEx::GetID() const
		{
			return m_ID;
		}
		SIZE CWinControlImplEx::GetWindowSize() const
		{
			RECT rect;
			GetWindowRect( m_hWnd, &rect );
			SIZE sz;
			sz.cx = rect.right - rect.left;
			sz.cy = rect.bottom - rect.top;
			return sz;
		}

		shared_ptr<mui_string> CWinControlImplEx::GetTitle() const
		{
			return GetWinText();
		}
		void CWinControlImplEx::SetTitle( mui_argstr str )
		{
			SetWinText( str );
		}

		shared_ptr<mui_string> CWinControlImplEx::GetWinText() const
		{
			UINT len = GetWindowTextLength( m_hWnd ) + 1;
			shared_ptr<mui_string> pstr( new mui_string );
			if( len > 0 )
			{
				pstr->resize( len );
				len = GetWindowText( m_hWnd, const_cast<TCHAR*>( pstr->c_str() ), len );
				pstr->resize( len );
			}
			return pstr;
		}
		void CWinControlImplEx::SetWinText( mui_argstr str )
		{
			SetWindowText( m_hWnd, str.c_str() );
		}

	}
}

namespace mui
{
	namespace Win
	{
		shared_ptr<IWinControlImpl> CreateWinControlImpl( mui_argstr control_typename, mui_argstr wnd_classname,
				arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CWinControlImplEx::Create( control_typename, wnd_classname, pTemp, pParent );
		}
	}
};