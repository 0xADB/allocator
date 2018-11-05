#ifndef RADIX_TREE_H
#define RADIX_TREE_H

#include <utility>
#include <memory>
#include <functional>

namespace nonstd
{
  namespace details
  {
    struct radix_tree_node_base_t
    {
      radix_tree_node_base_t * _right = nullptr;
      radix_tree_node_base_t * _down = nullptr;
      radix_tree_node_base_t ** _slot = nullptr;

      void swap(radix_tree_node_base_t& rhs)
      {
	using std::swap;
	swap(rhs._right, _right);
	swap(rhs._down, _down);
	swap(rhs._slot, _slot);
      }

      friend void swap(radix_tree_node_base_t& lhs, radix_tree_node_base_t& rhs)
      {
	using std::swap;
	swap(lhs._right, rhs._right);
	swap(lhs._down , rhs._down );
	swap(lhs._slot , rhs._slot );
      }
    };

    template<typename T>
      struct radix_tree_node_t : public radix_tree_node_base_t
      {
	T _value;

	void swap(radix_tree_node_t& rhs)
	{
	  using std::swap;
	  swap(static_cast<radix_tree_node_base_t>(rhs), static_cast<radix_tree_node_base_t>(*this));
	  swap(rhs._value, _value);
	}

	friend void swap(radix_tree_node_t& lhs, radix_tree_node_t& rhs)
	{
	  using std::swap;
	  swap(static_cast<radix_tree_node_base_t>(rhs), static_cast<radix_tree_node_base_t>(lhs));
	  swap(lhs._value, rhs._value);
	}
      };

    struct radix_tree_header_t
    {
      using size_type = size_t;

      radix_tree_node_base_t _node;
      size_type _size;

      radix_tree_header_t()
	: _node{&_node, &_node, nullptr}
	, _size(0)
      {}

      radix_tree_header_t(const radix_tree_header_t&) = default; //delete?
      radix_tree_header_t(radix_tree_header_t&& other) 
	: _node(std::move(other._node))
	, _size(other._size)
      {
	other.reset();
      }

      radix_tree_header_t& operator=(const radix_tree_header_t&) = default;
      radix_tree_header_t& operator=(radix_tree_header_t&& other)
      {
	_node = other._node;
	_size = other._size;
	other.reset();
	return *this;
      }

      void reset()
      {
	_node._right = &_node;
	_node._down = &_node;
	_node._slot = nullptr;
	_size = 0;
      }

      void swap(radix_tree_header_t& other)
      {
	using std::swap;
	swap(other._node, _node);
	swap(other._size, _size);
      }

      friend void swap(radix_tree_header_t& lhs, radix_tree_header_t& rhs)
      {
	using std::swap;
	swap(lhs._node, rhs._node);
	swap(lhs._size, rhs._size);
      }
    };

    radix_tree_node_base_t * radix_tree_increment(radix_tree_node_base_t *); // TODO
    const radix_tree_node_base_t * radix_tree_increment(const radix_tree_node_base_t *); //TODO

    template<typename T>
      struct iterator
      {
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using node_type = radix_tree_node_t<T>;
	using node_base_type = radix_tree_node_base_t;

	iterator() : _node(nullptr) {}
	iterator(const iterator&) = default;
	iterator(iterator&&) = default;
	iterator& operator=(const iterator&) = default;
	iterator& operator=(iterator&&) = default;
	~iterator(){}

	explicit iterator(node_base_type * node) : _node(node) {}

	reference operator*() const
	{
	  return (static_cast<node_type*>(_node))->_value;
	}

	pointer operator->() const
	{
	  return &(static_cast<node_type*>(_node))->_value;
	}

	friend void swap(iterator& lhs, iterator& rhs);

	iterator& operator++()
	{
	  _node = radix_tree_increment(_node);
	  return *this;
	}

	iterator operator++(int)
	{
	  iterator result(*this);
	  ++(*this);
	  return result;
	}

	friend bool operator==(const iterator& lhs, const iterator& rhs)
	{
	  return (lhs._node == rhs._node);
	}

	friend bool operator!=(const iterator& lhs, const iterator& rhs)
	{
	  return (lhs._node != rhs._node);
	}

