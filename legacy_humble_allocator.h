#pragma once

#if __cplusplus >= 201103L && __cplusplus <= 201402L
#include "unlikely.h"

#include <utility>
#include <memory>
#include <cassert>

#include "legacy_memory_block.h"

namespace nonstd
{
namespace legacy
{
  template<typename T, size_t N>
    struct humble_allocator
    {
      using value_type = T;
      using pointer = T*;
      using const_pointer = const T*;
      using reference = T&;
      using const_reference = const T&;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;

      constexpr static size_t block_size = sizeof(T) * N;
      using block = nonstd::legacy::memory_block;

      template<typename U>
	struct rebind
	{
	  using other = humble_allocator<U, N>;
	};

      ~humble_allocator()
      {
	if (storage_ && !(--(storage_->_refcnt)))
	  delete storage_;
      }

      humble_allocator() = default;

      humble_allocator(const humble_allocator& /*other*/)
	// : storage_(other.storage_)
      {
	// if (storage_)
	//   ++storage_->_refcnt;
      }

      //! Move-constructor claims the allocated memory block (if any).
      humble_allocator(humble_allocator&& other)
	: storage_(other.storage_)
      {
	other.storage_ = nullptr;
      }

      template<typename U>
	humble_allocator(const humble_allocator<U,N>& /*other*/)
	  // : storage_(other.storage_)
      {
	// if (storage_)
	//   ++storage_->_refcnt;
      }

      humble_allocator& operator=(const humble_allocator& /*other*/) = delete;
      // {
      //   if (storage_ && !(--(storage_->_refcnt)))
      //     delete storage_;
      //   storage_ = other.storage_;
      //   ++(storage_->_refcnt);
      //   return *this;
      // }

      humble_allocator& operator=(humble_allocator&& /*other*/) = delete;
      // {
      //   if (storage_ && !(--(storage_->_refcnt)))
      //     delete storage_;
      //   storage_ = other.storage_;
      //   other.storage_ = nullptr;
      //   return *this;
      // }

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
	if (!storage_ || !storage_->deallocate(p, n * sizeof(T)))
	  throw std::out_of_range("deallocation outside the storage");
      }

      template<typename U, typename... Args>
      void construct(U *p, Args&&... args)
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
} // legacy
} //nonstd

//! Allocators for the same memory block and value type are replaceapble
template <typename T, size_t N>
  bool operator==(const nonstd::legacy::humble_allocator<T,N>& lhs, const nonstd::legacy::humble_allocator<T,N>& rhs)
{
  return (lhs.storage_ && rhs.storage_ && (lhs.storage_ == rhs.storage_));
}

//! Allocators for the same memory block and value type are replaceapble
template <typename T, size_t N>
  bool operator!=(const nonstd::legacy::humble_allocator<T,N>& lhs, const nonstd::legacy::humble_allocator<T,N>& rhs)
{
  return !(lhs == rhs);
}

//! Fallback template: in common case allocators aren't replaceapble
template <typename T1, size_t N1, typename T2, size_t N2>
  bool operator==(const nonstd::legacy::humble_allocator<T1,N1>&, const nonstd::legacy::humble_allocator<T2,N2>&)
{
  return false;
}

//! Fallback template: in common case allocators aren't replaceapble
template <typename T1, size_t N1, typename T2, size_t N2>
  bool operator!=(const nonstd::legacy::humble_allocator<T1,N1>&, const nonstd::legacy::humble_allocator<T2,N2>&)
{
  return true;
}

#endif // __cplusplus <= 201402L
