#pragma once

#include "IUiApp.h"

using namespace mui;


class CUiApp : public IUiApp
{
public:
	virtual void RegisterClass( arg_sptr_c<RegContext>::type );
	virtual shared_ptr<const RegContext> GetRegisterContext( mui_argstr )const;

	virtual shared_ptr<IFormThread> GetFormThread( UINT );

	virtual void InsertFormThread( arg_sptr<IFormThread>::type );

protected:
	std::map<mui_string, shared_ptr<const RegContext> > m_regmap;
	std::map<UINT, shared_ptr<IFormThread> > m_threadmap;
};