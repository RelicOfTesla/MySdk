#include "StdAfx_ui.h"
#include "TemplateProp.hpp"
#include "ICreater.h"
//////////////////////////////////////////////////////////////////////////

struct static_var
{
	shared_ptr<Json::Value> pjson_value;
	shared_ptr<mui::ITemplateDB> pTempDB;

	static_var() : pjson_value( new Json::Value )
	{
		pTempDB.reset( new CTemplateDB_Json( pjson_value ) );
	}
};
static_var* g_nil_json_t = new static_var;
shared_ptr<mui::ITemplateDB>& g_nil_temp_db = g_nil_json_t->pTempDB;
shared_ptr<Json::Value>& g_nil_pjson_value = g_nil_json_t->pjson_value;

namespace mui
{
	shared_ptr<ITemplateDB> LoadTemplate_FromJsonData( arg_sptr_c<Json::Value>::type pRoot )
	{
		if( !pRoot || pRoot->empty() )
		{
			return shared_ptr<ITemplateDB>();
		}
		shared_ptr<CTemplateDB_Json> pResult( new CTemplateDB_Json( pRoot ) );
		return pResult;
	}
	shared_ptr<ITemplateDB> LoadTemplate_FromJsonFile( mui_argstr filepath )
	{
		std::string ec;
		shared_ptr<Json::Value> pRoot = pjson_from_file( filepath.c_str(), &ec );
		if( ec.size() )
		{
			throw std::runtime_error( ec );
		}
		return LoadTemplate_FromJsonData(pRoot);
	}
	shared_ptr<ITemplateDB> LoadTemplate_FromJsonString( mui_argstr jsonStr )
	{
		std::string ec;
		shared_ptr<Json::Value> pRoot = pjson_from_string( jsonStr, &ec );
		if( ec.size() )
		{
			throw std::runtime_error( ec );
		}
		return LoadTemplate_FromJsonData(pRoot);
	}
};

using namespace mui;

CTemplateDB_Json::CTemplateDB_Json( arg_sptr_c<Json::Value>::type pRoot )
	: m_root( pRoot ), m_current( *pRoot )
{
}


CTemplateDB_Json::CTemplateDB_Json( arg_sptr_c<Json::Value>::type pRoot, const Json::Value& v, const std::list<const Json::Value*>& new_parent_path )
	: m_root( pRoot ), m_current( v ), m_parent_path( new_parent_path )
{
}

CTemplateDB_Json::~CTemplateDB_Json()
{
}

bool CTemplateDB_Json::empty()const
{
	if( m_root )
	{
		if( !m_current.empty() )
		{
			return false;
		}
	}
	return true;
}

shared_ptr<const ITemplateDB> CTemplateDB_Json::GetChild( mui_argstr name )const
{
	if( empty() )
	{
		return g_nil_temp_db;
	}
	if( !m_current.isObject() )
	{
		return g_nil_temp_db;
	}
	const Json::Value& nv = m_current[name];
	if( nv.empty() )
	{
		return g_nil_temp_db;
	}
	std::list< const Json::Value* > new_parent_path = m_parent_path;
	new_parent_path.push_back( &m_current );
	shared_ptr<const ITemplateDB> pChild( new CTemplateDB_Json( m_root, nv, new_parent_path ) );
	return pChild;
}

shared_ptr<const ITemplateDB> CTemplateDB_Json::GetParent()const
{
	if( m_parent_path.empty() )
	{
		return g_nil_temp_db;
	}
	std::list<const Json::Value*> new_parentlist = m_parent_path;
	const Json::Value* parent = new_parentlist.back();
	new_parentlist.pop_back();
	shared_ptr<const ITemplateDB> p( new CTemplateDB_Json( m_root, *parent, new_parentlist ) );
	return p;
}


shared_ptr<const ITemplateDB> CTemplateDB_Json::GetRoot()const
{
	shared_ptr<const ITemplateDB> p( new CTemplateDB_Json( m_root ) );
	return p;
}


UINT CTemplateDB_Json::as_UINT()const
{
	if( empty() )
	{
		return 0;
	}
	try
	{
		return m_current.asUInt();
	}
	catch( const std::exception&)
	{
		THROW_TEMPLATE_EXCEPTION;
		return 0;
	}
}

mui_string CTemplateDB_Json::as_mui_string()const
{
	if( empty() )
	{
		return mui_string();
	}
	try
	{
		return m_current.asString();
	}
	catch( const std::exception&)
	{
		THROW_TEMPLATE_EXCEPTION;
		return mui_string();
	}
}

shared_ptr<ITemplateIterator> CTemplateDB_Json::first_iter()const
{
	shared_ptr<ITemplateIterator> p( new CTemplateIterator_Json( shared_from_this() ) );
	return p;
}

//////////////////////////////////////////////////////////////////////////
CTemplateIterator_Json::CTemplateIterator_Json( arg_sptr_c<CTemplateDB_Json>::type from )
	: m_parent( from ), m_iter( from->m_current.begin() ), m_end( from->m_current.end() )
{
}

void CTemplateIterator_Json::move_next()
{
	++m_iter;
}

bool CTemplateIterator_Json::empty()const
{
	return m_iter == m_end;
}

shared_ptr<ITemplateDB> CTemplateIterator_Json::get_value()const
{
	std::list<const Json::Value*> new_parent_path = m_parent->m_parent_path;
	new_parent_path.push_back( &m_parent->m_current );
	shared_ptr<CTemplateDB_Json> pval( new CTemplateDB_Json( m_parent->m_root, *m_iter, new_parent_path ) );
	return pval;
}
