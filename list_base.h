#pragma once

#include <utility>

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

      node_base _node;
      size_type _size = 0;
    }; // header

  } // radix_tree_details
} // nonstd
