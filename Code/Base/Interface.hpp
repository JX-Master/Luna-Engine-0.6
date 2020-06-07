// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Interface.hpp
* @author JXMaster
* @date 2018/12/9
* @brief Some helper facilities to make interface implementation easier and faster.
 */
#pragma once
#include "IAllocator.hpp"
#include "Atomic.hpp"

// Helpers to implement `query_interface` and `query_interface_raw`.
namespace luna
{
	namespace impl
	{
		//! Helper function for casting interface.
		//! If the interface indicated by `iid` is one of the interfaces specified by template
		//! arguments, the `ret_object` which supports the interface specified will be casted to
		//! that interface type and returned.
		//! This function will NOT call add_ref internally, call it after you returned the object.
		template <typename _CT>
		RV query_interface_raw(const Guid& iid, void** obj, _CT* ret_object)
		{
			if (iid == _CT::_guid)
			{
				if (obj) { *obj = static_cast<_CT*>(ret_object); }
				return s_ok;
			}
			return e_interface_not_supported;
		}
		template <typename _CT, typename _I1, typename... _Is>
		RV query_interface_raw(const Guid& iid, void** obj, _CT* ret_object)
		{
			if (iid == _I1::_guid)
			{
				if (obj) { *obj = static_cast<_I1*>(ret_object); }
				return s_ok;
			}
			return query_interface_raw<_CT, _Is...>(iid, obj, ret_object);
		}

		//! Same as query_interface_raw, but call `add_ref` internally.
		template <typename _CT>
		RV query_interface(const Guid& iid, void** obj, _CT* ret_object)
		{
			RV r = query_interface_raw(iid, obj, ret_object);
			if (succeeded(r) && obj)
			{
				ret_object->add_ref();
			}
			return r;
		}
		
		//! Check if the input `iid` is one of the types specified by template argument.
		template <typename... _Is>
		bool is_any_of(const Guid& iid)
		{
			for (auto& id : { _Is::_guid... })
			{
				if (id == iid)
				{
					return true;
				}
			}
			return false;
		}
	}
}

// Query support macro pairs.
// Using QBEGIN, QITEM, QEND to declare all supported interface by this object.

#define luqbegin()	\
	virtual RV query_interface_raw(const Guid& iid, void** obj) override\
	{ 

#define luqitem(_ret_object, ...) \
		if(succeeded(impl::query_interface_raw<__VA_ARGS__>(iid, obj, _ret_object)))	\
		{	\
			return s_ok;	\
		}
#define luqdef()	\
		if (obj)	\
		{	\
			*obj = nullptr;	\
		}	\
		return e_interface_not_supported;

#define luqend()	\
		luqdef();	\
	}	\
	virtual RV query_interface(const Guid& iid, void** obj) override\
	{	\
		RV r = query_interface_raw(iid, obj);	\
		if(succeeded(r) && obj)	\
		{	\
			add_ref();	\
		}	\
		return r;	\
	}

// Shortcut pair for QBEGIN, QITEM, QEND if you only return `this` as the pointer.
#define luqself(...) \
		luqbegin();	\
		luqitem(this, __VA_ARGS__);	\
		luqend();

// Helpers to implement `add_ref`, `release`, `get_ref`, `add_weak_ref`, `release_weak`, `get_weak_ref`, `add_ref_if_not_expired` and `expired`.

namespace luna
{
	namespace impl
	{
		//! Helper struct to manage reference counting.
		struct RC
		{
			IAllocator* m_alloc;
			ref_count_t m_ref_count;
			ref_count_t m_weak_ref;
			uint32 m_expired;

			RC(IAllocator* alloc) :
				m_alloc(alloc),
				m_ref_count(1),
				m_weak_ref(0),
				m_expired(0)
			{
				if (alloc)
				{
					alloc->add_ref();
				}
			}
			~RC() {}
			template <typename _Owner>
			void expire(_Owner& o)
			{
				if (!atom_exchange_u32(&m_expired, 1))
				{
					//! We use number 2 to identify the object is destroying, so present the calls to `destroy`
					//! in destructor ~_Ty() to free this object. This may happen when object A holds a strong
					//! reference to object B and object B holds a weak reference to A, when A is expiring, it
					//! releases the reference to B, so B is destroyed and releases the reference to A, and makes
					//! weak and strong reference count of A being 0, which will trigger the `destroy` in 
					//! `release_weak`, and then triggers `destroy` again after A is destructed in `release`.
					atom_exchange_u32(&m_expired, 2);
					o.~_Owner();
					atom_exchange_u32(&m_expired, 1);
				}
			}
			template <typename _Owner>
			void destroy(_Owner& o)
			{
				if (m_alloc && m_expired != 2)
				{
					P<IAllocator> alloc = m_alloc;
					alloc->release();
					if(alignof(_Owner) <= max_align_v)
					{
						alloc->free(&o);
					}
					else
					{
						alloc->free_aligned(&o, alignof(_Owner));
					}
				}
			}

