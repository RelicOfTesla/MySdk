#pragma once

#include <string>
#include <c++10/functional.h>
#include <sdk/shared_ptr.h>
#include <sdk/noncopyable.h>

typedef std::string mui_string;
typedef const mui_string& mui_argstr;

namespace mui
{
	struct IBaseControl;
	struct IControl;
};

static shared_ptr<mui::IBaseControl> nil_base_control;
static shared_ptr<mui::IControl> nil_control;
