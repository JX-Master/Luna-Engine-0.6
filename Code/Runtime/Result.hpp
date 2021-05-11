// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file Result.hpp
* @author JXMaster
* @date 2020/6/29
 */
#pragma once
#include "MemoryUtils.hpp"
#include "Assert.hpp"
#include "Error.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! R<_Ty> (represents for Result) is the wrapper object for the returned value of the function that may fail.
	//! If the function succeeds, `R<_Ty>` contains the real returned object; if the function fails, `R<_Ty>` contains 
	//! the error code along with the error domain so that it can be identified.
	template <typename _Ty>
	struct [[nodiscard]] R
	{
	private:
		errcode_t m_err_code;
		Unconstructed<_Ty> m_value;

		R() = default;

	public:

		//! Tests if the returned value is valid.
		bool valid() const
		{
			return m_err_code == 0;
		}

		//! Constructs with the returned value means success.
		R(const _Ty& v) :
			m_err_code(0)
		{
			m_value.construct(v);
		}
		R(_Ty&& v) :
			m_err_code(0)
		{
			m_value.construct(move(v));
		}

		//! Construct the success value explicitly.
		static R success(const _Ty& v)
		{
			R r;
			r.m_err_code = 0;
			r.m_value.construct(v);
			return r;
		}

		//! Construct the success value explicitly.
		static R success(_Ty&& v)
		{
			R r;
			r.m_err_code = 0;
			r.m_value.construct(move(v));
			return r;
		}

		//! Constructs with error code means error.
		R(errcode_t error) :
			m_err_code(error)
		{
			luassert(m_err_code);
		}

		//! Constructs the error explicitly.
		static R failure(errcode_t error)
		{
			R r;
			r.m_err_code = error;
			luassert(r.m_err_code);
			return r;
		}

		R(const R& rhs) :
			m_err_code(rhs.m_err_code)
		{
			if (valid())
			{
				m_value.construct(rhs.m_value.get());
			}
		}
		R(R&& rhs) :
			m_err_code(rhs.m_err_code)
		{
			if (valid())
			{
				m_value.construct(move(rhs.m_value.get()));
			}
		}
		R& operator=(const R& rhs)
		{
			if (m_err_code && !rhs.m_err_code)	// this is failure and rhs is success.
			{
				m_value.construct(rhs.m_value.get());
			}
			else if (!m_err_code && !rhs.m_err_code)	// this is success and rhs is success.
			{
				m_value.get() = rhs.m_value.get();
			}
			else if (!m_err_code && rhs.m_err_code) // this is success and rhs is failure.
			{
				m_value.destruct();
			}
			m_err_code = rhs.m_err_code;
			return *this;
		}
		R& operator=(R&& rhs)
		{
			if (m_err_code && !rhs.m_err_code)	// this is failure and rhs is success.
			{
				m_value.construct(move(rhs.m_value.get()));
			}
			else if (!m_err_code && !rhs.m_err_code)	// this is success and rhs is success.
			{
				m_value.get() = move(rhs.m_value.get());
			}
			else if (!m_err_code && rhs.m_err_code) // this is success and rhs is failure.
			{
				m_value.destruct();
			}
			m_err_code = rhs.m_err_code;
			return *this;
		}

		~R()
		{
			if (!m_err_code)
			{
				m_value.destruct();
			}
		}

		//! Gets the returned value.
		//! This can be called if and only if `valid()` is `true`.
		const _Ty& get() const
		{
			luassert(valid());
			return m_value.get();
		}
		_Ty& get()
		{
			luassert(valid());
			return m_value.get();
		}

		//! Gets the error code.
		//! If the returned value is valid, this returns an empty error object.
		errcode_t errcode() const
		{
			return m_err_code;
		}
	};

	// Specification for void type.
	template <>
	struct [[nodiscard]] R<void>
	{
	private:
		errcode_t m_err_code;

	public:
		//! Tests if the returned value is valid.
		bool valid() const
		{
			return m_err_code == 0;
		}

		//! Success does not require any returned value.
		R() : m_err_code(0) {}

		//! Returns the error object means error.
		R(errcode_t error) :
			m_err_code(error)
		{
			luassert(m_err_code);
		}

		R(const R& rhs) :
			m_err_code(rhs.m_err_code) {}
		R& operator=(const R& rhs)
		{
			m_err_code = rhs.m_err_code;
			return *this;
		}

		//! Gets the error object.
		//! If the returned value is valid, this returns an empty error object.
		errcode_t errcode() const
		{
			return m_err_code;
		}
	};

	using RV = R<void>;

	template <typename _Ty>
	bool succeeded(const R<_Ty>& r)
	{
		return r.valid();
	}

	template <typename _Ty>
	bool failed(const R<_Ty>& r)
	{
		return !r.valid();
	}

	//! A helper function to test the error code of the current function.
	//! @param[in] obj The original result object received from the called function. If the error code of this object is `BasicError::error_object`, 
	//! this call returns the error code stored in the error object of the current thread. If the error code of this object is not `BasicError::error_object`,
	//! this call returns the error code as is.
	template <typename _Ty>
	inline errcode_t get_errcode(const R<_Ty>& obj)
	{
		return get_errcode(obj.errcode());
	}
}

// Static-typed zero-overhead exception mechanism.

#define lures _try_res
#define luerr _try_err
#define lutry errcode_t lures = 0;
#define luthrow(_r) lures = (_r); goto luerr;
#define lucatch luerr: if(lures)
#define lucatchret luerr: if(lures) { return lures; }
#define luexp(_exp) lures = (_exp).errcode(); if(lures) { goto luerr; }
#define luset(_v, _exp)  { auto _res = (_exp); if(!_res.valid()) { lures = _res.errcode(); goto luerr; } (_v) = move(_res.get()); }
#define lulet(_v, _exp) auto _r_##_v = (_exp); if(!(_r_##_v).valid()) { lures = _r_##_v.errcode(); goto luerr; } auto& _v = _r_##_v.get();

#define lures2 _try_res2
#define luerr2 _try_err2
#define lutry2 errcode_t lures2 = 0;
#define luthrow2(_r) lures2 = (_r); goto luerr2;
#define lucatch2 luerr2: if(lures2)
#define lucatchret2 luerr2: if(lures2) { return lures2; }
#define luexp2(_exp) lures2 = (_exp).errcode(); if(lures2) { goto luerr2; }
#define luset2(_v, _exp)  { auto _res = (_exp); if(!_res.valid()) { lures2 = _res.errcode(); goto luerr2; } (_v) = move(_res.get()); }
#define lulet2(_v, _exp) auto _r_##_v = (_exp); if(!(_r_##_v).valid()) { lures2 = _r_##_v.errcode(); goto luerr2; } auto& _v = _r_##_v.get();

#define lures3 _try_res3
#define luerr3 _try_err3
#define lutry3 errcode_t lures3 = 0;
#define luthrow3(_r) lures3 = (_r); goto luerr3;
#define lucatch3 luerr3: if(lures3)
#define lucatchret3 luerr3: if(lures3) { return lures3; }
#define luexp3(_exp) lures3 = (_exp).errcode(); if(lures3) { goto luerr3; }
#define luset3(_v, _exp)  { auto _res = (_exp); if(!_res.valid()) { lures3 = _res.errcode(); goto luerr3; } (_v) = move(_res.get()); }
#define lulet3(_v, _exp) auto _r_##_v = (_exp); if(!(_r_##_v).valid()) { lures3 = _r_##_v.errcode(); goto luerr3; } auto& _v = _r_##_v.get();