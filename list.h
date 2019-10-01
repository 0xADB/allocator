#pragma once

#include "list_base.h"

#include <memory>
#include <utility>
#include <initializer_list>

#include <cassert>

namespace nonstd
{
  template <
    typename T
    , typename Allocator = std::allocator<T>
    >
  class list
  {
    private:
      using node_type = list_details::node<T>;
      using node_base_type = typename node_type::base_type;
      using header_type = list_details::header;

    public:
      using value_type = T;
      using reference = T&;
      using const_reference = const T&;
      using difference_type = std::ptrdiff_t;
      using size_type = size_t;
      using iterator = list_details::iterator<T>;
      using const_iterator = list_details::const_iterator<T>;
      using allocator_type = typename Allocator::template rebind<node_type>::other;

    public:

      list() = default;

      explicit list(const allocator_type& alloc)
	: _allocator(alloc)
      {}

      // list(size_type count, const T& value, const allocator_type& alloc = allocator_type())
      //   : _allocator(alloc)
      // {
      //   while(count--)
      //     push_back(value);
      // }

      list(const list& other)
	: _allocator()
	, _header(clone(other._header))
      {
	if (_header.empty() && !other._header.empty())
	  throw std::bad_alloc();
      }

      // list(const list& other, const allocator_type& alloc)
      //   : _allocator(alloc)
      // {
      //   _header._node = list_clone(other.root);
      //   if (_header.empty())
      //     throw std::bad_alloc();
      // }

      list& operator=(const list& other)
      {
	if (&other != this)
	{
	  destroy(_header);
	  _header._node._next = clone(other._header);
	  if (_header.empty() && !other._header.empty())
	    throw std::bad_alloc();
	}
	return *this;
      }

      list(list&& other)
      {
	if (_allocator == other._allocator)
	{
	  _header = std::move(other._header);
	}
	else
	{
	  operator=(other);
	}
      }

      // list(list&& other, const allocator_type& alloc)
      // {
      //   if (alloc == other.get_allocator())
      //   {
      //     _header = std::move(other._header);
      //     _allocator = alloc;
      //   }
      //   else
      //   {
      //     _allocator = alloc;
      //     for (auto&& value : other)
      //       push_back(std::move(value));
      //   }
      // }

      list& operator=(list&& other)
      {
	using std::swap;
	swap(other._header, _header);

	if (_allocator != other._allocator)
	  swap(_allocator, other._allocator);

	return *this;
      }

      list(std::initializer_list<value_type> l)
      {
	list_details::node_base **end = &_header._node._next;
	for (const value_type& value : l)
	{
	  *end = create_node(value);
	  end = &((*end)->_next);
	}
      }

      // list(std::initializer_list<T>&& l, const allocator_type& alloc = allocator_type())
      //   : _allocator(alloc)
      // {
      //   list_details::node_base **end = &_header._node._next;
      //   for (auto&& value : l)
      //   {
      //     *end = create_node(std::forward<value_type>(value));
      //     end = &(*end)->_next;
      //   }
      //   (*end) = &_header._node;
      // }

      template<typename InputIt>
	list(InputIt first, InputIt last)
	{
	  list_details::node_base **end = &_header._node._next;
	  while (first != last)
	  {
	    *end = create_node(std::forward<value_type>(*first));
	    end = &((*end)->_next);
	    ++first;
	  }
	}

      ~list()
      {
	destroy(_header);
      }

      void swap(list& other)
      {
	using std::swap;
	swap(other._header, _header);
      }

      friend void swap (list& lhs, list& rhs)
      {
	using std::swap;
	swap(lhs._header, rhs._header);
	swap(lhs._size, rhs._size);
      }

      friend bool operator==(const list& lhs, const list& rhs)
      {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
      }

      friend bool operator!=(const list& lhs, const list& rhs)
      {
	return !(lhs == rhs);
      }

      iterator begin()
      {
	return iterator{_header._node._next};
      }

      const_iterator begin() const
      {
	return const_iterator{_header._node._next};
      }

      iterator end()
      {
	return iterator{&_header._node};
      }

      const_iterator end() const
      {
	return iterator{&_header._node};
      }

      const_iterator cbegin() const
      {
	return begin();
      }

      const_iterator cend() const
      {
	return end();
      }

      size_type max_size() const
      {
	return _allocator.max_size();
      }

      size_type size() const
      {
	return _header._size;
      }

      bool empty() const
      {
	return (_header._size == 0);
      }

      void push_back(value_type&& value)
      {
	list_details::node_base **end = _header.get_end_slot();
	*end = create_node(std::forward<value_type>(value));
      }

      template<typename... Args>
      void emplace_back(Args&&... args)
      {
	list_details::node_base **end = _header.get_end_slot();
	*end = create_node(std::forward<Args>(args)...);
      }

      void pop_back()
      {
	list_details::node_base **end = _header.get_last_node_slot();
	if (!_header.is_end(*end))
	{
	  auto node = *end;
	  *end = &_header._node;
	  destroy_node(static_cast<typename allocator_type::pointer>(node));
	}
      }

      const_reference back() const
      {
	list_details::node_base * const *end = _header.get_last_node_slot();
	return static_cast<typename allocator_type::pointer>(*end)->_value;
      }

      reference back()
      {
	list_details::node_base **end = _header.get_last_node_slot();
	return static_cast<typename allocator_type::pointer>(*end)->_value;
      }

      const_reference front() const
      {
	return static_cast<const typename allocator_type::pointer>(_header._node._next)->_value;
      }

      reference front()
      {
	return static_cast<typename allocator_type::pointer>(_header._node._next)->_value;
      }

    private:

      template<typename... Args>
	auto create_node(Args&&... args)
      {
	typename allocator_type::pointer node = _allocator.allocate(1);
	_allocator.construct(node, std::forward<Args>(args)...);
	node->_next = &_header._node;
	return node;
      }

      void destroy_node(typename allocator_type::pointer node)
      {
	_allocator.destroy(node);
	_allocator.deallocate(node, 1);
      }

      void destroy(list_details::header& header)
      {
	list_details::node_base * next = header._node._next;
	while (next != &header._node)
	{
	  auto it = next;
	  next = next->_next;
	  destroy_node(static_cast<typename allocator_type::pointer>(it));
	}
      }

      auto clone (const header_type& other_head)
      {
	list_details::header header;
	list_details::node_base ** slot_it = &header._node._next;
	typename allocator_type::const_pointer other_it =
		static_cast<typename allocator_type::const_pointer>(other_head._node._next);

	while (other_it != &other_head._node)
	{
	  *slot_it = create_node(other_it->_value);
	  slot_it = &((*slot_it)->_next);
	  other_it = static_cast<typename allocator_type::const_pointer>(other_it->_next);
	}
	return header._node._next;
      }

    private:
      allocator_type _allocator{};
      header_type _header{};
  };
}
