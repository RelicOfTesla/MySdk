#include "StdAfx_ui.h"
#include "IControl.hpp"
#include "TemplateProp.h"
#include "WinControlImpl.h"
#include "mui_util.hpp"
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

void ModifyStyle( HWND hWnd, UINT RemoveStyle, UINT AddStyle )
{
	UINT Style = GetWindowLong( hWnd, GWL_STYLE );
	Style |= AddStyle;
	Style &= ~RemoveStyle;
	SetWindowLong( hWnd, GWL_STYLE, Style );
}
void ModifyStyleEx( HWND hWnd, UINT RemoveStyle, UINT AddStyle )
{
	UINT Style = GetWindowLong( hWnd, GWL_EXSTYLE );
	Style |= AddStyle;
	Style &= ~RemoveStyle;
	SetWindowLong( hWnd, GWL_EXSTYLE, Style );
}

namespace mui
{
	//////////////////////////////////////////////////////////////////////////

// 	template<typename T>
// 	shared_ptr<Win::IWinControl> CreateWinControl_t(arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent)
// 	{
// 		return T::Create(pTemp, pParent);
// 	}
	template<typename T>
	shared_ptr<RegContext> GetRegContext_t()
	{
		static shared_ptr<RegContext> p = T::NewContext();
		return p;
	}

	template<typename T>
	shared_ptr<T> CreateWinControl_v2( mui_argstr control_typename, mui_argstr wnd_classname,
									   arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
	{
		shared_ptr<Win::IWinControl> pWnd = Win::CreateWinControlImpl( control_typename, wnd_classname, pTemp, pParent );
		if( pWnd )
		{
			SetWindowPos( pWnd->GetHWND(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_SHOWWINDOW );
			shared_ptr<T> p( new T( pWnd, pTemp ) );
			return p;
		}
		return shared_ptr<T>();

	}

	std::list< shared_ptr<RegContext> > GetRegContextList_BaseLib()
	{
		std::list< shared_ptr<RegContext> > vlist;
		vlist.push_back( GetRegContext_t<Win::CMyForm>() );
		vlist.push_back( GetRegContext_t<Win::CLabel>() );
		vlist.push_back( GetRegContext_t<Win::CEdit>() );
		vlist.push_back( GetRegContext_t<Win::CButton>() );
		vlist.push_back( GetRegContext_t<Win::CCheck>() );
		vlist.push_back( GetRegContext_t<Win::CColorButton>() );
		vlist.push_back( GetRegContext_t<Win::CWndControl>() );
		vlist.push_back( GetRegContext_t<Win::CListReport>() );

		return vlist;
	}

};


namespace mui
{
	namespace Win
	{

		mui_string CMyForm::_RegName = TEXT( "myform" );
		shared_ptr<RegContext> CMyForm::NewContext()
		{
			static bool isinit = false;
			if( !isinit )
			{
				WNDCLASS wndcls = {0};
				wndcls.lpszClassName = _WndClsName.c_str();
				wndcls.lpfnWndProc = DefWindowProc;
				wndcls.hInstance = GetModuleHandle( 0 );
				wndcls.hCursor = LoadCursor( 0, IDC_ARROW );
				wndcls.hbrBackground = ( HBRUSH )COLOR_WINDOW;
				if( !RegisterClass( &wndcls ) )
				{
					return shared_ptr<RegContext>();
				}

				isinit = true;
			}

			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CMyForm::Create;

			return pctx;
		}

		mui_string CMyForm::_WndClsName = TEXT( "mui-form" );
		shared_ptr<CMyForm> CMyForm::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CreateWinControl_v2<CMyForm>( _RegName, _WndClsName, pTemp, pParent );
		}

		CMyForm::CMyForm( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB)
			: TWinControlProxy< TControlPackImpl<IMyForm> >( p )
		{
			m_wndproc = SetWndProc( boost::bind( &CMyForm::OnMsg, this, _1, _2, _3 ) );

			m_cBkColor = pDB->GetChild( "bk_color" )->as_UINT();
			m_hBkBrush = CreateSolidBrush( m_cBkColor );
		}

		LONG CMyForm::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{

			if( uMsg == WM_ERASEBKGND )
			{
				HDC hDC = ( HDC )wParam;

				RECT bk_rect;
				SIZE sz = GetWindowSize();
				bk_rect.top = bk_rect.left = 0;
				bk_rect.right = sz.cx;
				bk_rect.bottom = sz.cy;

				FillRect( hDC, &bk_rect, m_hBkBrush );
				return 1;
			}
			else if( uMsg == WM_CLOSE )
			{
				this->event_OnClose();
			}
			else if( uMsg == WM_TIMER )
			{
				UINT nID = ( UINT )wParam;
				UINT nSec = nID;

				fnlist_ptr& pfnlist = m_timer_fnlist[nSec];
				( *pfnlist )();
			}
			return m_wndproc( uMsg, wParam, lParam );
		}

		IMyForm::timer_conn CMyForm::AddTimer( UINT nSec, lpfn_onTimer pfn )
		{
			auto it = m_timer_fnlist.find( nSec );
			if( it == m_timer_fnlist.end() )
			{
				fnlist_ptr pfnlist( new signals::signal<void()> );
				it = m_timer_fnlist.insert( std::make_pair( nSec, pfnlist ) ).first;
			}
			fnlist_ptr& pfnlist = it->second;
			timer_conn conn = pfnlist->connect( pfn );
			if( m_timer_fnlist.count( nSec ) == 1 )
			{
				UINT nID = nSec;
				::SetTimer( m_impl->GetHWND(), nID, nSec, 0 );
			}
			return conn;
		}

	}
};

namespace mui
{
	namespace Win
	{
		mui_string CLabel::_RegName = TEXT( "Label" );

