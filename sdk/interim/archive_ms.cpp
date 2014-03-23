#include "stdafx.h"
#include "archive.h"


#include <boost/lexical_cast.hpp>

struct CMsArchiveImpl : IArchiveImpl
{
	typedef CArchiveObject ArchiveObject;
	typedef std::string tstring;
	shared_ptr<tstring> m_pPath;
	CArchiveObject* m_obj;
	tstring m_ParentPath;

	CMsArchiveImpl(const tstring& path) : m_pPath(new tstring(path)), m_obj(0)
	{
		//dbg_assert( dynamic_cast<ArchiveObject*>(this) );
	}
	CMsArchiveImpl(CArchiveObject* newobj, CMsArchiveImpl* parent) : m_obj(newobj)
	{
		m_pPath = parent->m_pPath;
		m_ParentPath = GetParentPath();
	}
	std::string GetParentPath()
	{
		std::string s;
		if( shared_ptr<ArchiveObject> parent = m_obj->GetParent() )
		{
			auto paths = parent->GetPath();
			auto it = paths.begin();
			if (it != paths.end())
			{
				s = *it;
				for (++it; it != paths.end(); ++it)
				{
					s += '-' + *it;
				}
			}
		}
		return s;
	}

	int ToInt()
	{
		if( m_ParentPath.size() )
		{
			return GetPrivateProfileIntA(m_ParentPath.c_str(), m_obj->m_name.c_str(), 0, m_pPath->c_str());
		}
		return 0;
	}
	size_t ToSize()
	{
		if( m_ParentPath.size() )
		{
			return GetPrivateProfileIntA(m_ParentPath.c_str(), m_obj->m_name.c_str(), 0, m_pPath->c_str());
		}
		return 0;
	}
	bool ToBool()
	{
		if( m_ParentPath.size() )
		{
			return GetPrivateProfileIntA(m_ParentPath.c_str(), m_obj->m_name.c_str(), 0, m_pPath->c_str()) != 0;
		}
		return false;
	}
	std::string ToStr()
	{
		if( m_ParentPath.size() )
		{
			std::string buf;
			for (size_t buflen = 1024; ; buflen *= 2)
			{
				buf.resize(buflen);
				size_t len = GetPrivateProfileStringA(m_ParentPath.c_str(), m_obj->m_name.c_str(), "", &buf.front(), buflen, m_pPath->c_str());
				if (len < buflen - 2 )
				{
					buf.resize(len);
					break;
				}
			}
			//return GetPrivateProfileIparent->m_name.c_str(), m_obj->m_name.c_str(), 0, m_pPath->c_str())!=0;
			return buf;
		}
		return 0;
	}

	bool empty()
	{
		if( m_ParentPath.size() )
		{
			char buf[2];
			size_t len = GetPrivateProfileStringA(m_ParentPath.c_str(), m_obj->m_name.c_str(), "", buf, sizeof(buf), m_pPath->c_str());
			return len == 0;
		}
		else if (m_obj && m_obj->m_name.size())
		{
			char buf[3];
			tstring app = m_obj->m_name;
			size_t len = GetPrivateProfileSectionA(app.c_str(), buf, sizeof(buf), m_pPath->c_str());
			return len == 0;
		}
		return true;
	}
	void remove()
	{
		if( m_ParentPath.size() )
		{
			WritePrivateProfileStringA(m_ParentPath.c_str(), m_obj->m_name.c_str(), nullptr, m_pPath->c_str());
		}
	}

	void SetInt(int value)
	{
		SetStr(boost::lexical_cast<std::string>(value).c_str());
	}
	void SetSize(size_t value)
	{
		SetStr(boost::lexical_cast<std::string>(value).c_str());
	}
	void SetBool(bool value)
	{
		SetInt(value ? 1 : 0);
	}

	void SetStr(const char* value)
	{
		if( m_ParentPath.size() )
		{
			WritePrivateProfileStringA(m_ParentPath.c_str(), m_obj->m_name.c_str(), value, m_pPath->c_str());
		}
	}

	shared_ptr<IBaseArchive> dump(CArchiveObject* p)
	{
		return shared_ptr<CMsArchiveImpl>(new CMsArchiveImpl(p, this));
	}
};

archive_root MsArchive_CreateInstance(const std::string& path, const char* root)
{
	auto pImpl = shared_ptr<CMsArchiveImpl>(new CMsArchiveImpl(path));
	shared_ptr<CArchiveObject> r ( new CArchiveObject(pImpl, root) );
	return r;
}


