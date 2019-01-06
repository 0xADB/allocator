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

namespace nonstd
{
namespace allocator
{
#if __cplusplus >= 201103L && __cplusplus <= 201402L
  namespace details
  {
    struct memory_block
    {
      using byte_type = unsigned char;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;

      byte_type * _storage = nullptr;
      byte_type * _end = nullptr;
      byte_type * _storage_end = nullptr;

      memory_block() = default;

      memory_block(size_type N)
	: _storage(static_cast<byte_type *>(::operator new(N)))
	, _end(_storage)
	, _storage_end(_storage + N)
      {}

      // TODO: do I have to memcpy objects
      // TODO: and if I do will they be in a valid state after that
      memory_block(const memory_block& other)
	: _storage(static_cast<byte_type *>(::operator new(other.capacity())))
	, _end(_storage)
	, _storage_end(_storage + other.capacity())
      {}

      // TODO: 1) do I have to destroy stored objects first?
      // TODO: 2) do I have to copy-contruct objects from other
      // TODO:    and if I do will they be in a valid state after that
      memory_block& operator=(const memory_block& other) = delete;
      // {
      //   return *this;
      // }

      memory_block(memory_block&& other)
	: _storage(other._storage)
	, _end(other._end)
	, _storage_end(other._storage_end)
      {
	other._storage = nullptr;
	other._end = nullptr;
	other._storage_end = nullptr;
      }

      memory_block& operator=(memory_block&& other)
      {
	other.swap(*this);
	return *this;
      }

      ~memory_block()
      {
	if (_storage)
	  ::operator delete(_storage);
      }

      void swap(memory_block& other)
      {
	std::swap(_storage, other._storage);
	std::swap(_end, other._end);
	std::swap(_storage_end, other._storage_end);
      }

      friend void swap(memory_block& lhs, memory_block& rhs)
      {
	lhs.swap(rhs);
      }

      bool is_pointed_by(const void * p, size_type size = 0) const
      {
	return ((_storage <= reinterpret_cast<const byte_type *>(p))
	    && (reinterpret_cast<const byte_type *>(p) < _end)
	    && (size == 0 || reinterpret_cast<const byte_type *>(p) + size <= _end)
	    );
      }

      bool initialized() const
      {
	return (_storage && _end && _storage_end);
      }

      bool empty() const
      {
	return (_end == _storage);
      }

      size_type size() const
      {
	return (_end - _storage);
      }

      size_type capacity() const
      {
	return (_storage_end - _storage);
      }

      void * allocate(size_type n)
      {
	void * p = nullptr;
	if (_end + n <= _storage_end)
	{
	  p = _end;
	  _end += n;
	}
	return p;
      }

      bool deallocate(void * p, size_type size)
      {
	if (_storage <= p && p < _end)
	{
	  // TODO: how should I actually handle deallocation?
	  // TODO: 1) keep track on occupied slots breaks contiguousness guarantee for vector and deque
	  // TODO: 2) memmoving could work for contiguous container, I guess,
	  // TODO:    but I doubt it could for node-based containers

	  // FIXME: double-free or corruption on commented out code
	  // std::memmove(
	  //     p
	  //     , reinterpret_cast<byte_type *>(p) + size
	  //     , static_cast<difference_type>(_end - reinterpret_cast<byte_type *>(p)) + size
	  //     );
	  // _end -= size;
	  // assert(_storage <= _end);
	  return true;
	}
	return false;
      }

      void * end()
      {
	return _end;
      }

      const void * end() const
      {
	return _end;
      }

      void * begin()
      {
	return _storage;
      }

      const void * begin() const
      {
	return _storage;
      }

      void * advance(void * p, size_type n) const
      {
	p = reinterpret_cast<byte_type *>(p) + n;
	if (p <= _end)
	  return p;
	return _end;
      }
    };
  }

  template<typename T, size_t N>
  struct humble
  {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;

    constexpr static size_t block_size = sizeof(T) * N;
    using block = details::memory_block;

    // forbid allocator copying  with the container swapping
    using propagate_on_container_copy_assignment = std::false_type;

