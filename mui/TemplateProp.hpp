#pragma once

#include "TemplateProp.h"
#include <sdk/json.h>

#define THROW_TEMPLATE_EXCEPTION

class CTemplateDB_Json : public mui::ITemplateDB, public enable_shared_from_this<CTemplateDB_Json>
{
	friend class CTemplateIterator_Json;
public:
	CTemplateDB_Json( arg_sptr_c<Json::Value>::type );
	CTemplateDB_Json( arg_sptr_c<Json::Value>::type pRoot, const Json::Value& v, const std::list<const Json::Value*>& new_parent_path );
	~CTemplateDB_Json();
public:
	virtual bool empty() const;

	virtual shared_ptr<const ITemplateDB> GetChild( mui_argstr ) const;

	virtual UINT as_UINT() const;

	virtual mui_string as_mui_string() const;

	virtual shared_ptr<mui::ITemplateIterator> first_iter() const;

	virtual shared_ptr<const ITemplateDB> GetParent() const;

	virtual shared_ptr<const ITemplateDB> GetRoot() const;

protected:
	shared_ptr<const Json::Value> m_root;
	const Json::Value& m_current;
	std::list<const Json::Value*> m_parent_path;
};


class CTemplateIterator_Json  : public mui::ITemplateIterator
{
public:
	CTemplateIterator_Json( arg_sptr_c<CTemplateDB_Json>::type from );
public:
	virtual void move_next();

	virtual	bool empty() const;

	virtual shared_ptr<mui::ITemplateDB> get_value() const;
protected:
	const shared_ptr<const CTemplateDB_Json> m_parent;
	Json::ValueIterator m_iter;
	Json::ValueIterator m_end;
};