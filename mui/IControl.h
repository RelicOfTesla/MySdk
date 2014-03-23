#pragma once

#include "config.h"
#include <sdk/boost_lib/signals.hpp>
#include <sdk/slot_function.hpp>

namespace mui
{
	struct IMuiObject : noncopyable, signals::trackable
	{
	};
	struct IBaseControl : IMuiObject
	{
		virtual ~IBaseControl() {}

		virtual bool is_invalid() const = 0;
	};

	struct IBaseControlPack
	{
		virtual bool InsertChild( arg_sptr<IBaseControl>::type ) = 0;
		virtual bool RemoveChild( arg_sptr<IBaseControl>::type ) = 0;
		virtual bool CheckHasChild( arg_sptr_c<IBaseControl>::type ) const = 0;
		virtual void Modal() = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	struct IControl : IBaseControl
	{
		typedef UINT CONTROL_ID;
		virtual CONTROL_ID GetID() const = 0;
	};
	struct IControlPack : IBaseControlPack
	{
		virtual shared_ptr<IControl> FindChild( IControl::CONTROL_ID nID, bool bChildPack = false ) const = 0;
		virtual void AddExternData( shared_ptr<void> ) = 0;

		//////////////////////////////////////////////////////////////////////////
		template<typename T>
		inline shared_ptr<T> FindChild_t( IControl::CONTROL_ID nID, bool bChildPack = false )const
		{
			return dynamic_pointer_cast<T>( FindChild( nID, bChildPack ) );
		}
		template<typename T>
		inline shared_ptr<T> GetChild_t( IControl::CONTROL_ID nID, bool bChildPack = false ) const
		{
			shared_ptr<T> p = FindChild_t<T>( nID, bChildPack );
			if( !p )
			{
				throw std::runtime_error( "can't find control" );
			}
			return p;
		}

	};

	namespace Win
	{

		struct IWinControl : IControl
		{
			virtual HWND GetHWND() const = 0;
			virtual SIZE GetWindowSize() const = 0;

			virtual shared_ptr<mui_string> GetTitle() const = 0;
			virtual void SetTitle( mui_argstr ) = 0;

			typedef slot_function< LONG( UINT, WPARAM, LPARAM ) > slot_wndproc;
			virtual slot_wndproc SetWndProc( slot_wndproc ) = 0;
			//////////////////////////////////////////////////////////////////////////
			inline LRESULT SendMessage( UINT Msg, WPARAM wParam, LPARAM lParam )const
			{
				return ::SendMessage( GetHWND(), Msg, wParam, lParam );
			}
			inline LRESULT PostMessage( UINT Msg, WPARAM wParam, LPARAM lParam )const
			{
				return ::PostMessage( GetHWND(), Msg, wParam, lParam );
			}

		};


		struct IForm : IWinControl, IControlPack
		{
			signals::signal<void()> event_OnClose;

			//////////////////////////////////////////////////////////////////////////
			static void AsyncClose(shared_ptr<IForm>& p)
			{
				p->PostMessage( WM_CLOSE, 0, 0 );
			}
			void SyncClose()
			{
				SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
				SendMessage(WM_CLOSE, 0, 0);
			}
			//////////////////////////////////////////////////////////////////////////
			inline shared_ptr<mui_string> GetChildText( IControl::CONTROL_ID ID, bool bChildPack = false ) const
			{
				shared_ptr<mui_string> pstr;
				if( shared_ptr<IWinControl> pWnd = FindChild_t<IWinControl>( ID, bChildPack ) )
				{
					pstr = pWnd->GetTitle();
				}
				return pstr;
			}
			inline bool SetChildText( IControl::CONTROL_ID ID, mui_argstr title, bool bChildPack = false ) const
			{
				if( shared_ptr<IWinControl> pWnd = FindChild_t<IWinControl>( ID, bChildPack ) )
				{
					pWnd->SetTitle( title );
					return true;
				}
				return false;
			}
		};

		struct IMyForm : IForm
		{
			typedef std::function<void()> lpfn_onTimer;
			typedef signals::connection timer_conn;

			virtual timer_conn AddTimer( UINT nSec, lpfn_onTimer ) = 0;
		};

		struct ILabel : IWinControl
		{
			virtual void SetColor( COLORREF text, COLORREF bkcolor ) = 0;
			virtual void GetColor( COLORREF& text, COLORREF& bkcolor ) = 0;

		};

		struct IEdit : IWinControl
		{
			virtual void SetColor( COLORREF text, COLORREF bkcolor ) = 0;
			virtual void GetColor( COLORREF& text, COLORREF& bkcolor ) = 0;
			//signals::signal<void()> event_OnChanged;
		};

		struct IButton : IWinControl
		{
			signals::signal<void()> event_OnClicked;
		};

		struct ICheck : IWinControl
		{
			signals::signal<void( bool )> event_OnChanged;

			virtual bool GetCheck() const = 0;
			virtual void SetCheck( bool ) = 0;
		};

		struct IColorButton : IButton
		{
			virtual void SetColor( COLORREF text, COLORREF bkcolor, COLORREF hover_text, COLORREF hover_bk ) = 0;

		};

		struct IListReport : IWinControl
		{
			virtual int AppendColumn( mui_argstr, int nWidth ) = 0;
			virtual int AppendItem() = 0;
			virtual void SetItemText( int nItem, int nCol, mui_argstr s ) = 0;
			virtual void SetItemData( int nItem, int data ) = 0;
			virtual size_t GetItemCount() const = 0;
			virtual void DeleteAllItems() = 0;
			virtual int GetItemData( int nItem ) const = 0;
			virtual int GetFirstSelect() const = 0;
			virtual int FindItem_FromData( int data ) const = 0;
		};
	};


};