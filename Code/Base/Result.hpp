// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Result.hpp
* @author JXMaster
* @date 2019/1/7
 */
#pragma once
#include "Assert.hpp"

namespace luna
{
	//! R<_Ty> (represents for Result) can be used to return both the `result_t` result value and the real
	//! function return value along with it. If the result is success, R object holds both the return value 
	//! and the result code `result_t`. The returned value can be fetched by calling `get()` and the result
	//! code can be fetched by calling `result()`. However, if the result is failure, R object holds only 
	//! the `result_t` result code, leaving return value not being constructed. Trying to fetch the return value 
	//! in failure case leads to undefined behavior and triggers an assertion failure in Debug mode.
	//! 
	//! Prefer returning `R` instead of `result_t` even if no return code is used, because in C++17 and later, the
	//! `R` object is declared with [[nodiscard]] attribute, which makes the compiler to generate warnings if you 
	//! forget to handle the possibly error case. `R` has one template specialization for `R<void>`, in which only 
	//! `result_t` is stored, so it will be as efficient as passing `result_t` directly with the fruit of compiling-time
	//! check for possibly ignoring the error handling. You can use `RV` shortcut for `R<void>`.
	template <typename _Ty>
	struct [[nodiscard]] R
	{
	private:
		Unconstructed<_Ty> m_value;
		result_t m_result;

		R() = default;

	public:
		//! Explicitly construct the success result.
		static R success(const _Ty& v, result_t r = s_ok)
		{
			luassert_succeeded(r);
			R res;
			res.m_value.construct(v);
			res.m_result = r;
			return res;
		}
		static R success(_Ty&& v, result_t r = s_ok)
		{
			luassert_succeeded(r);
			R res;
			res.m_value.construct(move(v));
			res.m_result = r;
			return res;
		}

		//! Explicitly construct the failure result.
		static R failure(result_t r)
		{
			luassert_failed(r);
			R res;
			res.m_result = r;
			return res;
		}

		//! Returns the value means success.
		R(const _Ty& v) :
			m_result(s_ok)
		{
			m_value.construct(v);
		}
		R(const _Ty& v, result_t r) :
			m_result(r)
		{
			luassert_succeeded(r);
			m_value.construct(v);
		}
		R(_Ty&& v) :
			m_result(s_ok)
		{
			m_value.construct(move(v));
		}
		R(_Ty&& v, result_t r) :
			m_result(r)
		{
			luassert_succeeded(r);
			m_value.construct(move(v));
		}

		//! Returns the result directly means error for any result whose return value is not `void`.
		R(result_t err) :
			m_result(err)
		{
			luassert_failed(err);
		}

		R(const R& rhs) :
			m_result(rhs.m_result)
		{
			if (succeeded(rhs.m_result))
			{
				m_value.construct(rhs.m_value.get());
			}
		}
		R(R&& rhs) :
			m_result(rhs.m_result)
		{
			if (succeeded(rhs.m_result))
			{
				m_value.construct(move(rhs.m_value.get()));
			}
		}
		R& operator=(const R& rhs)
		{
			if (failed(m_result) && succeeded(rhs.m_result))
			{
				m_value.construct(rhs.m_value.get());
			}
			else if (succeeded(m_result) && succeeded(rhs.m_result))
			{
				m_value.get() = rhs.m_value.get();
			}
			else if (succeeded(m_result) && failed(rhs.m_result))
			{
				m_value.destruct();
			}
			m_result = rhs.m_result;
			return *this;
		}
		R& operator=(R&& rhs)
		{
			if (failed(m_result) && succeeded(rhs.m_result))
			{
				m_value.construct(move(rhs.m_value.get()));
			}
			else if (succeeded(m_result) && succeeded(rhs.m_result))
			{
				m_value.get() = move(rhs.m_value.get());
			}
			else if (succeeded(m_result) && failed(rhs.m_result))
			{
				m_value.destruct();
			}
			m_result = rhs.m_result;
			return *this;
		}

		~R()
		{
			if (succeeded(m_result))
			{
				m_value.destruct();
			}
		}

		//! Gets the value and returns an assertion fail if the result is not success.
		_Ty& get()
		{
			luassert_succeeded(m_result);
			return m_value.get();
		}

		//! Gets the result value.
		result_t result() const
		{
			return m_result;
		}

		operator result_t() const
		{
			return m_result;
		}
	};

	// Specification for void type.
	template <>
	struct [[nodiscard]] R<void>
	{
	private:
		result_t m_result;
	public:
		//! The only way to construct a void result is to pass `result_t` value directly.
		R(result_t res) : m_result(res) {}
		R(const R<void>&) = default;
		R(R<void>&&) = default;
		R& operator=(result_t res)
		{
			m_result = res;
		}
		R& operator=(const R<void>&) = default;
		R& operator=(R<void>&&) = default;

		//! Gets the result value.
		result_t result() const
		{
			return m_result;
		}

		operator result_t() const
		{
			return m_result;
		}
	};

	using RV = R<void>;

	template <typename _Ty>
	bool failed(const R<_Ty>& r)
	{
		return failed(r.result());
	}

	template <typename _Ty>
	bool succeeded(const R<_Ty>& r)
	{
		return succeeded(r.result());
	}
}

// Static-typed zero-overhead exception mechanism.