	void swap (iterator& rhs)
	{
	  using std::swap;
	  swap(rhs._node, _node);
	}

	friend void swap (iterator& lhs, iterator& rhs)
	{
	  using std::swap;
	  swap(lhs._node, rhs._node);
	}

	node_base_type * _node;
      };

    template<typename T>
      struct const_iterator
      {
	using value_type = T;
	using reference = const T&;
	using pointer = const T*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using node_type = radix_tree_node_t<T>;
	using node_base_type = radix_tree_node_base_t;

	const_iterator() : _node(nullptr) {}
	const_iterator(const const_iterator&) = default;
	const_iterator(const_iterator&&) = default;
	const_iterator& operator=(const const_iterator&) = default;
	const_iterator& operator=(const_iterator&&) = default;
	~const_iterator(){}

	explicit const_iterator(const node_base_type * node) : _node(node) {}
	const_iterator(const iterator<T>& it) : _node(it._node) {}

	reference operator*() const
	{
	  return (static_cast<node_type*>(_node))->_value;
	}

	pointer operator->() const
	{
	  return &(static_cast<node_type*>(_node))->_value;
	}

	friend void swap(const_iterator& lhs, const_iterator rhs);

	const_iterator& operator++()
	{
	  _node = radix_tree_increment(_node);
	  return *this;
	}

	const_iterator operator++(int)
	{
	  const_iterator result(*this);
	  ++(*this);
	  return result;
	}

	friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
	{
	  return (lhs._node == rhs._node);
	}

	friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
	{
	  return (lhs._node != rhs._node);
	}

	void swap (const_iterator& rhs)
	{
	  using std::swap;
	  swap(rhs._node, _node);
	}

	friend void swap (const_iterator& lhs, const_iterator& rhs)
	{
	  using std::swap;
	  swap(lhs._node, rhs._node);
	}

	const node_base_type * _node;
      };
  }

  template <
    typename T
    , typename Compare = std::less<T>
    , typename Allocator = std::allocator<T>
    >
  class radix_tree
  {
    private:
      using node_type = details::radix_tree_node_t<T>;

    public:
      using value_type = T;
      using reference = T&;
      using const_reference = const T&;
      using difference_type = ssize_t;
      using size_type = size_t;
      using iterator = details::iterator<T>;
      using const_iterator = details::const_iterator<T>;

    public:

      radix_tree()
	: _header()
      {}

      radix_tree(const radix_tree& other)
	: _header(radix_tree_clone(other.root))
      {
	if (!_header)
	  throw std::bad_alloc(std::string(__PRETTY_FUNCTION__) + ": failed to clone");
      }

      radix_tree& operator=(const radix_tree& other)
      {
	if (&other != this)
	{
	  radix_tree_destroy(_header);
	  _header = radix_tree_clone(other._header);
	  if (!_header)
	    throw std::bad_alloc(std::string(__PRETTY_FUNCTION__) + ": failed to clone");
	}
	return *this;
      }

      radix_tree(radix_tree&& other)
	: _header(std::move(other._header))
      {}

      radix_tree& operator=(radix_tree&& other)
      {
	using std::swap;
	swap(other._header, _header);
	return *this;
      }

      ~radix_tree()
      {
	radix_tree_destroy(_header);
      }

      void swap(radix_tree& other)
      {
	using std::swap;
	swap(other._header, _header);
      }

      friend void swap (radix_tree& lhs, radix_tree& rhs)
      {
	using std::swap;
	swap(lhs._header, rhs._header);
	swap(lhs._size, rhs._size);
      }

      friend bool operator==(const radix_tree& lhs, const radix_tree& rhs)
      {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
      }

      friend bool operator!= (const radix_tree& lhs, const radix_tree& rhs)
      {
	return !(lhs == rhs);
      }

      iterator begin()
      {
	return iterator{_header._node._down};
      }

      const_iterator begin() const
      {
	return const_iterator{_header._node._down};
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

    private:
      static details::radix_tree_node_base_t * radix_tree_clone(details::radix_tree_node_base_t * root); // TODO
      static void radix_tree_destroy(details::radix_tree_node_base_t * root); // TODO

    private:
      details::radix_tree_header_t _header;
      Compare compare;
      Allocator allocator;
  };
}

#endif // RADIX_TREE_H
