#pragma once

#include "unlikely.h"

#include <new>
#include <utility>
#include <memory>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <bitset>
#include <list>
#include <algorithm>
#include <cstring>

#include "memory.h"

namespace nonstd
{
namespace legacy_allocator
{
#if __cplusplus >= 201103L && __cplusplus <= 201402L
  template<typename T, size_t N>
    struct humble
    {
      using value_type = T;
      using pointer = T*;
      using const_pointer = const T*;
      using reference = T&;
      using const_reference = const T&;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;

      constexpr static size_t block_size = sizeof(T) * N;
      using block = nonstd::memory::memory_block;

      // forbid allocator copying with the container swapping
      //using propagate_on_container_copy_assignment = std::false_type;

      // forbid allocator moving with the container
      //using propagate_on_container_move_assignment = std::false_type;

      // forbid allocator swapping with the container swapping
      //using propagate_on_container_swap = std::false_type;

      template<typename U>
	struct rebind
	{
	  using other = humble<U, N>;
	};

      ~humble()
      {
	if (storage_ && !(--(storage_->_refcnt)))
	  delete storage_;
      }

      humble() = default;

      //! Copy-constructor shares the allocated memory block (if any).
      humble(const humble& other)
	: storage_{other.storage_}
      {
	if (storage_)
	  storage_->_refcnt++;
      }

      //! Move-constructor claims the allocated memory block (if any).
      humble(humble&& other)
	: storage_(other.storage_)
      {
	other.storage_ = nullptr;
      }

      //! Copy assignment shares the allocated memory block (if any).
      humble& operator=(const humble& other)
      {
	storage_ = other.storage_;
	if (storage_)
	  storage_->_refcnt++;
	return *this;
      }

      //! Move assignment claims the allocated memory block (if any).
      humble& operator=(humble&& other)
      {
	storage_ = other.storage_;
	other.storage_ = nullptr;
	return *this;
      }

      pointer allocate(std::size_t n)
      {
	if (!storage_)
	  storage_ = new block(block_size);

	void * p = storage_->allocate(n * sizeof(T));
	if (unlikely(!p))
	  throw std::bad_alloc();
	return reinterpret_cast<T *>(p);
      }

      void deallocate(T *p, std::size_t n)
      {
	if (storage_ && storage_->deallocate(p, n * sizeof(T)))
	{
	  if (storage_->empty())
	  {
	    delete storage_;
	    storage_ = nullptr;
	  }
	}
	else
	{
	  throw std::out_of_range("deallocation outside the storage");
	}
      }

      template<typename U, typename ...Args>
      void construct(U *p, Args &&...args)
      {
	new(p) U(std::forward<Args>(args)...);
      }

      void destroy(T *p)
      {
	p->~T();
      }

      constexpr size_type max_size() const noexcept
      {
	return N;
      }

      size_type size() const
      {
	if (storage_)
	  return storage_->size();
	return 0;
      }

      block * storage_ = nullptr;
    };
#endif // __cplusplus <= 201402L
} // allocator
} //nonstd

//! Allocators for the same memory block and value type are replaceapble
template <typename T, size_t N>
  bool operator==(const nonstd::legacy_allocator::humble<T,N>& lhs, const nonstd::legacy_allocator::humble<T,N>& rhs)
{
  return (lhs.storage_ && rhs.storage_ && (lhs.storage_ == rhs.storage_));
}

//! Allocators for the same memory block and value type are replaceapble
template <typename T, size_t N>
  bool operator!=(const nonstd::legacy_allocator::humble<T,N>& lhs, const nonstd::legacy_allocator::humble<T,N>& rhs)
{
  return !(lhs == rhs);
}

//! Fallback template: in common case allocators aren't replaceapble
template <typename T1, size_t N1, typename T2, size_t N2>
  bool operator==(const nonstd::legacy_allocator::humble<T1,N1>&, const nonstd::legacy_allocator::humble<T2,N2>&)
{
  return false;
}

//! Fallback template: in common case allocators aren't replaceapble
template <typename T1, size_t N1, typename T2, size_t N2>
  bool operator!=(const nonstd::legacy_allocator::humble<T1,N1>&, const nonstd::legacy_allocator::humble<T2,N2>&)
{
  return true;
}
