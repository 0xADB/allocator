#pragma once

#include "list_base.h"

#include <memory>
#include <utility>

namespace nonstd
{
  namespace list_details
  {
    template<typename T>
      struct node : public node_base
      {
	using base_type = node_base;

	T _value{};

	void swap(node& rhs)
	{
	  using std::swap;
	  swap(rhs._next, this->_next);
	  swap(rhs._value, _value);
	}

	friend void swap(node& lhs, node& rhs)
	{
	  using std::swap;
	  swap(lhs._next, rhs._next);
	  swap(lhs._value, rhs._value);
	}
      };

    template<typename T>
      struct iterator
      {
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using node_type = node<T>;

	iterator() = default;
	iterator(const iterator&) = default;
	iterator(iterator&&) = default;
	iterator& operator=(const iterator&) = default;
	iterator& operator=(iterator&&) = default;
	~iterator(){}

	explicit iterator(node_base * node) : _node(node) {}

	reference operator*() const
	{
	  return (static_cast<node_type*>(_node)->_value);
	}

	pointer operator->() const
	{
	  return &((static_cast<node_type*>(_node))->_value);
	}

	iterator& operator++()
	{
	  if (_node)
	    _node = _node->_next;
	  return *this;
	}

	iterator operator++(int)
	{
	  iterator result(*this);
	  ++(*this);
	  return result;
	}

	template<typename U>
	friend bool operator==(const iterator<U>& lhs, const iterator<U>& rhs)
	{
	  return (lhs._path == rhs._path);
	}

	template<typename U>
	friend bool operator!=(const iterator<U>& lhs, const iterator<U>& rhs)
	{
	  return (lhs._path != rhs._path);
	}

	void swap (iterator& rhs)
	{
	  using std::swap;
	  swap(rhs._node, _node);
	}

	template<typename U>
	friend void swap (iterator<U>& lhs, iterator<U>& rhs)
	{
	  using std::swap;
	  swap(lhs._node, rhs._node);
	}

	node_base * _node;
      };

    template<typename T>
      struct const_iterator
      {
	using value_type = T;
	using reference = const T&;
	using pointer = const T*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using node_type = node<T>;

	const_iterator() : _node(nullptr) {}
	const_iterator(const const_iterator&) = default;
	const_iterator(const_iterator&&) = default;
	const_iterator& operator=(const const_iterator&) = default;
	const_iterator& operator=(const_iterator&&) = default;
	~const_iterator(){}

	explicit const_iterator(const node_base * node) : _node(node) {}

	reference operator*() const
	{
	  return (static_cast<node_type*>(_node))->_value;
	}

	pointer operator->() const
	{
	  return &(static_cast<node_type*>(_node))->_value;
	}

	const_iterator& operator++()
	{
	  if (_node)
	    _node = _node->_next;
	  return *this;
	}

	const_iterator operator++(int)
	{
	  const_iterator result(*this);
	  ++(*this);
	  return result;
	}

	template<typename U>
	friend bool operator==(const const_iterator<U>& lhs, const const_iterator<U>& rhs)
	{
	  return (lhs._node == rhs._node);
	}

	template<typename U>
	friend bool operator!=(const const_iterator<U>& lhs, const const_iterator<U>& rhs)
	{
	  return (lhs._node != rhs._node);
	}

	void swap (const_iterator& rhs)
	{
	  using std::swap;
	  swap(rhs._node, _node);
	}

	template<typename U>
	friend void swap (const_iterator<U>& lhs, const_iterator<U>& rhs)
	{
	  using std::swap;
	  swap(lhs._node, rhs._node);
	}

	const node_base * _node;
      };
  }

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
      using node_allocator = typename Allocator::template rebind<node_type>::other;

    public:
      using value_type = T;
      using reference = T&;
      using const_reference = const T&;
      using difference_type = std::ptrdiff_t;
      using size_type = size_t;
      using iterator = list_details::iterator<T>;
      using const_iterator = list_details::const_iterator<T>;

    public:

      list() : _header(){}

      list(const list& other)
	: _header(list_clone(other.root))
      {
	if (_header.empty())
	  throw std::bad_alloc(std::string(__PRETTY_FUNCTION__) + ": failed to clone");
      }

      list& operator=(const list& other)
      {
	if (&other != this)
	{
	  destroy(_header);
	  _header = clone(other._header);
	  if (_header.empty())
	    throw std::bad_alloc(std::string(__PRETTY_FUNCTION__) + ": failed to clone");
	}
	return *this;
      }

      list(list&& other)
	: _header(std::move(other._header))
      {}

      list& operator=(list&& other)
      {
	using std::swap;
	swap(other._header, _header);
	return *this;
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

      size_type max_size() const
      {
	return allocator.max_size();
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
	list_details::node_base **end = &_header._node._next;
	while (*end != &_header._node)
	  end = &((*end)->_next);
	*end = create_node(std::forward(value));
	(*end)->_next = &_header._node;
      }

      void pop_back()
      {
	list_details::node_base **end = &_header._node._next;
	while (*end != &_header._node && (*end)->_next != &_header._node)
	  end = &((*end)->_next);

	if ((*end)->_next != &_header._node)
	{
	  auto node = (*end)->_next;
	  (*end)->_next = &_header._node;
	  destroy_node(node);
	}
      }

    private:
      template<typename... Args>
	auto create_node(Args&&... args)
      {
	typename node_allocator::pointer node = allocator.allocate(1);
	allocator.construct(node, std::forward<Args>(args)...);
	return node;
      }

      void destroy_node(typename node_allocator::pointer node)
      {
	allocator.destroy(node);
	allocator.deallocate(node, sizeof(node_type));
      }

      void destroy(list_details::header& header)
      {
	list_details::node_base * next = header._node._next;
	while (next != &header._node)
	{
	  auto it = next;
	  next = it->_next;
	  destroy_node(static_cast<typename node_allocator::pointer>(it));
	}
      }

      auto clone (const typename node_allocator::pointer other_head)
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
      header_type _header;
      node_allocator allocator{};
  };
}