#define lures _try_res
#define luerr _try_err
#define lutry result_t lures = s_ok;
#define luthrow(_r) lures = (_r); goto luerr;
#define lucatch luerr: if(failed(lures))
#define lucatchret luerr: if(failed(lures)) { return lures; }
#define luexp(_exp) lures = (_exp); if(failed(lures)) { goto luerr; }
#define luset(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures = _res.result(); goto luerr; } (_v) = move(_res.get()); }
#define lulet(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures = _r_##_v.result(); goto luerr; } auto& _v = _r_##_v.get();

#define lures2 _try_res2
#define luerr2 _try_err2
#define lutry2 result_t lures2 = s_ok;
#define luthrow2(_r) lures2 = (_r); goto luerr2;
#define lucatch2 luerr2: if(failed(lures2))
#define lucatchret2 luerr2: if(failed(lures2)) { return lures2; }
#define luexp2(_exp) lures2 = (_exp); if(failed(lures2)) { goto luerr2; }
#define luset2(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures2 = _res.result(); goto luerr2; } (_v) = move(_res.get()); }
#define lulet2(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures2 = _r_##_v.result(); goto luerr2; } auto& _v = _r_##_v.get();

#define lures3 _try_res3
#define luerr3 _try_err3
#define lutry3 result_t lures3 = s_ok;
#define luthrow3(_r) lures3 = (_r); goto luerr3;
#define lucatch3 luerr3: if(failed(lures3))
#define lucatchret3 luerr3: if(failed(lures3)) { return lures3; }
#define luexp3(_exp) lures3 = (_exp); if(failed(lures3)) { goto luerr3; }
#define luset3(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures3 = _res.result(); goto luerr3; } (_v) = move(_res.get()); }
#define lulet3(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures3 = _r_##_v.result(); goto luerr3; } auto& _v = _r_##_v.get();

#define lures4 _try_res4
#define luerr4 _try_err4
#define lutry4 result_t lures4 = s_ok;
#define luthrow4(_r) lures4 = (_r); goto luerr4;
#define lucatch4 luerr4: if(failed(lures4))
#define lucatchret4 luerr4: if(failed(lures4)) { return lures4; }
#define luexp4(_exp) lures4 = (_exp); if(failed(lures4)) { goto luerr4; }
#define luset4(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures4 = _res.result(); goto luerr4; } (_v) = move(_res.get()); }
#define lulet4(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures4 = _r_##_v.result(); goto luerr4; } auto& _v = _r_##_v.get();

#define lures5 _try_res5
#define luerr5 _try_err5
#define lutry5 result_t lures5 = s_ok;
#define luthrow5(_r) lures5 = (_r); goto luerr5;
#define lucatch5 luerr5: if(failed(lures5))
#define lucatchret5 luerr5: if(failed(lures5)) { return lures5; }
#define luexp5(_exp) lures5 = (_exp); if(failed(lures5)) { goto luerr5; }
#define luset5(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures5 = _res.result(); goto luerr5; } (_v) = move(_res.get()); }
#define lulet5(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures5 = _r_##_v.result(); goto luerr5; } auto& _v = _r_##_v.get();

#define lures6 _try_res6
#define luerr6 _try_err6
#define lutry6 result_t lures6 = s_ok;
#define luthrow6(_r) lures6 = (_r); goto luerr6;
#define lucatch6 luerr6: if(failed(lures6))
#define lucatchret6 luerr6: if(failed(lures6)) { return lures6; }
#define luexp6(_exp) lures6 = (_exp); if(failed(lures6)) { goto luerr6; }
#define luset6(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures6 = _res.result(); goto luerr6; } (_v) = move(_res.get()); }
#define lulet6(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures6 = _r_##_v.result(); goto luerr6; } auto& _v = _r_##_v.get();

#define lures7 _try_res7
#define luerr7 _try_err7
#define lutry7 result_t lures7 = s_ok;
#define luthrow7(_r) lures7 = (_r); goto luerr7;
#define lucatch7 luerr7: if(failed(lures7))
#define lucatchret7 luerr7: if(failed(lures7)) { return lures7; }
#define luexp7(_exp) lures7 = (_exp); if(failed(lures7)) { goto luerr7; }
#define luset7(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures7 = _res.result(); goto luerr7; } (_v) = move(_res.get()); }
#define lulet7(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures7 = _r_##_v.result(); goto luerr7; } auto& _v = _r_##_v.get();

#define lures8 _try_res8
#define luerr8 _try_err8
#define lutry8 result_t lures8 = s_ok;
#define luthrow8(_r) lures8 = (_r); goto luerr8;
#define lucatch8 luerr8: if(failed(lures8))
#define lucatchret8 luerr8: if(failed(lures8)) { return lures8; }
#define luexp8(_exp) lures8 = (_exp); if(failed(lures8)) { goto luerr8; }
#define luset8(_v, _exp)  { auto _res = (_exp); if(failed(_res)) { lures8 = _res.result(); goto luerr8; } (_v) = move(_res.get()); }
#define lulet8(_v, _exp) auto _r_##_v = (_exp); if(failed(_r_##_v)) { lures8 = _r_##_v.result(); goto luerr8; } auto& _v = _r_##_v.get();