			ref_count_t add_ref()
			{
				return atom_inc_i32(&(m_ref_count));
			}
			RV add_ref_if_not_expired()
			{
				while (true)
				{
					ref_count_t tmp = static_cast<ref_count_t const volatile&>(m_ref_count);
					if (!tmp)
					{
						return e_expired;
					}
					if (atom_compare_exchange_i32(&m_ref_count, tmp + 1, tmp) == tmp)
					{
						return s_ok;
					}
				}
			}
			template <typename _Owner>
			ref_count_t release(_Owner& owner)
			{
				ref_count_t r = atom_dec_i32(&(m_ref_count));
				if (!r)
				{
					expire<_Owner>(owner);
					if (!(m_weak_ref))
					{
						destroy<_Owner>(owner);
					}
				}
				return r;
			}
			ref_count_t add_weak_ref()
			{
				return atom_inc_i32(&(m_weak_ref));
			}
			template <typename _Owner>
			ref_count_t release_weak(_Owner& owner)
			{
				ref_count_t r = atom_dec_i32(&(m_weak_ref));
				if (!r && !(m_ref_count))
				{
					destroy<_Owner>(owner);
				}
				return r;
			}
		};
	}
}

#define lurc() \
	impl::RC m_rc;	\
	virtual ref_count_t get_ref() override	\
	{	\
		return m_rc.m_ref_count;	\
	}	\
	virtual ref_count_t add_ref() override	\
	{	\
		return m_rc.add_ref();	\
	}	\
	virtual RV add_ref_if_not_expired() override	\
	{	\
		return m_rc.add_ref_if_not_expired();	\
	}	\
	virtual ref_count_t release() override	\
	{	\
		return m_rc.release(*this);	\
	}	\
	virtual ref_count_t get_weak_ref() override	\
	{	\
		return m_rc.m_weak_ref;	\
	}	\
	virtual ref_count_t add_weak_ref() override	\
	{	\
		return m_rc.add_weak_ref();	\
	}	\
	virtual ref_count_t release_weak() override	\
	{	\
		return m_rc.release_weak(*this);	\
	}	\
	virtual bool expired() override	\
	{	\
		return (bool)m_rc.m_expired;	\
	}

namespace luna
{
	namespace impl
	{
		struct RCNoFree
		{
			ref_count_t m_ref_count;
			ref_count_t m_weak_ref;
			uint32 m_expired;

			RCNoFree() :
				m_ref_count(1),
				m_weak_ref(0),
				m_expired(0) {}
			~RCNoFree() {}
			template <typename _Owner>
			void expire(_Owner& o)
			{
				if (!atom_exchange_u32(&m_expired, 1))
				{
					//! We use number 2 to identify the object is destroying, so present the calls to `destroy`
					//! in destructor ~_Ty() to free this object. This may happen when object A holds a strong
					//! reference to object B and object B holds a weak reference to A, when A is expiring, it
					//! releases the reference to B, so B is destroyed and releases the reference to A, and makes
					//! weak and strong reference count of A being 0, which will trigger the `destroy` in 
					//! `release_weak`, and then triggers `destroy` again after A is destructed in `release`.
					atom_exchange_u32(&m_expired, 2);
					o.~_Owner();
					atom_exchange_u32(&m_expired, 1);
				}
			}
			ref_count_t add_ref()
			{
				return atom_inc_i32(&(m_ref_count));
			}
			RV add_ref_if_not_expired()
			{
				while (true)
				{
					ref_count_t tmp = static_cast<ref_count_t const volatile&>(m_ref_count);
					if (!tmp)
					{
						return e_expired;
					}
					if (atom_compare_exchange_i32(&m_ref_count, tmp + 1, tmp) == tmp)
					{
						return s_ok;
					}
				}
			}
			template <typename _Owner>
			ref_count_t release(_Owner& owner)
			{
				ref_count_t r = atom_dec_i32(&(m_ref_count));
				if (!r)
				{
					expire<_Owner>(owner);
				}
				return r;
			}
			ref_count_t add_ref_weak()
			{
				return atom_inc_i32(&(m_weak_ref));
			}
			ref_count_t release_weak()
			{
				ref_count_t r = atom_dec_i32(&(m_weak_ref));
				return r;
			}
		};
	}
}

