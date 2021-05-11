// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file List.hpp
* @author JXMaster
* @date 2021/4/27
*/
#pragma once
#include "Memory.hpp"
#include "Algorithm.hpp"

namespace Luna
{
	namespace ListImpl
	{
		template <typename _Ty>
		struct Node
		{
			Node<_Ty>* m_next;
			Node<_Ty>* m_prev;
			_Ty m_elem;
		};

		template <typename _Ty, bool _Const>
		class Iterator
		{
		public:
			using value_type = _Ty;
			using pointer = conditional_t<_Const, const value_type*, value_type*>;
			using reference = conditional_t<_Const, const value_type&, value_type&>;
		private:
			Node<_Ty>* m_cur;
		public:
			Iterator() :
				m_cur(nullptr) {}
			Iterator(const Iterator& rhs) :
				m_cur(rhs.m_cur) {}
			Iterator(Node<_Ty>* pnode) :
				m_cur(pnode) {}

			pointer operator->() const
			{
				return &(m_cur->m_elem);
			}
			reference operator*() const
			{
				return m_cur->m_elem;
			}
			Iterator& operator++()
			{
				m_cur = m_cur->m_next;
				return *this;
			}
			Iterator operator++(int)
			{
				Iterator tmp(*this);
				operator++();
				return tmp;
			}
			Iterator& operator--()
			{
				m_cur = m_cur->m_prev;
				return *this;
			}
			Iterator operator--(int)
			{
				Iterator tmp(*this);
				operator--();
				return tmp;
			}
		};

		template <typename _Ty, bool _Const1, bool _Const2>
		bool operator==(const Iterator<_Ty, _Const1>& lhs, const Iterator<_Ty, _Const2>& rhs)
		{
			return lhs.m_cur == rhs.m_cur;
		}

		template <typename _Ty, bool _Const1, bool _Const2>
		bool operator!=(const Iterator<_Ty, _Const1>& lhs, const Iterator<_Ty, _Const2>& rhs)
		{
			return lhs.m_cur != rhs.m_cur;
		}
	}

	template <typename _Ty>
	class List
	{
	private:
		// -------------------- Begin of ABI compatible part --------------------
		ListImpl::Node* m_first;	// Points to the first node in the node list.
		ListImpl::Node* m_last;		// Points to the last node in the node list.
		usize m_size;				// Number of nodes in the list.
		// --------------------  End of ABI compatible part  --------------------

		void free_all()
		{
			auto node = m_first;
			while (node)
			{
				auto next = node->m_next;
				memdelete(node);
				node = next;
			}
			m_first = nullptr;
			m_last = nullptr;
		}

	public:
		using value_type = _Ty;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = ListImpl::Iterator<value_type, false>;
		using const_iterator = ListImpl::Iterator<value_type, true>;
		using reverse_iterator = ReverseIterator<iterator>;
		using const_reverse_iterator = ReverseIterator<const_iterator>;

		List();
		List(usize count, const_reference value);
		explicit List(usize count);
		template <typename _InputIt>
		List(_InputIt first, _InputIt last);
		List(const List& rhs);
		List(List&& rhs);
		List(InitializerList<value_type> ilist);
		~List();
		List& operator=(const List& rhs);
		List& operator=(List&& rhs);
		List& operator=(InitializerList<value_type> ilist);
		void assign(usize count, const_reference value);
		template <typename _InputIt>
		void assign(_InputIt first, _InputIt last);
		void assign(InitializerList<value_type> ilist);
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;
		reverse_iterator rbegin();
		const_reverse_iterator rbegin() const;
		const_reverse_iterator crbegin() const;
		reverse_iterator rend();
		const_reverse_iterator rend() const;
		const_reverse_iterator crend() const;
		bool empty() const;
		usize size() const;
		void clear();
		iterator insert(const_iterator pos, const value_type& value);
		iterator insert(const_iterator pos, value_type&& value);
		iterator insert(const_iterator pos, usize count, const value_type& reference value);
		template<typename _InputIt>
		iterator insert(const_iterator pos, _InputIt first, _InputIt last);
		iterator insert(const_iterator pos, InitializerList<value_type> ilist);
		template<typename... _Args>
		iterator emplace(const_iterator pos, _Args&&... args);
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void push_back(const value_type& value);
		void push_back(value_type&& value);
		template<typename... _Args>
		reference emplace_back(_Args&&... args);
		void pop_back();
		void push_front(const value_type& value);
		void push_front(value_type&& value);
		template<typename... Args>
		reference emplace_front(Args&&... args);
		void pop_front();
		void resize(usize count);
		void resize(usize count, const value_type& value);
		void swap(List& other);
		void merge(List& other);
		void merge(List&& other);
		template <typename _Compare>
		void merge(List& other, _Compare comp);
		template <typename _Compare>
		void merge(List&& other, _Compare comp);
		void splice(const_iterator pos, List& other);
		void splice(const_iterator pos, List&& other);
		void splice(const_iterator pos, List& other, const_iterator it);
		void splice(const_iterator pos, List&& other, const_iterator it);
		void splice(const_iterator pos, List& other,
			const_iterator first, const_iterator last);
		void splice(const_iterator pos, List&& other,
			const_iterator first, const_iterator last);
		usize remove(const value_type& value);
		template<typename _UnaryPredicate >
		usize remove_if(_UnaryPredicate p);
		void reverse();
		usize unique();
		template<typename _BinaryPredicate>
		usize unique(_BinaryPredicate p);
		void sort();
		template<typename _Compare>
		void sort(_Compare comp);
	};
	template<typename _Ty>
	bool operator==(const List<_Ty>& lhs, const List<_Ty>& rhs);
	template<typename _Ty>
	bool operator!=(const List<_Ty>& lhs, const List<_Ty>& rhs);
	template<typename _Ty>
	void swap(List<_Ty>& lhs, List<_Ty>& rhs);
}