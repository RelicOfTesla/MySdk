#pragma once
#include "IControl.h"
#include "IUiApp.h"
#include "IFormThread.h"
#include "util_class.hpp"
namespace mui
{
	namespace Win
	{
		template<typename base_type>
		struct TWinControlProxy : base_type
		{
			bool is_invalid() const
			{
				return m_impl->is_invalid();
			}
			virtual HWND GetHWND() const
			{
				return m_impl->GetHWND();
			}
			virtual shared_ptr<mui_string> GetTitle() const
			{
				return m_impl->GetTitle();
			}
			virtual void SetTitle( mui_argstr str )
			{
				return m_impl->SetTitle( str );
			}
			virtual IControl::CONTROL_ID GetID() const
			{
				return m_impl->GetID();
			}
			virtual SIZE GetWindowSize() const
			{
				return m_impl->GetWindowSize();
			}
			virtual Win::IWinControl::slot_wndproc SetWndProc( Win::IWinControl::slot_wndproc proc )
			{
				//dbg_assert( dynamic_pointer_cast<signals::trackable>(proc) );
				return m_impl->SetWndProc( proc );
			}
			TWinControlProxy( arg_sptr<Win::IWinControl>::type p ) : m_impl( p )
			{
			}
			shared_ptr<IWinControl> m_impl;
		};


		class CMyForm : public TWinControlProxy< TControlPackImpl<IMyForm> >, public enable_shared_from_this<CMyForm>
		{
		public:
			CMyForm( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );

			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CMyForm> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		public:
			virtual timer_conn AddTimer( UINT nSec, lpfn_onTimer pfn );
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
			COLORREF m_cBkColor;
			HBRUSH m_hBkBrush;

			typedef shared_ptr< signals::signal<void()> > fnlist_ptr;
			std::map<UINT, fnlist_ptr> m_timer_fnlist;
		};

		class CLabel : public TWinControlProxy<ILabel>
		{
			friend class CColorButton;
		public:
			CLabel( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );

			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CLabel> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		public:
			virtual void SetColor( COLORREF text, COLORREF bkcolor );
			virtual void GetColor( COLORREF& text, COLORREF& bkcolor );
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
			RECT m_DrawRect;
			COLORREF m_cTextColor, m_cBkColor;
		};

		class CEdit : public TWinControlProxy<IEdit>
		{
		public:
			CEdit( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );
			~CEdit();
			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CEdit> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		public:
			virtual void SetColor( COLORREF text, COLORREF bkcolor );
			virtual void GetColor( COLORREF& text, COLORREF& bkcolor );
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
			COLORREF m_cTextColor, m_cBkColor;
			HBRUSH m_hBkBrush;
		};

		class CButton : public TWinControlProxy<IButton>
		{
		public:
			CButton( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );
			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CButton> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
		protected:
		};

		class CCheck : public TWinControlProxy<ICheck>
		{
		public:
			CCheck( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );
			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CCheck> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		public:
			bool GetCheck()const;
			void SetCheck( bool );
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
			COLORREF m_cTextColor, m_cBkColor;
			HBRUSH m_hBkBrush;
		};

		class CColorButton : public TWinControlProxy<IColorButton>
		{
		public:
			CColorButton( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );

			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CColorButton> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		public:
			virtual void SetColor( COLORREF text, COLORREF bkcolor, COLORREF hover_text, COLORREF hover_bk );
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
		protected:
			shared_ptr<CLabel> m_label;
			bool m_bMouseIn;
			bool m_bPressing;
			COLORREF m_cNormalTextColor, m_cNormalBkColor;
			COLORREF m_cHoverTextColor, m_cHoverBkColor;
			COLORREF m_cDisableTextColor, m_cDisableBkColor;
		};

		class CWndControl : public TWinControlProxy<IWinControl>, public TControlPackImpl<IControlPack>
		{
		public:
			CWndControl( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );

			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CWndControl> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
			shared_ptr<COLORREF> m_pTextColor, m_pBkColor;
			HBRUSH m_hBkBrush;
			BOOL m_erase_bk;
		};

		class CListReport : public TWinControlProxy<IListReport>
		{
		public:
			CListReport( arg_sptr<IWinControl>::type p, arg_sptr_c<ITemplateDB>::type );

			static shared_ptr<RegContext> NewContext();
			static shared_ptr<CListReport> Create( arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			static mui_string _RegName;
			static mui_string _WndClsName;
		public:
			virtual int AppendColumn( mui_argstr, int nWidth );
			virtual int AppendItem();
			virtual void SetItemText( int nItem, int, mui_argstr );
			virtual void SetItemData( int nItem, int data );
			virtual size_t GetItemCount()const;
			virtual void DeleteAllItems();
			virtual int GetItemData( int nItem )const;
			virtual int GetFirstSelect()const;
			virtual int FindItem_FromData(int data)const;
		protected:
			LONG OnMsg( UINT uMsg, WPARAM, LPARAM );
		protected:
			slot_wndproc m_wndproc;
		protected:
		};
	}
};
