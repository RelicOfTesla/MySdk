#pragma once

#include "IControl.h"
#include "TemplateProp.h"
namespace mui
{
	namespace Win
	{
		struct IWinControlImpl : IWinControl
		{
// 		protected:
// 			virtual void initialize(mui_argstr, HWND, arg_sptr_c<ITemplateDB>::type) = 0;
// 			virtual void uninitialize() = 0;

		};

		shared_ptr<IWinControlImpl> CreateWinControlImpl( mui_argstr control_typename, mui_argstr wnd_classname,
				arg_sptr_c<ITemplateDB>::type pTemp, arg_sptr_c<IControlPack>::type pParent );

	};

};