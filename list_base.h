#pragma once

#include <utility>
#include <iterator>

namespace nonstd
{
  namespace list_details
  {
    struct node_base
    {
      node_base * _next = nullptr;
    }; //node_base

    struct header
    {
      using size_type = std::size_t;

      header()
	: _node{&_node}
      {}

      header(const header&) = delete;
      header(header&& other)
	: _node(std::move(other._node))
	, _size(other._size)
      {
	other.reset();
      }

      header& operator=(const header&) = delete;
      header& operator=(header&& other)
      {
	_node = other._node;
	_size = other._size;
	other.reset();
	return *this;
      }

      void reset()
      {
	_node._next = &_node;
	_size = 0;
      }

      void swap(header& other)
      {
	using std::swap;
	swap(other._node, _node);
	swap(other._size, _size);
      }

      friend void swap(header& lhs, header& rhs)
      {
	using std::swap;
	swap(lhs._node, rhs._node);
	swap(lhs._size, rhs._size);
      }

      bool empty() const
      {
	return (!_node._next);
      }

      bool is_end(const node_base * node) const
      {
	return (node == &_node);
      }

      //! returns pointer to the const pointer to the last node
      node_base * const * get_last_node_slot() const
      {
	node_base * const *end = &_node._next;
	if (!is_end(*end))
	{
	  while (!is_end((*end)->_next))
	    end = &((*end)->_next);
	}
	return end;
      }

      //! returns pointer to the const pointer to the last node
      node_base ** get_last_node_slot()
      {
	node_base **end = &_node._next;
	if (!is_end(*end))
	{
	  while (!is_end((*end)->_next))
	    end = &((*end)->_next);
	}
	return end;
      }

      //! returns pointer to the pointer to the end
      node_base * const * get_end_slot() const
      {
	node_base *const * end = &_node._next;
	while (!is_end(*end))
	  end = &((*end)->_next);
	return end;
      }

      //! returns pointer to the pointer to the end
      node_base **get_end_slot()
      {
	node_base **end = &_node._next;
	while (!is_end(*end))
	  end = &((*end)->_next);
	return end;
      }


      node_base _node;
      size_type _size = 0;
    }; // header

    template<typename T>
      struct node : public node_base
    {
      using value_type = T;
      using base_type = node_base;

      T _value{};

      node(value_type&& value)
	: _value(std::forward<value_type>(value))
      {}

      node() = default;
      node(const node&) = default;
      node& operator=(const node&) = default;
      node(node&&) = default;
      node& operator=(node&&) = default;
      ~node(){}

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
	    return (lhs._node == rhs._node);
	  }

	template<typename U>
	  friend bool operator!=(const iterator<U>& lhs, const iterator<U>& rhs)
	  {
	    return (lhs._node != rhs._node);
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


  } // list_details
} // nonstd
