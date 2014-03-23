#pragma once

#include "IControl.h"
#include "TemplateProp.h"
#include "WinControlImpl.h"

namespace mui
{
	namespace Win
	{
		class CWinControlImplBase : public IWinControlImpl , public enable_shared_from_this< CWinControlImplBase >
		{
		protected:
			static LONG CALLBACK _WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		protected:
			LONG BottomWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
		public:
			virtual void initialize( mui_argstr control_typename, HWND, arg_sptr_c<ITemplateDB>::type );
			virtual void uninitialize();
			virtual bool is_invalid() const;
		protected:
			CWinControlImplBase();
			~CWinControlImplBase();

			virtual slot_wndproc SetWndProc( slot_wndproc );
			virtual HWND GetHWND() const;
		protected:
			WNDPROC m_raw_wnd_proc;
			slot_wndproc m_user_wndproc;
			HWND m_hWnd;
		};

		class CWinControlImplEx : public CWinControlImplBase
		{
		public:
			static shared_ptr<CWinControlImplEx> Create( mui_argstr control_typename, mui_argstr wnd_classname,
					arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );
			LONG OnMsg( UINT uMsg, WPARAM wParam, LPARAM lParam );
		protected:
			CWinControlImplEx();
		public:
			virtual void initialize( mui_argstr control_typename, HWND, arg_sptr_c<ITemplateDB>::type );
			~CWinControlImplEx();
		public:
			SIZE GetWindowSize() const;
		protected:
			virtual CONTROL_ID GetID() const;
			virtual shared_ptr<mui_string> GetTitle() const;
			virtual void SetTitle( mui_argstr str );
		protected:
			shared_ptr<mui_string> GetWinText() const;
			void SetWinText( mui_argstr str );
		protected:
			CONTROL_ID m_ID;
			slot_wndproc m_next_wndproc;
			HFONT m_hFont;
		};


		shared_ptr<IWinControlImpl> CreateWinControlImpl( mui_argstr control_typename, mui_argstr wnd_classname,
				arg_sptr_c<ITemplateDB>::type pTemp, shared_ptr<IControlPack> pParent );
	};


};