		LONG WINAPI MyForm_ClssProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			if( uMsg == WM_NCDESTROY )
			{
				PostQuitMessage( 0 );
			}
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}

		shared_ptr<RegContext> CLabel::NewContext()
		{
			static bool isinit = false;
			if( !isinit )
			{
				WNDCLASS wndcls = {0};
				wndcls.lpszClassName = _WndClsName.c_str();
				wndcls.lpfnWndProc = MyForm_ClssProc;
				wndcls.hInstance = GetModuleHandle( 0 );
				wndcls.hCursor = LoadCursor( 0, IDC_ARROW );
				wndcls.hbrBackground = ( HBRUSH )COLOR_WINDOW;
				if( !RegisterClass( &wndcls ) )
				{
					return shared_ptr<RegContext>();
				}

				isinit = true;
			}

			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CLabel::Create;

			return pctx;
		}

		mui_string CLabel::_WndClsName = TEXT( "mui-label" );
		shared_ptr<CLabel> CLabel::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CreateWinControl_v2<CLabel>( _RegName, _WndClsName, pTemp, pParent );
		}

		CLabel::CLabel( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB )
			: TWinControlProxy<ILabel>( p )
		{
			SIZE sz = p->GetWindowSize();
			m_DrawRect.top = m_DrawRect.left = 0;
			m_DrawRect.right = sz.cx;
			m_DrawRect.bottom = sz.cy;

			m_cBkColor = pDB->GetChild( "bk_color" )->as_UINT();
			m_cTextColor = OptionUintValue( pDB, "text_color", RGB( 0, 0, 0 ) );
			m_wndproc = SetWndProc( boost::bind( &CLabel::OnMsg, this, _1, _2, _3 ) );
		}
		void CLabel::SetColor( COLORREF text, COLORREF bkcolor )
		{
			bool changed = false;
			if( m_cBkColor != bkcolor )
			{
				m_cBkColor = bkcolor;
				changed = true;
			}
			if( m_cTextColor != text )
			{
				m_cTextColor = text;
				changed = true;
			}
			if( changed )
			{
				InvalidateRect( m_impl->GetHWND(), nullptr, FALSE );
			}
		}
		void CLabel::GetColor( COLORREF& text, COLORREF& bkcolor )
		{
			text = m_cTextColor;
			bkcolor = m_cBkColor;
		}