//! Does not call destroy() when all references are freed.
#define lurc_static() \
	impl::RCNoFree m_rc;	\
	virtual ref_count_t get_ref() override	\
	{	\
		return m_rc.m_ref_count;	\
	}	\
	virtual ref_count_t add_ref() override	\
	{	\
		return m_rc.add_ref();	\
	}	\
	virtual RV add_ref_if_not_expired() override	\
	{	\
		return m_rc.add_ref_if_not_expired();	\
	}	\
	virtual ref_count_t release() override	\
	{	\
		return m_rc.release(*this);	\
	}	\
	virtual ref_count_t get_weak_ref() override	\
	{	\
		return m_rc.m_weak_ref;	\
	}	\
	virtual ref_count_t add_weak_ref() override	\
	{	\
		return m_rc.add_ref_weak();	\
	}	\
	virtual ref_count_t release_weak() override	\
	{	\
		return m_rc.release_weak();	\
	}	\
	virtual bool expired() override	\
	{	\
		return (bool)m_rc.m_expired;	\
	}

#define luibind(_alloc) \
	m_rc(_alloc)

#define luiimpl(...)	\
	luqself(__VA_ARGS__);	\
	lurc();

#define luiimpl_static(...)	\
	luqself(__VA_ARGS__);	\
	lurc_static();

//! Delegate strategy: Share the lifetime with another object, and dispatch all API calls
//! to another object.
#define luiimpl_delegate(_t) \
	virtual RV query_interface(const Guid& iid, void** obj) override	\
	{	\
		return (_t)->query_interface(iid, obj);	\
	}	\
	virtual RV query_interface_raw(const Guid& iid, void** obj) override	\
	{	\
		return (_t)->query_interface_raw(iid, obj);	\
	}	\
	virtual bool expired() override \
	{	\
		return (_t)->expired();	\
	}	\
	virtual ref_count_t get_ref() override \
	{	\
		return (_t)->get_ref();	\
	}	\
	virtual ref_count_t add_ref() override	\
	{	\
		return (_t)->add_ref();	\
	}	\
	virtual ref_count_t release() override	\
	{	\
		return (_t)->release();	\
	}	\
	virtual ref_count_t get_weak_ref()	\
	{	\
		return (_t)->get_weak_ref();	\
	}	\
	virtual ref_count_t add_weak_ref() override	\
	{	\
		return (_t)->add_weak_ref();	\
	}	\
	virtual ref_count_t release_weak() override	\
	{	\
		return (_t)->release_weak();	\
	}

//! A helper interface that redirects IObject interface to one of its parent.
#define luiprimary(_t) \
	virtual RV query_interface(const Guid& iid, void** obj) override	\
	{	\
	return ((_t*)this)->query_interface(iid, obj);	\
	}	\
	virtual RV query_interface_raw(const Guid& iid, void** obj) override	\
	{	\
	return ((_t*)this)->query_interface_raw(iid, obj);	\
	}	\
	virtual bool expired() override \
	{	\
	return ((_t*)this)->expired();	\
	}	\
	virtual ref_count_t get_ref() override \
	{	\
	return ((_t*)this)->get_ref();	\
	}	\
	virtual ref_count_t add_ref() override	\
	{	\
	return ((_t*)this)->add_ref();	\
	}	\
	virtual ref_count_t release() override	\
	{	\
	return ((_t*)this)->release();	\
	}	\
	virtual ref_count_t get_weak_ref()	\
	{	\
	return ((_t*)this)->get_weak_ref();	\
	}	\
	virtual ref_count_t add_weak_ref() override	\
	{	\
	return ((_t*)this)->add_weak_ref();	\
	}	\
	virtual ref_count_t release_weak() override	\
	{	\
	return ((_t*)this)->release_weak();	\
	}