#pragma once

#include <utility>
#include <deque>
#include <functional>

namespace nonstd
{
  namespace radix_tree_details
  {
    struct node_base
    {
      node_base * _right = nullptr; //! next on the same level
      node_base * _down = nullptr;  //! first on the next level
      node_base ** _slot = nullptr; //! pointer to self in parent node
      bool _is_value = false;       //! if also holds a value

      void swap(node_base& rhs)
      {
	using std::swap;
	swap(rhs._right, _right);
	swap(rhs._down, _down);
	swap(rhs._slot, _slot);
	swap(rhs._is_value, _is_value);
      }

      friend void swap(node_base& lhs, node_base& rhs)
      {
	using std::swap;
	swap(lhs._right, rhs._right);
	swap(lhs._down, rhs._down);
	swap(lhs._slot, rhs._slot);
	swap(lhs._is_value, rhs._is_value);
      }
    }; //node_base

    struct header
    {
      using size_type = std::size_t;

      header()
	: _node{&_node, &_node, nullptr}
      {}

      header(const header&) = default; //delete?
      header(header&& other) 
	: _node(std::move(other._node))
	, _size(other._size)
      {
	other.reset();
      }

      header& operator=(const header&) = default;
      header& operator=(header&& other)
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

      node_base _node;
      size_type _size = 0;
    }; // header

    struct path
    {
      using storage_type = std::deque<node_base *>;

      path() = default;
      path(const path& other) : _slots(other._slots) {}
      path(path&& other) : _slots(std::move(other._slots)) {}
      path& operator=(const path& other)
      {
	_slots = other._slots;
	return *this;
      }

      path& operator=(const path&& other)
      {
	_slots = std::move(other._slots);
	return *this;
      }

      void swap(path& other)
      {
	using std::swap;
	swap(other._slots, _slots);
      }

      friend void swap(path& lhs, path& rhs)
      {
	using std::swap;
	swap(lhs._slots, rhs._slots);
      }

      node_base * last_rslot_value() const
      {
	if (!_slots.empty())
	  return _slots.back();
	return nullptr;
      }

      node_base * last_dslot_value() const
      {
	node_base * last = last_rslot_value();
	if (last)
	  last = bottom_from(last);
	return last;
      }

      node_base * bottom_from (node_base * current) const
      {
	while (current && current->_down)
	  current = current->_down;
	return current;
      }

      //! returns a node under the current that has _right non-nullptr value
      //! or nullptr if there's none
      node_base * next_dfs_rslot_value_from (node_base * current) const
      {
	node_base * slot = nullptr;

	current = current->_down;
	while (current && !current->_right)
	  current = current->_down;

	if (current && current->_right)
	  slot = current;

	return slot;
      }

      void increment_right()
      {
	while (!_slots.empty() && !_slots.back()->_right)
	  _slots.pop_back();

	if (!_slots.empty() && _slots.back()->_right)
	{
	  _slots.push_back(_slots.back()->_right);

	  node_base * deeper_slot = next_dfs_rslot_value_from(_slots.back());
	  while (deeper_slot)
	  {
	    _slots.push_back(deeper_slot);
	    deeper_slot = next_dfs_rslot_value_from(deeper_slot);
	  }
	}
      }

      std::pair<storage_type::const_iterator, storage_type::const_iterator> get() const
      {
	return std::make_pair(std::begin(_slots), std::end(_slots));
      }

      storage_type _slots;
    }; // path

  } // radix_tree_details
} // nonstd