		LONG CLabel::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			if( uMsg == WM_PAINT )
			{
				HWND hWnd = m_impl->GetHWND();
				UINT pResult = m_wndproc( uMsg, wParam, lParam );
				//PAINTSTRUCT ps = {0};
				//BeginPaint(hWnd, &ps);
				HDC hDC = GetDC( hWnd );

				UINT len = GetWindowTextLength( hWnd ) + 1;
				std::string str;
				str.resize( len );
				len = GetWindowText( hWnd, &str[0], len );
				str.resize( len );

				HBRUSH hBkBrush = CreateSolidBrush( m_cBkColor );
				HGDIOBJ hOldBkBrush = SelectObject( hDC, hBkBrush );
				HFONT hFont = ( HFONT )::SendMessage( hWnd, WM_GETFONT, 0, 0 );
				HGDIOBJ hOldFont = SelectObject( hDC, hFont );

				{
					RECT bk_rect;
					SIZE sz = GetWindowSize();
					bk_rect.top = bk_rect.left = 0;
					bk_rect.right = sz.cx;
					bk_rect.bottom = sz.cy;

					FillRect( hDC, &bk_rect, hBkBrush );
				}
				SetBkColor( hDC, m_cBkColor );
				SetTextColor( hDC, m_cTextColor );
				SetBkMode( hDC, TRANSPARENT );
				UINT dt_style = DT_VCENTER | DT_SINGLELINE;
				ULONG control_style = GetWindowLong( hWnd, GWL_STYLE );
				if( control_style & 1 )
				{
					dt_style |= DT_CENTER;
				}
				if( control_style & 2 )
				{
					dt_style |= DT_RIGHT;
				}
				DrawText( hDC, str.c_str(), str.size(), &m_DrawRect, dt_style );

				SelectObject( hDC, hOldFont );
				SelectObject( hDC, hBkBrush );

				DeleteObject( hBkBrush );

				ReleaseDC( hWnd, hDC );
				return pResult;
			}
			else if( uMsg == WM_SETTEXT )
			{
				LONG r = m_wndproc( uMsg, wParam, lParam );
				InvalidateRect( m_impl->GetHWND(), 0, FALSE );
				return r;
			}
			return m_wndproc( uMsg, wParam, lParam );
		}
	}
};
namespace mui
{
	namespace Win
	{

		mui_string CEdit::_RegName = TEXT( "Edit" );
		shared_ptr<RegContext> CEdit::NewContext()
		{
			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CEdit::Create;
			return pctx;
		}

		mui_string CEdit::_WndClsName = TEXT( "Edit" );
		shared_ptr<CEdit> CEdit::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CreateWinControl_v2<CEdit>( _RegName, _WndClsName, pTemp, pParent );
		}

		CEdit::CEdit( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB)
			: TWinControlProxy<IEdit>( p )
		{
			m_cTextColor = OptionUintValue( pDB, "text_color", RGB( 0, 0, 0 ) );
			m_cBkColor = OptionUintValue( pDB, "bk_color", RGB( 0xFF, 0xFF, 0xFF ) );
			m_hBkBrush = CreateSolidBrush( m_cBkColor );
			m_wndproc = SetWndProc( boost::bind( &CEdit::OnMsg, this, _1, _2, _3 ) );
		}
		CEdit::~CEdit()
		{
			DeleteObject( m_hBkBrush );
		}

		LONG CEdit::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			switch( uMsg )
			{
			case WM_CTLCOLORSTATIC:
			case WM_CTLCOLOREDIT:
				{
					HWND hChild = ( HWND )lParam;
					if( hChild == m_impl->GetHWND() )
					{
						HDC hDC = ( HDC )wParam;
						SetTextColor( hDC, m_cTextColor );
						SetBkColor( hDC, m_cBkColor );

						return ( LONG )m_hBkBrush;
					}

				}
				break;
			}
			return m_wndproc( uMsg, wParam, lParam );
		}

		void CEdit::SetColor( COLORREF text, COLORREF bkcolor )
		{
			bool changed = false;
			if( m_cBkColor != bkcolor )
			{
				DeleteObject( m_hBkBrush );
				m_hBkBrush = CreateSolidBrush( bkcolor );

				m_cBkColor = bkcolor;
				changed = true;
			}
			if( m_cTextColor != text )
			{
				m_cTextColor = text;
				changed = true;
			}
			if( changed )
			{
				InvalidateRect( m_impl->GetHWND(), nullptr, FALSE );
			}
		}
		void CEdit::GetColor( COLORREF& text, COLORREF& bkcolor )
		{
			text = m_cTextColor;
			bkcolor = m_cBkColor;
		}
	}
};

