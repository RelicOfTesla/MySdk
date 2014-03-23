#pragma once

#include <sdk/shared_ptr.h>
#include <string>
#include <deque>
#include <map>
#include <sdk/dbg_assert.h>

struct CArchiveObject;

struct IBaseArchive
{
	virtual shared_ptr<IBaseArchive> dump(CArchiveObject*) = 0;

	virtual bool empty() = 0;

	virtual void remove() = 0;
};

struct IArchiveImpl : IBaseArchive
{
	virtual int ToInt() = 0;
	virtual size_t ToSize() = 0;
	virtual bool ToBool() = 0;
	virtual std::string ToStr() = 0;

	virtual void SetInt(int) = 0;
	virtual void SetSize(size_t) = 0;
	virtual void SetBool(bool) = 0;
	virtual void SetStr(const char*) = 0;
};

namespace archive_function
{
	template<typename T> void assign(IBaseArchive* pImpl, const T& value);

// 	inline void assign(IBaseArchive* pImpl, const T& value);


	template<typename T>
	T to_value(IBaseArchive* pImpl);
};

namespace archive_function
{
	inline void assign(IBaseArchive* pImpl, const int& value)
	{
		((IArchiveImpl*)pImpl)->SetInt(value);
	}

	inline void assign(IBaseArchive* pImpl, const size_t& value)
	{
		((IArchiveImpl*)pImpl)->SetSize(value);
	}

	inline void assign(IBaseArchive* pImpl, const bool& value)
	{
		((IArchiveImpl*)pImpl)->SetBool(value);
	}

	inline void assign(IBaseArchive* pImpl, const char* value)
	{
		((IArchiveImpl*)pImpl)->SetStr(value);
	}
	inline void assign(IBaseArchive* pImpl, const std::string& value)
	{
		assign(pImpl, value.c_str());
	}

	//////////////////////////////////////////////////////////////////////////
	template<> inline
	int to_value<int>(IBaseArchive* pImpl)
	{
		return ((IArchiveImpl*)pImpl)->ToInt();
	}

	template<> inline
	size_t to_value<size_t>(IBaseArchive* pImpl)
	{
		return ((IArchiveImpl*)pImpl)->ToSize();
	}

	template<> inline
	bool to_value<bool>(IBaseArchive* pImpl)
	{
		return ((IArchiveImpl*)pImpl)->ToBool();
	}

	template<> inline
	std::string to_value<std::string>(IBaseArchive* pImpl)
	{
		return ((IArchiveImpl*)pImpl)->ToStr();
	}

};

struct CArchiveObject : enable_shared_from_this< CArchiveObject >
{
	typedef std::string tstring;
	typedef const tstring& tstring_param;

	typedef CArchiveObject ArchiveObject;

	shared_ptr<IBaseArchive> m_impl;
	weak_ptr<ArchiveObject> m_parent;
	std::map< tstring, shared_ptr<ArchiveObject> > m_childs;
	tstring m_name;

	CArchiveObject(shared_ptr<IArchiveImpl> impl, tstring_param name)
		: m_name(name)
	{
		m_impl = impl->dump(this);
	}
	CArchiveObject(shared_ptr<ArchiveObject> parent, tstring_param name)
		: m_parent(parent), m_name(name)
	{
		m_impl = parent->m_impl->dump(this);
	}

	shared_ptr<ArchiveObject> GetParent()
	{
		return m_parent.lock();
	}

	ArchiveObject& Get(tstring_param name)
	{
		dbg_assert(this);
		shared_ptr<ArchiveObject>& p = m_childs[name];
		if (!p)
		{
			p.reset(new ArchiveObject(shared_from_this(), name));;
		}
		return *p;
	}
	///
	template<typename T> void Assign(const T& value)
	{
		archive_function::assign(m_impl.get(), value);
	}
	template<typename T> T ToValue()
	{
		return archive_function::to_value<T>(m_impl.get());
	}
	template<typename T> T OptValue(const T& def)
	{
		return IsEmpty() ? def : ToValue<T>();
	}
	bool IsEmpty()
	{
		return m_impl->empty();
	}
	void Erase(const char* name)
	{
		Get(name).m_impl->remove();
		m_childs.erase(name);
	}

	template<typename T> T OptValue(const char* name, const T& def)
	{
		return Get(name).OptValue(def);
	}
	tstring OptValue(const char* name, const char def[])
	{
		return Get(name).OptValue<tstring>(def);
	}
	bool Has(tstring_param name)
	{
		return Get(name).IsEmpty();
	}

	ArchiveObject& operator [](tstring_param name)
	{
		return Get(name);
	}
	template<typename T> void operator =(const T& value)
	{
		Assign(value);
	}
	template<typename T> operator T()
	{
		return ToValue<T>();
	}
	//////////////////////////////////////////////////////////////////////////
	std::deque<std::string> GetPath()
	{
		std::deque<std::string> result;
		for (shared_ptr<ArchiveObject> node = shared_from_this(); node; node = node->m_parent.lock())
		{
			if (node->m_name.size())
			{
				result.push_front(node->m_name);
			}
		}
		return result;
	}
};

typedef shared_ptr<CArchiveObject> archive_root;
typedef CArchiveObject& archive_node;