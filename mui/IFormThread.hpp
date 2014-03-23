#pragma once
#include "IFormThread.h"
#include "util_class.hpp"
#include <list>

using namespace mui;

namespace std
{
	template<typename T>
	bool operator < ( const weak_ptr<T>& left, const weak_ptr<T>& right )
	{
		return left.lock() < right.lock();
	}

};

class CFormThread : public IFormThread, public TControlPackImpl<IControlPack>, public enable_shared_from_this<CFormThread>
{
public:
	CFormThread( UINT TID );
	~CFormThread();

	virtual void DoModal( BOOL bWaltAll );
	virtual UINT GetThreadId();
	virtual bool InsertMap( PRIV_ID id, weak_ptr<IBaseControl> );
	virtual bool RemoveMap( PRIV_ID );
	virtual shared_ptr<IBaseControl> GetMap( PRIV_ID id );
protected:

	UINT m_TID;
	std::map< PRIV_ID, weak_ptr<IBaseControl> > m_priv_maps;

};