namespace mui
{
	namespace Win
	{
		mui_string CButton::_RegName = TEXT( "Button" );
		shared_ptr<RegContext> CButton::NewContext()
		{
			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CButton::Create;
			return pctx;
		}

		mui_string CButton::_WndClsName = "Button";
		shared_ptr<CButton> CButton::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CreateWinControl_v2<CButton>( _RegName, _WndClsName, pTemp, pParent );
		}

		CButton::CButton( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type )
			: TWinControlProxy<IButton>( p )
		{
			m_wndproc = SetWndProc( boost::bind( &CButton::OnMsg, this, _1, _2, _3 ) );
		}

		LONG CButton::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			if( uMsg == WM_COMMAND && ( HWND )lParam == m_impl->GetHWND() )
			{
				USHORT nCode = HIWORD( wParam );
				if( nCode == BN_CLICKED )
				{
					this->event_OnClicked();
				}
			}
			return m_wndproc( uMsg, wParam, lParam );
		}
	}
};

namespace mui
{
	namespace Win
	{
		mui_string CCheck::_RegName = TEXT( "Check" );
		shared_ptr<RegContext> CCheck::NewContext()
		{
			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CCheck::Create;
			return pctx;
		}

		mui_string CCheck::_WndClsName = "Button";
		shared_ptr<CCheck> CCheck::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CreateWinControl_v2<CCheck>( _RegName, _WndClsName, pTemp, pParent );
		}

		CCheck::CCheck( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB )
			: TWinControlProxy<ICheck>( p )
		{
			ModifyStyle( p->GetHWND(), 0, BS_AUTOCHECKBOX );
			m_wndproc = SetWndProc( boost::bind( &CCheck::OnMsg, this, _1, _2, _3 ) );

			m_cTextColor = OptionUintValue( pDB, "text_color", RGB( 0, 0, 0 ) );
			m_cBkColor = OptionUintValue( pDB, "bk_color", RGB( 0xFF, 0xFF, 0xFF ) );
			m_hBkBrush = CreateSolidBrush( m_cBkColor );
		}

		LONG CCheck::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			switch( uMsg )
			{
			case WM_COMMAND:
				if( ( HWND )lParam == m_impl->GetHWND() )
				{
					USHORT nCode = HIWORD( wParam );
					if( nCode == BN_CLICKED )
					{
						this->event_OnChanged( GetCheck() );
					}

				}
				break;
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORSTATIC:
				{
					HWND hChild = ( HWND )lParam;
					if( hChild == m_impl->GetHWND() )
					{
						HDC hDC = ( HDC )wParam;
						SetTextColor( hDC, m_cTextColor );
						SetBkColor( hDC, m_cBkColor );

						return ( LONG )m_hBkBrush;
					}
				}
				break;
			}
			return m_wndproc( uMsg, wParam, lParam );
		}

		bool CCheck::GetCheck()const
		{
			return m_impl->SendMessage( BM_GETCHECK, 0, 0 ) ? true : false;
		}
		void CCheck::SetCheck( bool bChk )
		{
			m_impl->SendMessage( BM_SETCHECK, bChk ? BST_CHECKED : BST_UNCHECKED, 0 );
		}
	}
};

namespace mui
{
	namespace Win
	{
		mui_string CColorButton::_RegName = TEXT( "ColorButton" );
		shared_ptr<CColorButton> CColorButton::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			shared_ptr<CLabel> pLabel = dynamic_pointer_cast<CLabel>( CLabel::Create( pTemp, pParent ) );
			if( pLabel )
			{
				shared_ptr<CColorButton> pColorButton( new CColorButton( pLabel, pTemp ) );
				return pColorButton;
			}
			return shared_ptr<CColorButton>();
		}

