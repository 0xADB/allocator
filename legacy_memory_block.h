#pragma once

#include "unlikely.h"
#include <cstddef>
#include <utility>
#include <mutex>
#include <atomic>
#include <iostream>

namespace nonstd
{
  namespace legacy
  {

    /**
     * @struct memory_block
     * @brief Non-copyable and non-movable contigious thread-safe memory block holder.
     *
     * Allocates N bytes upon construction, fills space on demand.
     * Deallocates upon destruction.
     */
    struct memory_block
    {
#if __cplusplus >= 201103L && __cplusplus <= 201402L
      enum class byte_type : unsigned char {};
#elif __cplusplus >= 201103L
      using byte_type = std::byte;
#endif

      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;

      byte_type * const _storage = nullptr;     //! block's beginning
      byte_type * const _storage_end = nullptr; //! block's end
      byte_type * _end = nullptr;               //! end of space used at least once
      std::atomic<size_type> _stored{};
      std::atomic<size_type> _refcnt{1};
      mutable std::mutex _mutex{};

#ifdef MEMORY_BLOCK_TRACING
      static std::atomic_int count;
#endif

      memory_block(size_type N)
	: _storage(static_cast<byte_type *>(malloc(N)))
	, _storage_end(_storage + N)
	, _end(_storage)
      {
#ifdef MEMORY_BLOCK_TRACING
	std::cout << __PRETTY_FUNCTION__ << ": " << ++count << std::endl;
#endif
      }

      memory_block(const memory_block& other) = delete;
      memory_block& operator=(const memory_block& other) = delete;
      memory_block(memory_block&& other) = delete;
      memory_block& operator=(memory_block&& other) = delete;

      ~memory_block()
      {
#ifdef MEMORY_BLOCK_TRACING
	std::cout << __PRETTY_FUNCTION__ << ": " << --count << std::endl;
#endif
	std::lock_guard<std::mutex> lock(_mutex);
	if (_storage)
	  free(const_cast<byte_type *>(_storage));
      }

      bool is_pointed_by(const void * p, size_type size = 0) const
      {
	std::lock_guard<std::mutex> lock(_mutex);
	return (initialized()
	    && (_storage <= reinterpret_cast<const byte_type *>(p))
	    && (reinterpret_cast<const byte_type *>(p) < _storage_end)
	    && (size == 0 || reinterpret_cast<const byte_type *>(p) + size <= _storage_end)
	    );
      }

      bool initialized() const
      {
	return (_storage && _storage_end);
      }

      bool empty() const
      {
	return (_stored.load() == 0);
      }

      size_type size() const
      {
	return _stored.load();
      }

      void * allocate(size_type n)
      {
	std::lock_guard<std::mutex> lock(_mutex);
	void * p = nullptr;
	if (_end + n <= _storage_end)
	{
	  p = _end;
	  _end += n;
	  _stored += n;
#ifdef MEMORY_BLOCK_TRACING
	  std::cout << __PRETTY_FUNCTION__ << ": allocated: " << n << std::endl;
#endif
	}
#ifdef MEMORY_BLOCK_TRACING
	else
	{
	  std::cout << __PRETTY_FUNCTION__ << ": no room" << std::endl;
	}
#endif
	return p;
      }

      bool deallocate(void * p, size_type size)
      {
	std::lock_guard<std::mutex> lock(_mutex);
	if (initialized() && _stored.load() && _storage <= p && p < _end)
	{
#ifdef MEMORY_BLOCK_TRACING
	  std::cout << __PRETTY_FUNCTION__ << ": " << size << std::endl;
#endif
	  _stored -= size;
	  return true;
	}
#ifdef MEMORY_BLOCK_TRACING
	std::cout << __PRETTY_FUNCTION__ << ": wrong pointer or size" << std::endl;
#endif
	return false;
      }
    };

  } // memory
} // nonstd