    // forbid allocator moving   with the container
    using propagate_on_container_move_assignment = std::false_type;

    // forbid allocator swapping with the container swapping
    using propagate_on_container_swap = std::false_type;

    template<typename U>
    struct rebind
    {
      using other = humble<U, N>;
    };

    ~humble() {}

    humble()
      : storage_{block(block_size)}
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    humble(const humble&)
      : storage_{block(block_size)}
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
      //humble::copy_values_from(other);
    }

    humble(humble&& other)
      : storage_(std::move(other.storage_))
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    // it is required by the Allocator concept
    template<typename U>
      humble(const humble<U,N>&)
	: storage_{block(humble<U,N>::block_size)}
      {
	//std::cout << __PRETTY_FUNCTION__ << std::endl;
	//humble::copy_values_from(other);
      }

    humble& operator=(const humble& other) = delete;
    // {
    //   std::cout << __PRETTY_FUNCTION__ << std::endl;
    //   destroy_storage();
    //   humble::copy_values_from(other);
    //   return *this;
    // }

    humble& operator=(humble&& other)
    {
      //std::cout << __PRETTY_FUNCTION__ << std::endl;
      std::swap(storage_, other.storage_);
      return *this;
    }

    pointer allocate(std::size_t n)
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
      void * p = storage_.front().allocate(n * sizeof(T)); //TODO: choose block instead
      if (unlikely(!p))
	throw std::bad_alloc(); //TODO: add new block to storage instead
      return reinterpret_cast<T *>(p);
    }

    void deallocate(T *p, std::size_t n)
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
      if (unlikely(!storage_.front().deallocate(p, n * sizeof(T)))) // TODO: choose block instead
	throw std::out_of_range("deallocation outside the storage");
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args &&...args)
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
      new(p) U(std::forward<Args>(args)...);
    }

    void destroy(T *p)
    {
      // std::cout << __PRETTY_FUNCTION__ << std::endl;
      p->~T();
    }

    constexpr size_type max_size() const noexcept
    {
      return N;
    }

    size_type size() const
    {
      return storage_.front().size(); // TODO: sum all blocks sizes instead
    }

    // TODO: container has to do this, right?
    // void destroy_storage()
    // {
    //   for (auto& mb : storage_)
    //   {
    //     for (auto * it = mb.begin(), endIt = mb.end()
    //         ; it != endIt
    //         ; it = mb.advance(it, sizeof(T))
    //         )
    //     {
    //       destroy(it);
    //     }
    //   }
    // }

    // template<typename U>
    // void copy_values_from(const humble<U,N>& other)
    // {
    //   if (!storage_.front().empty())
    //     destroy_storage();
    //   storage_ = {block(humble<U,N>::block_size)};

    // TODO: isn't this a container job?
    // TODO: check whether std::list nodes of destination
    //   auto& oMb = this->storage_.front();
    //   auto& iMb = other.storage_.front();

    //   for (void * oIt = oMb.begin()
    //       , * oEndIt  = oMb.end()
    //       , * iIt     = iMb.begin()
    //       , * iEndIt  = iMb.end()
    //       ; (oIt != oEndIt) && (iIt != iEndIt)
    //       ; oIt = oMb.advance(oIt, sizeof(U))
    //       , iIt = iMb.advance(iIt, sizeof(U))
    //       )
    //   {
    //     construct<U>(*(reinterpret_cast<const U*>(iIt))); 
    //   }
    // }

    std::list<block> storage_;
  };
#endif // __cplusplus <= 201402L
} // allocator
} //nonstd

// All allocators aren't replaceable since they're statefull, i.e. every allocation/deallocation
// changes inner allocator members
//
template <typename T1, size_t N1, typename T2, size_t N2>
  bool operator==(const nonstd::allocator::humble<T1,N1>&, const nonstd::allocator::humble<T2,N2>&)
{
  return false;
}

template <typename T1, size_t N1, typename T2, size_t N2>
  bool operator!=(const nonstd::allocator::humble<T1,N1>&, const nonstd::allocator::humble<T2,N2>&)
{
  return true;
}