		shared_ptr<RegContext> CColorButton::NewContext()
		{
			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = CColorButton::Create;
			return pctx;
		}

		static COLORREF GetAdjustColor( arg_sptr_c<ITemplateDB>::type pDB, mui_argstr name, COLORREF adj_base,
										double mul_H, double mul_S, double mul_L )
		{
			shared_ptr<const ITemplateDB> child = pDB->GetChild( name );
			if( !child->empty() )
			{
				return child->as_UINT();
			}
			COLOR_HSL hsl = RGBtoHSL( adj_base );
			hsl.hue *= mul_H;
			if( hsl.hue > 360 )
			{
				hsl.hue = 360;
			}
			hsl.saturation *= mul_S;
			if( hsl.saturation > 1 )
			{
				hsl.saturation = 1;
			}
			hsl.luminance *= mul_L;
			if( hsl.luminance > 1 )
			{
				hsl.luminance = 1;
			}
			return HSLtoRGB( hsl );
		}

		CColorButton::CColorButton( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB )
			: TWinControlProxy<IColorButton>( p )
		{
			ModifyStyle( p->GetHWND(), 0, 1 );

			m_bMouseIn = false;
			m_bPressing = false;
			m_label = dynamic_pointer_cast<CLabel>( p );
			if( m_label )
			{
				m_wndproc = SetWndProc( boost::bind( &CColorButton::OnMsg, this, _1, _2, _3 ) );
			}
			m_cNormalTextColor = OptionUintValue( pDB, "text_color", RGB( 0, 0, 0 ) );
			m_cNormalBkColor = OptionUintValue( pDB, "bk_color", RGB( 230, 230, 230 ) );
			m_cHoverTextColor = GetAdjustColor( pDB, "hover_text_color", m_cNormalTextColor, 1.1, 1.1, 1.1 );
			m_cHoverBkColor = GetAdjustColor( pDB, "hover_bk_color", m_cNormalBkColor, 1.1, 1.1, 1.1 );
			m_cDisableTextColor = GetAdjustColor( pDB, "disable_text_color", m_cNormalTextColor, 1, 0.8, 0.8 );
			m_cDisableBkColor = GetAdjustColor( pDB, "disable_bk_color", m_cNormalBkColor, 1, 0.8, 0.8 );
			m_label->SetColor(m_cNormalTextColor, m_cNormalBkColor);
		}

