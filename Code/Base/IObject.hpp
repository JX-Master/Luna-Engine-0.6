// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IObject.hpp
* @author JXMaster
* @date 2018/10/26
 */
#pragma once

#include "BaseDefines.hpp"
#include "Result.hpp"
#include "Functional.hpp"

namespace luna
{
	using ref_count_t = int32;
	struct IThread;

	//! @interface IObject
	//! Base interface for all interfaces and implementation objects. The objects inherits from this implements reference count and RTTI.
	struct IObject
	{
		luiid("{00000000-0000-0000-0000-00000000FFFF}");

		//! Queries if the specified target interface is supported by this object, and optionally get a pointer to the new interface if it supports.
		//! @param[in] iid The interface id of the target interface you want to query. 
		//! This is the static `_guid` member of all interface types.
		//! @param[out] obj Optional. If this parameter is not `nullptr` and the object supports the target interface, this parameter
		//! is used to receive the pointer to the new interface and the reference count of this object is increased by 1. 
		//! If the object does not support the target interface, this parameter is set to `nullptr` and the reference count is not changed.
		//! 
		//! If this parameter is `nullptr`, this function simply checks whether the target interface is supported by this object, and report
		//! result by return value. In this case, the reference count will not be changed.
		//! 
		//! Note that the reference count added is ALWAYS strong reference count.
		//! @return Returns `s_ok` if the object supports the target interface, or `e_interface_not_supportted` if not. 
		//! If `obj` is not `nullptr` and the return value is `s_ok`, the pointer to new interface is stored in `obj`.
		virtual RV query_interface(const Guid& iid, void** obj) = 0;
		//! This function behaves the same as the `query_interface` function, except for that the reference count will not be increased if the target interface
		//! is successfully assigned to `obj`. 
		//! This function is used to implement the move assignment traits and bare reference. And should be used with caution.
		//! See docs for `query_interface` for parameters and returns.
		virtual RV query_interface_raw(const Guid& iid, void** obj) = 0;
		//! Gets the strong reference count of this object.
		//! @return Returns the strong reference count of this object.
		virtual ref_count_t get_ref() = 0;
		//! Increases the strong reference count of the object by 1.
		//! @return Returns the strong reference count after the operation.
		virtual ref_count_t add_ref() = 0;
		//! Checks if the object is expired, and adds one strong reference to the object if the object is not expired.
		//! If the object is expired, this function fails with strong reference count value being unmodified, so it will not go back to non-zero value
		//! after it is expired.
		//! This function is atomic in multi thread environment. Use this to convert a weak reference to a strong reference.
		//! @return Returns `s_ok` on success, `e_expired` if the object is expired.
		virtual RV add_ref_if_not_expired() = 0;
		//! Decreases the strong reference count of the object by 1.
		//! @return Returns the strong reference count after the operation.
		virtual ref_count_t release() = 0;
		//! Gets the weak reference count of this object.
		//! @return Returns the reference count of this object.
		virtual ref_count_t get_weak_ref() = 0;
		//! Increase the weak reference count of the object by 1.
		//! @return Returns the weak reference count after the operation.
		virtual ref_count_t add_weak_ref() = 0;
		//! Decrease the weak reference count of the object by 1.
		//! @return Returns the weak reference count after the operation.
		virtual ref_count_t release_weak() = 0;
		//! Checks if this object is expired.
		virtual bool expired() = 0;
	};


	//! @class P
	//! The P (shortcut for Pointer) template class represents a strong reference in Luna Engine. This is the 
	//! core object for implementing the ARC (automatic reference counting) in engine.
	template <typename _Ty>
	class P
	{
		_Ty* p;

		void internal_addref() const
		{
			if (p)
			{
				p->add_ref();
			}
		}
		void internal_clear()
		{
			if (p)
			{
				p->release();
				p = nullptr;
			}
		}

		template <typename _Rty> friend class P;

	public:
		//! clear the smart pointer and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Test if this pointer is empty.
		//! A pointer is valid if it is not `nullptr`.
		bool empty() const
		{
			return p == nullptr;
		}
		//! Get the underlying raw pointer without adding reference count.
		_Ty* get() const
		{
			return p;
		}
		//! Releases the underlying pointer and attaches the specified pointer without increasing ref count.
		void attach(_Ty* ptr)
		{
			internal_clear();
			p = ptr;
		}
		//! Set the underlying pointer to `nullptr` and return the pointer.
		_Ty* detach()
		{
			_Ty* r = p;
			p = nullptr;
			return r;
		}
		//! Try to cast the pointer to the type specified and detach the underlying type if succeeded.
		void* cast_detach(const Guid& iid)
		{
			void* r;
			if (succeeded(p->query_interface_raw(iid, &r)))
			{
				p = nullptr;
			}
			return r;
		}
		template <typename _Rty>
		_Rty* cast_detach()
		{
			_Rty* r;
			if (succeeded(p->query_interface_raw(_Rty::_guid, (void**)&r)))
			{
				p = nullptr;
			}
			return r;
		}
		//! Use this interface as another interface convertible for inline method calling.
		//! The reference count is not increased for this call, so you should not save 
		//! the returned pointer, or only save them in raw pointers whose lifetime is 
		//! shorter than this smart pointer.
		template <typename _Rty>
		_Rty* as()
		{
			_Rty* r;
			if (succeeded(p->query_interface_raw(_Rty::_guid, (void**)&r)))
			{
				return r;
			}
			return nullptr;
		}

