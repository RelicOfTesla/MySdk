#pragma once

#include "TemplateProp.h"


namespace mui
{
	shared_ptr<ITemplateDB> LoadTemplate_FromJsonFile( mui_argstr );
	shared_ptr<ITemplateDB> LoadTemplate_FromJsonString( mui_argstr );
};