		LONG CColorButton::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			switch( uMsg )
			{
			case WM_LBUTTONDOWN:
				m_label->m_DrawRect.top = m_label->m_DrawRect.left = 4;
				InvalidateRect( m_label->GetHWND(), 0, FALSE );
				m_bPressing = true;
				break;
			case WM_LBUTTONUP:
				m_label->m_DrawRect.top = m_label->m_DrawRect.left = 0;
				InvalidateRect( m_label->GetHWND(), 0, FALSE );
				if( m_bPressing )
				{
					m_bPressing = false;
					this->event_OnClicked();
				}
				m_bPressing = false;
				break;
			case WM_MOUSELEAVE:
				m_label->m_DrawRect.top = m_label->m_DrawRect.left = 0;
				if( m_bMouseIn )
				{
					m_label->SetColor( m_cNormalTextColor, m_cNormalBkColor );
					m_bMouseIn = false;
				}
				m_bPressing = false;
				InvalidateRect( m_label->GetHWND(), 0, FALSE );
				break;
			case WM_MOUSEMOVE:
				{
					if( !m_bMouseIn )
					{
						HWND hLabel = m_label->GetHWND();

						m_bMouseIn = true;
						TRACKMOUSEEVENT et = {0};
						et.cbSize = sizeof( et );
						et.hwndTrack = hLabel;
						et.dwFlags = TME_LEAVE;
						TrackMouseEvent( &et );

						if( shared_ptr<IWinControlImpl> pImpl = dynamic_pointer_cast<IWinControlImpl>( m_label->m_impl ) )
						{
							m_label->SetColor( m_cHoverTextColor, m_cHoverBkColor );
							InvalidateRect( m_label->GetHWND(), 0, FALSE );
						}
					}
				}
				break;
			case WM_ENABLE:
				{
					bool bEnable = BOOL( wParam ) ? true : false;
					if( bEnable )
					{
						m_label->SetColor( m_cNormalTextColor, m_cNormalBkColor );
					}
					else
					{
						m_label->SetColor( m_cDisableTextColor, m_cDisableBkColor );
					}
					InvalidateRect( m_label->GetHWND(), 0, FALSE );
				}
				break;
			case WM_PAINT:
				{
					LONG result = m_wndproc( uMsg, wParam, lParam );
					if( m_bMouseIn )
					{
						HWND hLabel = m_label->GetHWND();
						HDC hDC = GetDC( hLabel );
						RECT rect;
						const int diff = 0;
						rect.left = rect.top = diff;
						SIZE sz = m_label->GetWindowSize();
						rect.right = sz.cx - diff;
						rect.bottom = sz.cy - diff;
						FrameRect( hDC, &rect, ( HBRUSH )GetStockObject( BLACK_BRUSH ) );
						ReleaseDC( hLabel, hDC );
					}
				}
			}
			return m_wndproc( uMsg, wParam, lParam );
		}
		void CColorButton::SetColor( COLORREF text, COLORREF bkcolor, COLORREF hover_text, COLORREF hover_bk )
		{
			m_cNormalTextColor = text;
			m_cNormalBkColor = bkcolor;
			m_cHoverTextColor = hover_text;
			m_cHoverBkColor = hover_bk;
		}
	}
};


namespace mui
{
	namespace Win
	{
		mui_string CWndControl::_RegName = TEXT( "WndControl" );
		shared_ptr<RegContext> CWndControl::NewContext()
		{
			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CWndControl::Create;

			static bool isinit = false;
			if( !isinit )
			{
				isinit = true;
				InitCommonControls();
			}
			return pctx;
		}
		shared_ptr<CWndControl> CWndControl::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			mui_string WndClsName = pTemp->GetChild( "WndName" )->as_mui_string();
			return CreateWinControl_v2<CWndControl>( _RegName, WndClsName, pTemp, pParent );
		}

		CWndControl::CWndControl( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB )
			: TWinControlProxy<IWinControl>( p )
		{
			shared_ptr<const ITemplateDB> pnode = pDB->GetChild( "text_color" );
			if( !pnode->empty() )
			{
				COLORREF color = pnode->as_UINT();
				m_pTextColor.reset( new COLORREF( color ) );
			}
			pnode = pDB->GetChild( "bk_color" );
			if( !pnode->empty() )
			{
				COLORREF color = pnode->as_UINT();
				m_pBkColor.reset( new COLORREF( color ) );
				m_hBkBrush = CreateSolidBrush( color );
			}

			if( m_pTextColor || m_pBkColor )
			{
				m_wndproc = SetWndProc( boost::bind( &CWndControl::OnMsg, this, _1, _2, _3 ) );
			}
			m_erase_bk = OptionUintValue( pDB, "erase_bk", FALSE );
		}

		LONG CWndControl::OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			switch( uMsg )
			{
			case WM_ERASEBKGND:
				{
					if( m_pBkColor && m_erase_bk )
					{
						HDC hDC = ( HDC )wParam;

						RECT bk_rect;
						SIZE sz = GetWindowSize();
						bk_rect.top = bk_rect.left = 0;
						bk_rect.right = sz.cx;
						bk_rect.bottom = sz.cy;

						FillRect( hDC, &bk_rect, m_hBkBrush );
						return 1;
					}
				}
				break;
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
				{
					HWND hChild = ( HWND )lParam;
					if( hChild == m_impl->GetHWND() )
					{
						HDC hDC = ( HDC )wParam;
						if( m_pTextColor )
						{
							SetTextColor( hDC, *m_pTextColor );
						}
						if( m_pBkColor )
						{
							SetBkColor( hDC, *m_pBkColor );
							return ( LONG )m_hBkBrush;
						}

					}
				}
				break;
			}
			return m_wndproc( uMsg, wParam, lParam );
		}

	}
};