		//! Checks if this interface supports the specified interface.
		template <typename _Rty>
		bool supports()
		{
			if (succeeded(p->query_interface(_Rty::_guid, nullptr)))
			{
				return true;
			}
			return false;
		}

		//! Get the address of pointer, in order to pass it to a function that fetches the interface.
		_Ty* const* get_address_of() const
		{
			return &p;
		}
		_Ty** get_address_of()
		{
			return &p;
		}
		//! Release the pointer (if any) and get the address of the underlying pointer storage.
		_Ty** release_and_get_address_of()
		{
			internal_clear();
			return &p;
		}

		P() : p{ nullptr } {}
		~P()
		{
			internal_clear();
		}
		// Ctor from another Pointer of the same type.
		P(const P& rhs) : p{ rhs.p }
		{
			internal_addref();
		}
		P(P&& rhs) : p{ rhs.p }
		{
			rhs.p = nullptr;
		}
		P& operator=(const P& rhs)
		{
			internal_clear();
			p = rhs.p;
			internal_addref();
			return *this;
		}
		P& operator=(P&& rhs)
		{
			internal_clear();
			p = rhs.p;
			rhs.p = nullptr;
			return *this;
		}
		// Ctor from another Pointer of a different type.
		template <typename _Rty>
		P(const P<_Rty>& rhs)
		{
			p = nullptr;
			if (!rhs.empty())
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
			}
		}
		template <typename _Rty>
		P(P<_Rty>&& rhs)
		{
			p = nullptr;
			if (!rhs.empty())
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
				if (p)
				{
					rhs.clear();
				}
			}
		}
		template <typename _Rty>
		P& operator=(const P<_Rty>& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
			}
			return *this;
		}
		template <typename _Rty>
		P& operator=(P<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
				if (p)
				{
					rhs.clear();
				}
			}
			return *this;
		}
		// Ctor from raw pointer of the same type.
		P(const _Ty*& rhs) : p(rhs)
		{
			internal_addref();
		}
		P& operator=(const _Ty*& rhs)
		{
			internal_clear();
			p = rhs;
			internal_addref();
			return *this;
		}
		P(_Ty*&& rhs) : p(rhs) 
		{
			internal_addref();
		}
		P& operator=(_Ty*&& rhs)
		{
			internal_clear();
			p = rhs;
			internal_addref();
			return *this;
		}
		// Ctor from raw pointer of different type.
		template <typename _Rty>
		P(_Rty*& rhs)
		{
			p = nullptr;
			if (rhs)
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
			}
		}
		template <typename _Rty>
		P(_Rty*&& rhs)
		{
			p = nullptr;
			if (rhs)
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
			}
		}
		template <typename _Rty>
		P& operator=(_Rty*& rhs)
		{
			internal_clear();
			if (rhs)
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
			}
			return *this;
		}
		template <typename _Rty>
		P& operator=(_Rty*&& rhs)
		{
			internal_clear();
			if (rhs)
			{
				auto _ = rhs->query_interface(_Ty::_guid, (void**)&p);
			}
			return *this;
		}

		//operator bool() const
		//{
		//	return p != nullptr;
		//}
		bool operator== (const P<_Ty>& rhs) const
		{
			return p == rhs.p;
		}
		bool operator!= (const P<_Ty>& rhs) const
		{
			return p != rhs.p;
		}
		bool operator< (const P<_Ty>& rhs) const
		{
			return (size_t)p < (size_t)(rhs.p);
		}

		_Ty* operator->() const
		{
			return p;
		}

		//! Implicitly convert to raw pointer. Same as get() method.
		//! This will not add ref count.
		operator _Ty*() const
		{
			return p;
		}
	};

	template <typename _Ty>
	P<_Ty> makep(_Ty* obj)
	{
		return P<_Ty>(obj);
	}

	//! Creates an P smart pointer from a raw pointer without increasing its reference count.
	template <typename _Ty>
	P<_Ty> box_ptr(_Ty* obj)
	{
		P<_Ty> r;
		r.attach(obj);
		return r;
	}

	//! @interface WP
	//! WP (represents weak reference) smart pointer is used to hold one weak reference to the interface.
	//! One weak reference allows the target object to be destroyed before the reference is released, and the 
	//! reference gets informed when this happens by calling `IObject::expired()`. This is the core object for
	//! implementing the ARC (automatic reference counting) in Engine.
	//! 
	//! The smart pointer treats expired objects as null pointers, which means that any user access to an expired 
	//! object behaves the same as accessing to a `nullptr`. The smart pointer releases the weak reference and 
	//! reset the pointer to `nullptr` as soon as it detects the object is expired, this is transparent to user so
	//! the user only needs to check if this pointer is `empty()` to determine if the referred object is valid.
	//! 
	//! Unlike `P<_Ty>` smart pointer, the user cannot access the raw pointer `_Ty*` stored in this smart pointer directly.
	//! To access the raw pointer, the user must "lock" the pointer first to make sure the interface will not be expired
	//! when the user is using the pointer. The "lock" means querying a strong reference from the weak one, this can be done 
	//! by calling `lock()` on this interface, and if the interface is not expired, this call returns a `P<_Ty>` smart pointer 
	//! which holds a strong reference to the interface; if the interface is expired, this call returns a `P<_Ty>` which holds
	//! `nullptr` instead.
	template <typename _Ty>
	class WP
	{
		_Ty* p;

		void internal_addref() const
		{
			if (p)
			{
				p->add_weak_ref();
			}
		}
		void internal_clear()
		{
			if (p)
			{
				p->release_weak();
				p = nullptr;
			}
		}
		_Ty* getp()
		{
			if (p && p->expired())
			{
				internal_clear();
			}
			return p;
		}

		_Ty* cgetp() const
		{
			if (p && p->expired())
			{
				return nullptr;
			}
			return p;
		}

	public:
		//! clear the smart pointer and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Test if this pointer is empty.
		//! A pointer is valid if it is not `nullptr` and the object is not expired.
		bool empty()
		{
			return getp() == nullptr;
		}
		bool empty() const
		{
			return cgetp() == nullptr;
		}
		//! Set the underlying pointer to `nullptr` and return the pointer.
		_Ty* detach()
		{
			_Ty* r = getp();
			p = nullptr;
			return r;
		}
		WP() : p{ nullptr } {}
		~WP()
		{
			internal_clear();
		}
		// Ctor from weak reference of the same type.
		WP(const WP& rhs) : p{ rhs.p }
		{
			internal_addref();
		}
		WP(WP&& rhs) : p{ rhs.p }
		{
			rhs.p = nullptr;
		}
		WP& operator=(const WP& rhs)
		{
			internal_clear();
			p = rhs.p;
			internal_addref();
			return *this;
		}
		WP& operator=(WP&& rhs)
		{
			internal_clear();
			p = rhs.p;
			rhs.p = nullptr;
			return *this;
		}
		// Ctor from strong reference of the same type.
		WP(const P<_Ty>& rhs) : p(rhs.get())
		{
			internal_addref();
		}
		WP& operator=(const P<_Ty>& rhs)
		{
			internal_clear();
			p = rhs.get();
			internal_addref();
			return *this;
		}
		// Ctor from raw pointer of the same type.
		WP(const _Ty*& rhs) :
			p(rhs)
		{
			internal_addref();
		}
		WP& operator=(const _Ty*& rhs)
		{
			internal_clear();
			p = rhs;
			internal_addref();
			return *this;
		}
		// Convert to the strong reference of the same type.
		P<_Ty> lock()
		{
			if (p)
			{
				if (failed(p->add_ref_if_not_expired()))
				{
					internal_clear();
				}
				return P<_Ty>(box_ptr<_Ty>(p));
			}
			else
			{
				return P<_Ty>();
			}
		}
		operator P<_Ty>()
		{
			return lock();
		}
		operator bool()
		{
			return getp() != nullptr;
		}
		operator bool() const
		{
			return cgetp() != nullptr;
		}
		bool operator== (WP<_Ty>& rhs)
		{
			return getp() == rhs.getp();
		}
		bool operator!= (WP<_Ty>& rhs)
		{
			return getp() != rhs.getp();
		}
		bool operator== (const WP<_Ty>& rhs) const
		{
			return cgetp() == rhs.cgetp();
		}
		bool operator!= (const WP<_Ty>& rhs) const
		{
			return cgetp() != rhs.cgetp();
		}
		bool operator== (const _Ty* rhs)
		{
			return getp() == rhs;
		}
		bool operator!= (const _Ty* rhs)
		{
			return getp() != rhs;
		}
		bool operator== (const _Ty* rhs) const
		{
			return cgetp() == rhs;
		}
		bool operator!= (const _Ty* rhs) const
		{
			return cgetp() != rhs;
		}
	};

	template <typename _Ty>
	bool operator==(const _Ty*& lhs, WP<_Ty>& rhs)
	{
		return rhs == lhs;
	}

	template <typename _Ty>
	bool operator==(const _Ty*& lhs, const WP<_Ty>& rhs)
	{
		return rhs == lhs;
	}

	template <typename _Ty>
	WP<_Ty> makewp(_Ty* obj)
	{
		return WP<_Ty>(obj);
	}

	//! RP<_Ty> is an template aliasing of R<P<_Ty>> to save some spelling.
	template <typename _Ty>
	using RP = R<P<_Ty>>;
}