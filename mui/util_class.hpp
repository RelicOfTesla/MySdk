#pragma once

#include "IControl.h"
#include <sdk/str_format.h>
#include <set>

namespace mui
{
	template<typename TBase>
	class TControlPackBaseImpl : public TBase
	{
	public:
		~TControlPackBaseImpl()
		{
#if _MUI_DESTROY_CHECK
			dbg_assert( m_child_list.empty() );
#endif
		}

// 		void native_clear()
// 		{
// 			m_child_list.clear();
// 		}

		bool InsertChild( arg_sptr<IBaseControl>::type p )
		{
			bool ok = m_child_list.insert( p ).second;
			dbg_assert( ok );
			return ok;
		}
		bool RemoveChild( arg_sptr<IBaseControl>::type p )
		{
			bool ok = m_child_list.erase( p ) == 1;
			dbg_assert( ok );
			return ok;
		}
		bool CheckHasChild( arg_sptr_c<IBaseControl>::type p ) const
		{
			return m_child_list.find( const_pointer_cast<IBaseControl>(p) ) != m_child_list.end();
		}

		void Modal()
		{
			for( auto it = m_child_list.begin(); it != m_child_list.end(); )
			{
				shared_ptr<const IBaseControl> p = *it;
				if( shared_ptr<const IBaseControlPack> pack = dynamic_pointer_cast<const IBaseControlPack>( p ) )
				{
					const_pointer_cast<IBaseControlPack>(pack)->Modal();
				}
				if( !p->is_invalid() )
				{
					++it;
				}
				else
				{
					it = m_child_list.erase( it );
				}
			}
		}

		size_t GetChildSize() const
		{
			return m_child_list.size();
		}
	protected:
		std::set< shared_ptr<IBaseControl> > m_child_list;
	};
	//////////////////////////////////////////////////////////////////////////
	template<typename TBase>
	class TControlPackImpl : public TControlPackBaseImpl<TBase>
	{
	public:

// 		void native_clear()
// 		{
// 			__super::native_clear();
// 			m_idmap.clear();
// 		}

		std::list< shared_ptr<void> > m_extern_list;
		virtual void AddExternData( shared_ptr<void> p )
		{
			m_extern_list.push_back(p);
		}


		shared_ptr<IControl> FindChild( IControl::CONTROL_ID nID, bool bChildPack ) const
		{
			auto it = m_idmap.find( nID );
			if( it != m_idmap.end() )
			{
				return it->second.lock();
			}
			if( bChildPack )
			{
				for( auto it = m_child_list.begin(); it != m_child_list.end(); ++it )
				{
					if( shared_ptr<IControlPack> pk = dynamic_pointer_cast<IControlPack>( *it ) )
					{
						if( shared_ptr<IControl> p = pk->FindChild( nID, bChildPack ) )
						{
							return p;
						}
					}
				}
			}
			return shared_ptr<IControl>();
		}
		bool InsertChild( arg_sptr<IBaseControl>::type p )
		{
			if( __super::InsertChild( p ) )
			{
				if( shared_ptr<IControl> pContorl = dynamic_pointer_cast<IControl>( p ) )
				{
					IControl::CONTROL_ID nID = pContorl->GetID();
					if( nID == 0 )
					{
						return true;
					}
					bool ok = m_idmap.insert( std::make_pair( nID, pContorl ) ).second;
					dbg_assert( ok );
					return ok;
				}
			}
			return false;
		}
		bool RemoveChild( arg_sptr<IBaseControl>::type p )
		{
			if( __super::InsertChild( p ) )
			{
				if( shared_ptr<IControl> pContorl = dynamic_pointer_cast<IControl>( p ) )
				{
					IControl::CONTROL_ID nID = pContorl->GetID();
					if( nID == 0 )
					{
						return true;
					}
					bool ok = m_idmap.erase( nID ) == 1;
					dbg_assert( ok );
					return ok;
				}
			}
			return false;
		}

		void Modal()
		{
			__super::Modal();
			for( auto it = m_idmap.begin(); it != m_idmap.end(); )
			{
				weak_ptr<IControl>  wControl = it->second;
				if( !wControl.expired() )
				{
					++it;
				}
				else
				{
					it = m_idmap.erase( it );
				}
			}
		}
	protected:
		std::map< IControl::CONTROL_ID, weak_ptr<IControl> > m_idmap;
	};

};