namespace mui
{
	namespace Win
	{
		mui_string CListReport::_RegName = TEXT( "ListReport" );
		shared_ptr<RegContext> CListReport::NewContext()
		{
			shared_ptr<RegContext> pctx( new RegContext );
			pctx->register_name = _RegName;
			pctx->pfn_CreateFrom_Temp = &CListReport::Create;

			static bool isinit = false;
			if( !isinit )
			{
				isinit = true;
				InitCommonControls();
			}
			return pctx;
		}
		mui_string CListReport::_WndClsName = TEXT( "SysListView32" );

		shared_ptr<CListReport> CListReport::Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent )
		{
			return CreateWinControl_v2<CListReport>( _RegName, _WndClsName, pTemp, pParent );
		}

		CListReport::CListReport( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type pDB )
			: TWinControlProxy<IListReport>( p )
		{
			ModifyStyle( GetHWND(), 0, LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER );
			SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT );
		}

		int CListReport::AppendColumn( mui_argstr title, int nWidth )
		{
			int nSubItem = 999;// GetColumeCount() + 1;

			LVCOLUMN column = {0};
			column.mask = LVCF_TEXT;
			column.pszText = ( LPTSTR )title.c_str();
			if( nWidth != -1 )
			{
				column.mask |= LVCF_WIDTH;
				column.cx = nWidth;
			}
			if( nSubItem != -1 )
			{
				column.mask |= LVCF_SUBITEM;
				column.iSubItem = nSubItem;
			}
			return SendMessage( LVM_INSERTCOLUMN, nSubItem, ( LPARAM )&column );

		}
		int CListReport::AppendItem()
		{
			int nItem = GetItemCount() + 1;

			LVITEM item = {0};
			item.iItem = nItem;
			return SendMessage( LVM_INSERTITEM, 0, ( LPARAM )&item );
		}
		void CListReport::SetItemText( int nItem, int nCol, mui_argstr s )
		{
			LVITEM item = {0};
			item.mask = LVIF_TEXT;
			item.iItem = nItem;
			item.iSubItem = nCol;
			item.pszText = const_cast<char*>( s.c_str() );
			BOOL ok = SendMessage( LVM_SETITEM, 0, ( LPARAM )&item );
			dbg_assert( ok );
		}
		void CListReport::SetItemData( int nItem, int data )
		{
			LVITEM item = {0};
			item.mask = LVIF_PARAM;
			item.iItem = nItem;
			item.lParam = data;
			BOOL ok = SendMessage( LVM_SETITEM, 0, ( LPARAM )&item );
			dbg_assert( ok );
		}
		size_t CListReport::GetItemCount()const
		{
			return SendMessage( LVM_GETITEMCOUNT, 0, 0 );
		}
		void CListReport::DeleteAllItems()
		{
			BOOL ok = SendMessage( LVM_DELETEALLITEMS, 0, 0 );
			dbg_assert( ok );
		}
		int CListReport::GetItemData( int nItem )const
		{
			LVITEM item = {0};
			item.mask = LVIF_PARAM;
			item.iItem = nItem;
			BOOL ok = SendMessage( LVM_GETITEM, 0, ( LPARAM )&item );
			dbg_assert( ok );
			return item.lParam;
		}
		int CListReport::GetFirstSelect()const
		{
			return SendMessage( TVM_GETNEXTITEM, LVIS_SELECTED, ( LPARAM )( -1 ) );
		}
		int CListReport::FindItem_FromData( int data )const
		{
			LVFINDINFO fi = {0};
			fi.lParam = data;
			fi.flags = LVFI_PARAM;
			int nStart = -1;
			return SendMessage( LVM_FINDITEM, nStart, ( LPARAM )&fi );
		}
	};
};



//////////////////////////////////////////////////////////////////////////
