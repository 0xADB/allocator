#pragma once

#if __cplusplus > 201402L

#include "list_base.h"
#include <memory_resource>

#include <memory>
#include <utility>
#include <initializer_list>

namespace nonstd
{
  namespace pmr
  {
    template <typename T>
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
	  using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

	public:

	  list() = default;

	  explicit list(allocator_type& alloc)
	    : _allocator(alloc)
	  {}

	  list(size_type count, const T& value, allocator_type alloc = {})
	    : _allocator(alloc)
	  {
	    while(count--)
	      push_back(value);
	  }

	  list(const list& other, allocator_type alloc = {})
	    : _allocator(alloc)
	  {
	    operator=(other);
	  }

	  list(list&& other)
	    : _allocator(other.get_allocator())
	  {
	    operator=(std::move(other));
	  }

	  list(list&& other, allocator_type alloc)
	    : _allocator(alloc)
	  {
	    operator=(std::move(other));
	  }

	  list& operator=(const list& other)
	  {
	    if (&other != this)
	    {
	      destroy(_header);
	      for (const T& v : other)
		push_back(v);
	    }
	    return *this;
	  }

	  list& operator=(list&& other)
	  {
	    if (&other != this)
	    {
	      if (_allocator == other._allocator)
	      {
		destroy(_header); // why?
		swap(other);
	      }
	      else
	      {
		operator=(other);
	      }
	    }
	    return *this;
	  }

	  list(std::initializer_list<T>&& l, const allocator_type& alloc = {})
	    : _allocator(alloc)
	  {
	    for (auto&& value : l)
	      push_back(std::forward<value_type>(value));
	  }

	  template<typename InputIt>
	    list(InputIt first, InputIt last, const allocator_type& alloc = {})
	      : _allocator(alloc)
	    {
	      while (first != last)
	      {
		push_back(*first);
		++first;
	      }
	    }

	  ~list()
	  {
	    erase(begin(), end());
	  }

	  void swap(list& other) noexcept
	  {
	    using std::swap;
	    swap(other._header, _header);
	  }

	  friend void swap (list& lhs, list& rhs)
	  {
	    lhs.swap(rhs);
	  }

	  friend bool operator==(const list& lhs, const list& rhs)
	  {
	    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	  }

	  friend bool operator!= (const list& lhs, const list& rhs)
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

	  // Size_type max_size() const
	  // {
	  //   return _allocator.max_size();
	  // }

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
	    (*end)->_next = &_header._node;
	  }

	  void pop_back()
	  {
	    list_details::node_base **end = _header.get_last_node_slot();
	    if (!_header.is_end(*end))
	    {
	      auto node = *end;
	      *end = &_header._node;
	      destroy_node(static_cast<node_type *>(node));
	    }
	  }

	  const_reference back() const
	  {
	    list_details::node_base * const *end = _header.get_last_node_slot();
	    return static_cast<node_type *>(*end)->_value;
	  }

	  reference back()
	  {
	    list_details::node_base **end = _header.get_last_node_slot();
	    return static_cast<node_type *>(*end)->_value;
	  }

	  const_reference front() const
	  {
	    return static_cast<node_type *>(_header._node._next)->_value;
	  }

	  reference front()
	  {
	    return static_cast<node_type *>(_header._node._next)->_value;
	  }

	private:

	  template<typename... Args>
	    auto create_node(Args&&... args)
	    {
	      node_type * node =
		static_cast<node_type *>(
		    _allocator.resource()->allocate(sizeof(node_type), alignof(node_type))
		    );
	      try
	      {
		_allocator.construct(std::addressof(node->_value), std::forward<Args>(args)...);
	      }
	      catch(...)
	      {
		_allocator.resource()->deallocate(node, sizeof(node_type), alignof(node_type));
		throw;
	      }
	      return node;
	    }

	  void destroy_node(node_type * node)
	  {
	    _allocator.destroy(std::addressof(node->_value));
	    _allocator.resource()->deallocate(node, sizeof(node_type), alignof(node_type));
	  }

	  void destroy(list_details::header& header)
	  {
	    list_details::node_base * next = header._node._next;
	    while (next != &header._node)
	    {
	      auto it = next;
	      next = next->_next;
	      destroy_node(static_cast<node_type *>(it));
	    }
	  }

	  auto clone (const node_type * other_head)
	  {
	    list_details::header header;
	    list_details::node_base ** it = &header._node._next;

	    for (auto other_it = other_head
		; other_it != nullptr
		; it = &((*it)->_next)
		, other_it = other_it->_next
		)
	    {
	      *it = create_node(other_it->_value);
	    }
	    return header._node._next;
	  }

	private:
	  header_type _header{};
	  allocator_type _allocator{};
      }; // list
  } // pmr
} //nonstd

#endif // __cplusplus > 201402L
